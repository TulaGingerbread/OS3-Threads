#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <sys/time.h>
 
int alarmed = 0;

void onalarm(nsig) {
        alarmed = 1;
        alarm(1);
}

void print1(ucontext_t *context1, ucontext_t *context2) {
        while (1) {
                printf("Thread #1\n");
                swapcontext(context1, context2);
        }
}

void print2(ucontext_t *context1, ucontext_t *context2) {
        while (1) {
                printf("Thread #2\n");
                swapcontext(context1, context2);
        }
}

int threadCount = 0;
ucontext_t threads[10];
char stacks[10][18000];

void addThread(void (*func)(ucontext_t*,ucontext_t*), ucontext_t *back, ucontext_t *finish) {
        threads[threadCount].uc_link = finish;
        threads[threadCount].uc_stack.ss_sp = stacks[threadCount];
        threads[threadCount].uc_stack.ss_size = sizeof(stacks[threadCount]);
        getcontext(&threads[threadCount]);
        makecontext(&threads[threadCount], (void (*)(void)) func, 2, &threads[threadCount], back);
        threadCount++;
}

ucontext_t main_context1, main_context2;

int main(void) {
        signal(SIGALRM, onalarm);
        alarm(1);
        
        addThread(print1, &main_context2, &main_context1);
        addThread(print2, &main_context2, &main_context1);
        
        getcontext(&main_context1);
        
        int turn = 0;
        
        while (1) {
                if (alarmed) {
                        alarmed = 0;
			turn++;
			if (turn%2 == 0) swapcontext(&main_context2, &threads[1]);
			swapcontext(&main_context2, &threads[0]);
                }
        }
        return 0;
}
