// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

//
u_int get_time(u_int *us) {
	int time = 0;
	syscall_read_dev(&time, 0x15000000, 4);
	syscall_read_dev(&time, 0x15000010, 4);
	syscall_read_dev(us,    0x15000020, 4);
	return time;
}

void usleep(u_int us) {
	u_int ms;
	u_int entry_time = get_time(&ms);
	// 读取进程进入 usleep 函数的时间
	while (1) {
		// 读取当前时间
		u_int cur_ms;
		u_int cur_time = get_time(&cur_ms);
		if ((((int)cur_time-(int)entry_time)*1000000 + ((int)(cur_ms)-(int)(ms))) >= us/* 当前时间 >= 进入时间 + us 微秒*/) {
		//	debugf("hello:%d", *cur_ms);
			return;
		} else {
			// 进程切换
			syscall_yield();
		}
	}
}
