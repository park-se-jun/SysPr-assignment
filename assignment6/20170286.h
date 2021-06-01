#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ucontext.h>

#define STACK_SIZE 8192
typedef struct tcb_t {
	struct tcb_t *next;
	// struct tcb_t *prev;
	ucontext_t   context;
	int threadid;
}tcb_t;

tcb_t *thread_queue = 0;
tcb_t *curr_thread = 0;
int g_tid = 1;

// addQueue(head, item) adds a queue item, pointed to by “item”, to the queue pointed to by head.
void addQueue(tcb_t **head, tcb_t *item) {
	// your code
	tcb_t *current = *head;
	if(item ==NULL){
		return;
	}
	if(current ==NULL){
		*head = item;
		// item->prev= NULL;
		item->next = NULL;
		return;
	}
	else{
		while(current->next){
			current = current->next;
		}
		current->next = item;
		// item->prev = current;
		return;
	}
}

// delQueue(head) deletes an item from head and returns a pointer to the deleted item. If the queue is already empty, flag error.
tcb_t *delQueue(tcb_t **head) {
	// your code
	tcb_t *return_thread= *head;
	if(return_thread == NULL)
		return NULL;
	else{
		*head = return_thread->next;
		// (*head) -> prev = NULL;
		return_thread -> next = NULL;
		return return_thread;
		
	}	
}



void init_TCB (tcb_t *tcb, void *function, void *stackP, int stack_size) {
	memset(tcb,'\0', sizeof(tcb_t));
	getcontext(&tcb->context);
	tcb->context.uc_stack.ss_sp = stackP;
	tcb->context.uc_stack.ss_size = (size_t) stack_size;
	makecontext(&tcb->context,function,0);
}


void create_thread(void (*function)(void)) {//만들어진 순서대로 큐에 쓰레드가 들어가있다.
	void *stack = (void *)malloc(STACK_SIZE);//쓰레드에 들어갈 스택
	tcb_t *tcb = (tcb_t *)malloc(sizeof(tcb_t));
	init_TCB(tcb, function, stack, STACK_SIZE);//쓰레드 초기화
	tcb->threadid = g_tid++;//만든 쓰레드의 id 설정
	addQueue(&thread_queue, tcb);//큐에 쓰레드 넣어 놓기
}

void run() {
	curr_thread = delQueue(&thread_queue);//큐의 가장 앞 쓰레드
	ucontext_t parent; 
	getcontext(&parent); // magic sauce
	swapcontext(&parent, &(curr_thread->context)); // start the first thread // 큐의 가장앞 쓰레드 실행
}

void yield() {  
	tcb_t *prev_thread;//이전쓰레드
	addQueue(&thread_queue, curr_thread); //현재쓰레드를 큐에 넣기
	prev_thread = curr_thread;//큐에들어간 쓰레드는 이전 쓰레드가 된다
	curr_thread = delQueue(&thread_queue);//큐에서 다음 쓰레드를 뺀다
	//swap the context, from prev_thread to the thread pointed to curr_thread
	swapcontext(&(prev_thread->context), &(curr_thread->context));//큐에서 꺼낸 쓰레드로 컨택스트 전달
}

int get_thread_id() {
	return curr_thread->threadid;
}

/* Semaphore Implementation */

typedef struct semaphore_t {
	int counter;
	tcb_t *sem_q;
} semaphore_t;


// mallocs a semaphore structure, initializes it to the value and returns the pointer to the semaphore.
semaphore_t sem_create(int value) {
	semaphore_t* sem = (semaphore_t*) malloc(sizeof(semaphore_t));
	sem->counter = value;
	sem->sem_q = NULL;
	return *sem;
}

// decrements the semaphore. 
// if the value is <= 0, then blocks the thread in the queue associated with the semaphore.
void P(semaphore_t *sem) {

	// your code
		while(sem->counter<=0){
			addQueue(&(sem->sem_q),curr_thread);
			yield();
		}
		sem->counter--;
}

// increments the semaphore.
// if the value => 1, then takes a PCB out of the semaphore queue and puts it into the thread_queue. 
// Note: The V routine also "yields" to the next runnable thread.
void V(semaphore_t *sem) {
	sem->counter++;
	if(sem->counter >=1)
	{
		tcb_t* t = delQueue(&(sem->sem_q));
		addQueue(&thread_queue,t);

	}
	// your code
	yield();
}
