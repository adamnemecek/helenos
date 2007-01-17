/*
 * Copyright (c) 2006 Ondrej Palkovsky
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

/** @addtogroup libcmips32	
 * @{
 */
/** @file
 * @ingroup libcmips32eb	
 */

#ifndef LIBC_mips32_PSTHREAD_H_
#define LIBC_mips32_PSTHREAD_H_

#include <types.h>

/* We define our own context_set, because we need to set
 * the TLS pointer to the tcb+0x7000
 *
 * See tls_set in thread.h
 */
#define context_set(c, _pc, stack, size, ptls) 			\
	(c)->pc = (sysarg_t) (_pc);				\
	(c)->sp = ((sysarg_t) (stack)) + (size) - SP_DELTA; 	\
        (c)->tls = ((sysarg_t)(ptls)) + 0x7000 + sizeof(tcb_t);


/* +16 is just for sure that the called function
 * have space to store it's arguments
 */
#define SP_DELTA	(8+16)

typedef struct  {
	uint32_t sp;
	uint32_t pc;
	
	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t gp;
	uint32_t tls; /* Thread local storage(=k1) */

	uint32_t f20;
	uint32_t f21;
	uint32_t f22;
	uint32_t f23;
	uint32_t f24;
	uint32_t f25;
	uint32_t f26;
	uint32_t f27;
	uint32_t f28;
	uint32_t f29;
	uint32_t f30;
	
} context_t;

#endif

/** @}
 */
