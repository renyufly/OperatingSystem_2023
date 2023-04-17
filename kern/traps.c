#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);
extern struct Env *curenv;

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	// 你需要在此处实现问题描述的处理要求
	curenv->env_ov_cnt++;
	unsigned long tmp_epc =  tf->cp0_epc;
	Pte* pg = KADDR(PTE_ADDR(*curenv->env_pgdir));
	if((*(pg) & 0x20) == 0x20 ) {  // add_exception
		unsigned long assm = *pg;
		*pg = assm | 0x1; 
		printk("add ov handled\n");
	} else if((*(pg) & 0x22) == 0x22) {   //sub_excpetion
		unsigned long assm = *pg;
		*pg = assm | 0x1; 
		printk("sub ov handled\n");
	} else {
		unsigned long assm = *pg;
		unsigned long t = (assm & 0x1F0000) >> 16;
		unsigned long s = (assm & 0x3E00000) >> 21;
		unsigned long imm = (assm & 0xFFFF);
		tf->regs[t] = tf->regs[s]/2 + imm/2;
		tf->cp0_epc += 4;
		printk("addi ov handled\n");
	}
	return;
}
