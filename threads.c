#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>

const int time_one = 1;
const int time_two = 2;
int i_one = 0;
int i_two = 0;
int k = 0;

ucontext_t context_one, context_two;

void switch_to_two(int sig);

void switch_to_one(int sig) {
    signal(SIGALRM, switch_to_two);
    alarm(time_one);
    swapcontext(&context_two, &context_one);
}

void switch_to_two(int sig) {
    signal(SIGALRM, switch_to_one);
    alarm(time_two);
    swapcontext(&context_one, &context_two);
}

void thread_one(void) {
	while (1) {
		if (k == 0) {
			printf("One: %d\n", i_one);
			k=1;
		}
		i_one++;
    }
}

void thread_two(void) {
	while (1) {
		if (k == 1) {
			printf("Two: %d\n", i_two);
			k=0;
		}
		i_two++;
    }
}

void init_thread_context(ucontext_t *context, void (*thread_func)(void)) {
    getcontext(context);
    context->uc_stack.ss_sp = malloc(SIGSTKSZ);
    context->uc_stack.ss_size = SIGSTKSZ;
    makecontext(context, thread_func, 0);
}

int main(void) {
    init_thread_context(&context_one, thread_one);
    init_thread_context(&context_two, thread_two);
    signal(SIGALRM, switch_to_two);
    alarm(time_one);
    setcontext(&context_one);
    return EXIT_SUCCESS;
}