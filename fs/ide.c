/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>


 static int lpmap[32]= {0}; //块号映射表
static int rwmap[32]= {0}; //物理块位图， 0可写，1不可写
 static int pcount[32]= {0};  //物理块累计擦除次数

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_ID, 4));  //设置下一次读写的磁盘编号(写入diskno)
		uint32_t temp_off = begin + off;
		panic_on(syscall_write_dev(&temp_off, DEV_DISK_ADDRESS | DEV_DISK_OFFSET, 4));  //设置下一次磁盘镜像偏移字节数(写入off)
		u_int opt = DEV_DISK_OPERATION_READ;
		panic_on(syscall_write_dev(&opt, DEV_DISK_ADDRESS | DEV_DISK_START_OPERATION, 4)); //写入0开始读磁盘
		syscall_read_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_STATUS, 4);  //读上一次状态返回值(0是失败)
		if(temp == 0) {
			panic_on("fail to read.");
		}
		panic_on(syscall_read_dev(dst+off, DEV_DISK_ADDRESS | DEV_DISK_BUFFER, BY2SECT)); //将对应off扇区的512bytes数据从设备缓冲区读入目标位置

	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		uint32_t temp_off = begin + off;
		panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_ID, 4));
		panic_on(syscall_write_dev(&temp_off, DEV_DISK_ADDRESS | DEV_DISK_OFFSET, 4));
		panic_on(syscall_write_dev(src+off, DEV_DISK_ADDRESS | DEV_DISK_BUFFER, BY2SECT)); //先将对应扇区512bytes写入数据缓冲区
		u_int opt = DEV_DISK_OPERATION_WRITE;
		panic_on(syscall_write_dev(&opt, DEV_DISK_ADDRESS | DEV_DISK_START_OPERATION, 4)); //再写入1启动写磁盘操作
		syscall_read_dev(&temp, DEV_DISK_ADDRESS | DEV_DISK_STATUS, 4);
		if(temp == 0) {
			panic_on("fail to write.");
		}

	}
}

//  ↓    //
/*
int lpmap[32]; //块号映射表
 int rwmap[32]; //物理块位图， 0可写，1不可写
 int pcount[32];  //物理块累计擦除次数
*/

void ssd_init() {
	int i;
	for(i=0; i<32; i++) {
		lpmap[i] = 0xFFFFFFFF;
		rwmap[i] = 0;
		pcount[i] = 0;
//		debugf("hello");
	}
}

int ssd_read(u_int logic_no, void *dst) {
	if(lpmap[logic_no] == 0xFFFFFFFF) {
		return -1;
	} else {
		int phy_no = lpmap[logic_no];
		ide_read(0, phy_no, dst, 1);
		return 0;
	}
}

void ssd_write(u_int logic_no, void *src) {
	if(lpmap[logic_no] == 0XFFFFFFFF) {
		int phy = alloc_phy();
		lpmap[logic_no] = phy;
		ide_write(0, phy, src, 1);
		rwmap[phy] = 1;
		//debugf("hello%d", phy);
	} else {
		int phy_no = lpmap[logic_no];
		remove_phy(phy_no);
		int phy = alloc_phy();
		lpmap[logic_no] = phy;
		ide_write(0, phy, src, 1);
		rwmap[phy] = 1;
	}
}

void ssd_erase(u_int logic_no) {
	if(lpmap[logic_no] != 0XFFFFFFFF) {
		int phy = lpmap[logic_no];
		remove_phy(phy);
		lpmap[logic_no] = 0XFFFFFFFF;
	}
}

//

void remove_phy(u_int number) {
	int num = 5;
	int *a = &num;
	memset(a, 0, BY2SECT);
	ide_write(0, number, a, 1);
	rwmap[number] = 0;
	pcount[number]++;
	//debugf("hello");
} //擦除物理块

int alloc_phy() {
	int min ;
	int i;
	for(i=0; i<32; i++) {
		if(rwmap[i] == 0) {
			min = i;
			break;
		}
	}
	for(i=0; i<32; i++) {
		if(rwmap[i] == 0) {
			if(pcount[i] < pcount[min] || (pcount[i] == pcount[min] && i < min)) {
				min = i;
			}
		}
	}
	if(pcount[min] < 5) {
		return min;
	}else {
		int b;
		int j;
		for(j=0; j<32; j++) {
			if(rwmap[j] == 1) {
				b = j;
				break;
			}
		}
		for(j=0; j<32; j++) {
			if(rwmap[j] == 1) {
				if(pcount[j] < pcount[b] || (pcount[j] == pcount[b] && j < b)) {
					b = j;
				}
			}
		}
		void* tmp;
		ide_read(0, b, tmp, 1);
		ide_write(0, min, tmp, 1);
		rwmap[min] = 1;
		int logic;
		int k;
		for(k = 0; k<32; k++) {
			if(lpmap[k] == b) {
				lpmap[k] = min;
				break;
			}
		}

		remove_phy(b);
		return b;
	}

} //分配物理块













