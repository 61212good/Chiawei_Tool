// Thread_test
#include <pthread.h>
#include <stdio.h>
int value = 0;
void *runner_1(void *param); /* the thread */
void *runner_2(void *param); /* the thread */
int main(int argc, char *argv[])
{
	pid_t pid;
	pthread_t tid_1, tid_2;
	pthread_attr_t attr_1, attr_2;
	pid = fork();
	if (pid == 0) { /* child process */
		pthread_attr_init(&attr_1);
		pthread_create(&tid_1,&attr_1,runner_1,NULL);
		pthread_attr_init(&attr_2);
		pthread_create(&tid_2,&attr_2,runner_2,NULL);
		pthread_join(tid_2,NULL);
		pthread_join(tid_1,NULL);
		printf("CHILD: value = %d",value); /* LINE C */
	}
	else if (pid > 0) { /* parent process */
		wait(NULL);
		printf("PARENT: value = %d",value); /* LINE P */
	}
}
void *runner_1(void *param) {
	value = 5;
	printf("CHILD_theard_1: value = %d",value);
	pthread_exit(0);
}
void *runner_2(void *param) {
	// value = 5;
	printf("CHILD_theard_2: value = %d",value);
	pthread_exit(0);
}