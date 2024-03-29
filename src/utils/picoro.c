/*
 * picoro - minimal coroutines for C.
 * Written by Tony Finch <dot@dotat.at>
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <assert.h>
#include <setjmp.h>

/*#include <longjmp_win64.h>
#define jmp_buf JMP_BUF_WIN64
#define setjmp setjmp_win64
#define longjmp longjmp_win64*/

/*
To make it work in Windows 10 64 bits w64devkit
*/
#ifdef _WIN64
#define setjmp(A) _setjmp(A, NULL)
#endif

//Custom GCC implementation with w64devkit  https://nullprogram.com/blog/2023/02/12/
/*__attribute__((naked,returns_twice))
int my_setjmp(jmp_buf buf)
{
    (void)buf;
    __asm(
        "mov (%rsp), %rax\n"
        "mov %rax,  0(%rcx)\n"
        "lea 8(%rsp), %rax\n"
        "mov %rax,  8(%rcx)\n"
        "mov %rbp, 16(%rcx)\n"
        "mov %rbx, 24(%rcx)\n"
        "mov %rdi, 32(%rcx)\n"
        "mov %rsi, 40(%rcx)\n"
        "mov %r12, 48(%rcx)\n"
        "mov %r13, 56(%rcx)\n"
        "mov %r14, 64(%rcx)\n"
        "mov %r15, 72(%rcx)\n"
        "xor %eax, %eax\n"
        "ret\n"
    );
}

__attribute__((naked,noreturn))
void my_longjmp(jmp_buf buf, int ret)
{
    (void)buf;
    (void)ret;
    __asm(
        "mov 72(%rcx), %r15\n"
        "mov 64(%rcx), %r14\n"
        "mov 56(%rcx), %r13\n"
        "mov 48(%rcx), %r12\n"
        "mov 40(%rcx), %rsi\n"
        "mov 32(%rcx), %rdi\n"
        "mov 24(%rcx), %rbx\n"
        "mov 16(%rcx), %rbp\n"
        "mov  8(%rcx), %rsp\n"
        "mov %edx, %eax\n"
        "jmp *0(%rcx)\n"
    );
}
#define setjmp(A) my_setjmp(A)
#define longjmp(A,B) my_longjmp(A,B)*/


#include <stdlib.h>

#include "picoro.h"

/*
 * Each coroutine has a jmp_buf to hold its context when suspended.
 *
 * There are lists of running and idle coroutines.
 *
 * The coroutine at the head of the running list has the CPU, and all
 * others are suspended inside resume(). The "first" coro object holds
 * the context for the program's initial stack and also ensures that
 * all externally-visible list elements have non-NULL next pointers.
 * (The "first" coroutine isn't exposed to the caller.)
 *
 * The idle list contains coroutines that are suspended in
 * coroutine_main(). After initialization it is never NULL except
 * briefly while coroutine_main() forks a new idle coroutine.
 */
static struct coro {
	struct coro *next;
	jmp_buf state;
} first, *running = &first, *idle;

/*
 * A coroutine can be passed to resume() if
 * it is not on the running or idle lists.
 */
int resumable(coro c) {
	return(c != NULL && c->next == NULL);
}

/*
 * Add a coroutine to a list and return the previous head of the list.
 */
static void push(coro *list, coro c) {
	c->next = *list;
	*list = c;
}

/*
 * Remove a coroutine from a list and return it.
 */
static coro pop(coro *list) {
	coro c = *list;
	*list = c->next;
	c->next = NULL;
	return(c);
}

/*
 * Pass a value and control from one coroutine to another.
 * The current coroutine's state is saved in "me" and the
 * target coroutine is at the head of the "running" list.
 */
static void *pass(coro me, void *arg) {
	static void *saved;
	saved = arg;
	if(!setjmp(me->state))
		longjmp(running->state, 1);
	return(saved);
}

void *resume(coro c, void *arg) {
	assert(resumable(c));
	push(&running, c);
	return(pass(c->next, arg));
}

void *yield(void *arg) {
	return(pass(pop(&running), arg));
}

/* Declare for mutual recursion. */
void coroutine_start(void), coroutine_main(void*);

/*
 * The coroutine constructor function.
 *
 * On the first invocation there are no idle coroutines, so fork the
 * first one, which will immediately yield back to us after becoming
 * idle. When there are idle coroutines, we pass one the function
 * pointer and return the activated coroutine's address.
 */
coro coroutine(void *function(void *arg)) {
	if(idle == NULL && !setjmp(running->state))
		coroutine_start();
	return(resume(pop(&idle), function));
}

/*
 * The main loop for a coroutine is responsible for managing the "idle" list.
 *
 * When we start the idle list is empty, so we put ourself on it to
 * ensure it remains non-NULL. Then we immediately suspend ourself
 * waiting for the first function we are to run. (The head of the
 * running list is the coroutine that forked us.) We pass the stack
 * pointer to prevent it from being optimised away. The first time we
 * are called we will return to the fork in the coroutine()
 * constructor function (above); on subsequent calls we will resume
 * the parent coroutine_main(). In both cases the passed value is
 * lost when pass() longjmp()s to the forking setjmp().
 *
 * When we are resumed, the idle list is empty again, so we fork
 * another coroutine. When the child coroutine_main() passes control
 * back to us, we drop into our main loop.
 *
 * We are now head of the running list with a function to call. We
 * immediately yield a pointer to our context object so our creator
 * can identify us. The creator can then resume us at which point we
 * pass the argument to the function to start executing.
 *
 * When the function returns, we move ourself from the running list to
 * the idle list, before passing the result back to the resumer. (This
 * is just like yield() except for adding the coroutine to the idle
 * list.) We can then only be resumed by the coroutine() constructor
 * function which will put us back on the running list and pass us a
 * new function to call.
 *
 * We do not declare coroutine_main() static to try to stop it being inlined.
 *
 * The conversion between the function pointer and a void pointer is not
 * allowed by ANSI C but we do it anyway.
 */
void coroutine_main(void *ret) {
	void *(*function)(void *arg);
	struct coro me;
	push(&idle, &me);
	function = pass(&me, ret);
	if(!setjmp(running->state))
		coroutine_start();
	for(;;) {
		ret = function(yield(&me));
		push(&idle, pop(&running));
		function = pass(&me, ret);
	}
}

/*
 * Allocate space for the current stack to grow before creating the
 * initial stack frame for the next coroutine.
 */
void coroutine_start(void) {
	char stack[16 * 1024];
	coroutine_main(stack);
}

/* eof */
