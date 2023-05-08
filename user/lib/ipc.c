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
void barrier_alloc(int n) {
	syscall_set_barrier(env->env_id, n);	
	syscall_set_tmpbar(env->env_id, 0);
}

void barrier_wait(void) {
//	user_panic("hello");
	u_int bar = syscall_get_barrier(env->env_id);
	u_int tmpbar = syscall_get_tmpbar(env->env_id);
	if(bar == (tmpbar + 1)) {
		syscall_awake(env->env_id);
		syscall_dec_barrier(env->env_id);
	} else if(bar == -1) {

	}
	else {
		syscall_inc_tmpbar(env->env_id);
		
	}

}




