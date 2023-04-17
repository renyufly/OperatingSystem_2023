#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	 static int user_time[5]; // 创建一个用户累计运行时间片数数组
	int user[5] = {0};
	struct Env *tmp = NULL;
	TAILQ_FOREACH( tmp,&env_sched_list, env_sched_link) {
		user[tmp->env_user]++;
	}

	/* Exercise 3.12: Your code here. */
	if(yield || (count == 0) || (e == NULL) || (e->env_status != ENV_RUNNABLE)) {
		if(e && e->env_status == ENV_RUNNABLE) {
			TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
			user_time[e->env_user] += e->env_pri;
		}

		if(TAILQ_EMPTY(&env_sched_list)) {
			panic("no runnable envs");
		}

		int next_user;   //next user to run
		int i;
		for(i=0; i<5; i++) {
			if(user[i] != 0) {
				next_user = i;
				break;	
			}
		}
		int j;
		for(j=0; j<5; j++) {
			if(user[j] != 0) {
				if(user_time[j] < user_time[next_user]) {
					next_user = j;
				} else if(user_time[j] == user_time[next_user]) {
					if(j < next_user) {
						next_user = j;
					}
				}
			}
		}
		struct Env *temp = NULL;
		TAILQ_FOREACH(temp, &env_sched_list, env_sched_link) {
			if(temp->env_user == next_user) {
				e = temp;
				break;	
			}
		}
		count = e->env_pri;
	}
		count--;
		env_run(e);

}
