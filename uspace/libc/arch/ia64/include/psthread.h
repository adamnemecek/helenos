/*
 * Copyright (c) 2005 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup libcia64	
 * @{
 */
/** @file
 */

#ifndef LIBC_ia64_PSTHREAD_H_
#define LIBC_ia64_PSTHREAD_H_

#include <types.h>
#include <align.h>
#include <libarch/stack.h>
#include <libarch/types.h>

/*
 * context_save() and context_restore() are both leaf procedures.
 * No need to allocate scratch area.
 */
#define SP_DELTA	(0+ALIGN_UP(STACK_ITEM_SIZE, STACK_ALIGNMENT))

#define PFM_MASK        (~0x3fffffffff)

#define PSTHREAD_INITIAL_STACK_PAGES_NO 2
/* Stack is divided into two equal parts (for memory stack and register stack). */
#define PSTHREAD_INITIAL_STACK_DIVISION 2  

#ifdef context_set
#undef context_set
#endif

#define context_set(c, _pc, stack, size, tls) 								\
	do {												\
		(c)->pc = (uint64_t) _pc;								\
		(c)->bsp = ((uint64_t) stack) + size / PSTHREAD_INITIAL_STACK_DIVISION;								\
		(c)->ar_pfs &= PFM_MASK; 								\
		(c)->sp = ((uint64_t) stack) + ALIGN_UP((size / PSTHREAD_INITIAL_STACK_DIVISION), STACK_ALIGNMENT) - SP_DELTA;		\
		(c)->tp = (uint64_t) tls;								\
	} while (0);
	

/*
 * Only save registers that must be preserved across
 * function calls.
 */
typedef struct context {

	/*
	 * Application registers
	 */
	uint64_t ar_pfs;
	uint64_t ar_unat_caller;
	uint64_t ar_unat_callee;
	uint64_t ar_rsc;
	uint64_t bsp;		/* ar_bsp */
	uint64_t ar_rnat;
	uint64_t ar_lc;

	/*
	 * General registers
	 */
	uint64_t r1;
	uint64_t r4;
	uint64_t r5;
	uint64_t r6;
	uint64_t r7;
	uint64_t sp;		/* r12 */
	uint64_t tp;		/* r13 */
	
	/*
	 * Branch registers
	 */
	uint64_t pc;		/* b0 */
	uint64_t b1;
	uint64_t b2;
	uint64_t b3;
	uint64_t b4;
	uint64_t b5;

	/*
	 * Predicate registers
	 */
	uint64_t pr;

	__r128 f2 __attribute__ ((aligned(16)));
	__r128 f3;
	__r128 f4;
	__r128 f5;

	__r128 f16;
	__r128 f17;
	__r128 f18;
	__r128 f19;
	__r128 f20;
	__r128 f21;
	__r128 f22;
	__r128 f23;
	__r128 f24;
	__r128 f25;
	__r128 f26;
	__r128 f27;
	__r128 f28;
	__r128 f29;
	__r128 f30;
	__r128 f31;

} context_t;

#endif

/** @}
 */
