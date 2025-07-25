#pragma once
#ifndef SCRIPT_H
#define SCRIPT_H

/*
 * Copyright 2021 Sandro Magi
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Pulled from: https://github.com/naasking/async.h
 * 
 * Author: Sandro Magi <naasking@gmail.com>
 */

/**
 * @file async.h
 * Stackless Async Subroutines
 *
 * Taking inspiration from protothreads and async/await as found in C#, Rust and JS,
 * this is an async/await implementation for C based on Duff's device.
 *
 * Features:
 *
 * 1. Subroutines can have persistent state that isn't just static state, because
 *    each async subroutine accepts its own struct it uses as a parameter, and
 *    the async state is stored there.
 * 2. Because of the more flexible state handling, async subroutines can be nested
 *    in tree-like fashion which permits fork/join concurrency patterns.
 *
 * Caveats:
 *
 * 1. Due to compile-time bug, MSVC requires changing: 
 *      Project Properties > Configuration Properties > C/C++ > General > Debug Information Format
 *    from "Program Database for Edit And Continue" to "Program Database".
 * 2. As with protothreads, you have to be very careful with switch statements within an async
 *    subroutine. Rule of thumb: just place all switch statements in their own function.
 * 3. As with protothreads, you can't make blocking system calls and preserve the async semantics.
 *    These must be changed into non-blocking calls that test a condition.
 */

//#include <limits.h>
#include <stdint.h>
#include "async.h"
/**
 * The async computation status
 */
//typedef enum ASYNC_EVT { ASYNC_INIT = 0, ASYNC_CONT = ASYNC_INIT, ASYNC_DONE = 1 } async;

/**
 * Declare the async state
 */
//#define async_state uint32_t _async_k

/**
 * Core async structure, optional to use.
 */
//struct async { async_state; };

/**
 * Mark the start of an async subroutine
 *
 * @param k The async state
 */
#define script_begin_with(k) unsigned *_async_k = &(k)->_async_k; switch(*_async_k) { default:

/**
 * Mark the end of a async subroutine
 */
#define script_end_simple *_async_k=ASYNC_DONE; case ASYNC_DONE: break /*ASYNC_DONE*/; }

/**
 * Wait until the condition succeeds
 * @param cond The condition that must be satisfied before execution can proceed
 */
#define script_await(cond) script_await_while(!(cond))

/**
 * Wait while the condition succeeds
 * @param cond The condition that must fail before execution can proceed
 */
#define script_await_while(cond) *_async_k = __LINE__; case __LINE__: if (cond) break /*ASYNC_CONT*/

/**
 * Yield execution
 */
#define script_yield *_async_k = __LINE__; break /*ASYNC_CONT*/; case __LINE__:

/**
 * Exit the current async subroutine
 */
#define script_exit *_async_k = ASYNC_DONE; break /*ASYNC_DONE*/

/**
 * Initialize a new async computation
 * @param state The async procedure state to initialize
 */
#define script_init(state) (state)->_async_k=ASYNC_INIT

/**
 * Check if async subroutine is done
 * @param state The async procedure state to check
 */
#define script_done(state) (state)->_async_k==ASYNC_DONE

#define script_not_done(state) (state)->_async_k!=ASYNC_DONE

/**
 * Resume a running async computation and check for completion
 *
 * Calling the function itself will return true if the async call is complete,
 * or false if it's still in progress.
 * @param f The async procedure
 * @param state The async procedure state
 */
//#define async_call(f, state) (async_done(state) || (f)(state))

#define script_end_and_init *_async_k=ASYNC_INIT; case ASYNC_DONE: /*return ASYNC_DONE*/break; }

#define script_begin \
    static struct async asyncState = { ASYNC_INIT }; \
    script_begin_with(&asyncState);

#define script_end script_end_and_init


/*
#define FRAME yield(0)
#define WAIT_FRAMES(X) \
    int _coroutine_frame_delay = (X); \
    while(_coroutine_frame_delay-- > 0) {  yield(0); }\

#define repeat do
#define until(exp) while(!(exp))

#define waitUntil(X) repeat { FRAME; } until (X);
#define waitForInput(X) waitUntil(X);
*/

//#define await_frames(X) \
    //int _coroutine_frame_delay = (X); \ // you can't declare variables here
    //await_while(_coroutine_frame_delay-- > 0)


#endif