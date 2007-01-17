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

/** @addtogroup ia64	
 * @{
 */
/** @file
 */

#ifndef KERN_ia64_ASM_H_
#define KERN_ia64_ASM_H_

#include <config.h>
#include <arch/types.h>
#include <arch/register.h>

/** Return base address of current stack
 *
 * Return the base address of the current stack.
 * The stack is assumed to be STACK_SIZE long.
 * The stack must start on page boundary.
 */
static inline uintptr_t get_stack_base(void)
{
	uint64_t v;

	__asm__ volatile ("and %0 = %1, r12" : "=r" (v) : "r" (~(STACK_SIZE-1)));
	
	return v;
}

/** Return Processor State Register.
 *
 * @return PSR.
 */
static inline uint64_t psr_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = psr\n" : "=r" (v));
	
	return v;
}

/** Read IVA (Interruption Vector Address).
 *
 * @return Return location of interruption vector table.
 */
static inline uint64_t iva_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = cr.iva\n" : "=r" (v));
	
	return v;
}

/** Write IVA (Interruption Vector Address) register.
 *
 * @param v New location of interruption vector table.
 */
static inline void iva_write(uint64_t v)
{
	__asm__ volatile ("mov cr.iva = %0\n" : : "r" (v));
}


/** Read IVR (External Interrupt Vector Register).
 *
 * @return Highest priority, pending, unmasked external interrupt vector.
 */
static inline uint64_t ivr_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = cr.ivr\n" : "=r" (v));
	
	return v;
}

/** Write ITC (Interval Timer Counter) register.
 *
 * @param v New counter value.
 */
static inline void itc_write(uint64_t v)
{
	__asm__ volatile ("mov ar.itc = %0\n" : : "r" (v));
}

/** Read ITC (Interval Timer Counter) register.
 *
 * @return Current counter value.
 */
static inline uint64_t itc_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = ar.itc\n" : "=r" (v));
	
	return v;
}

/** Write ITM (Interval Timer Match) register.
 *
 * @param v New match value.
 */
static inline void itm_write(uint64_t v)
{
	__asm__ volatile ("mov cr.itm = %0\n" : : "r" (v));
}

/** Read ITM (Interval Timer Match) register.
 *
 * @return Match value.
 */
static inline uint64_t itm_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = cr.itm\n" : "=r" (v));
	
	return v;
}

/** Read ITV (Interval Timer Vector) register.
 *
 * @return Current vector and mask bit.
 */
static inline uint64_t itv_read(void)
{
	uint64_t v;
	
	__asm__ volatile ("mov %0 = cr.itv\n" : "=r" (v));
	
	return v;
}

/** Write ITV (Interval Timer Vector) register.
 *
 * @param v New vector and mask bit.
 */
static inline void itv_write(uint64_t v)
{
	__asm__ volatile ("mov cr.itv = %0\n" : : "r" (v));
}

/** Write EOI (End Of Interrupt) register.
 *
 * @param v This value is ignored.
 */
static inline void eoi_write(uint64_t v)
{
	__asm__ volatile ("mov cr.eoi = %0\n" : : "r" (v));
}

/** Read TPR (Task Priority Register).
 *
 * @return Current value of TPR.
 */
static inline uint64_t tpr_read(void)
{
	uint64_t v;

	__asm__ volatile ("mov %0 = cr.tpr\n"  : "=r" (v));
	
	return v;
}

/** Write TPR (Task Priority Register).
 *
 * @param v New value of TPR.
 */
static inline void tpr_write(uint64_t v)
{
	__asm__ volatile ("mov cr.tpr = %0\n" : : "r" (v));
}

/** Disable interrupts.
 *
 * Disable interrupts and return previous
 * value of PSR.
 *
 * @return Old interrupt priority level.
 */
static ipl_t interrupts_disable(void)
{
	uint64_t v;
	
	__asm__ volatile (
		"mov %0 = psr\n"
		"rsm %1\n"
		: "=r" (v)
		: "i" (PSR_I_MASK)
	);
	
	return (ipl_t) v;
}

/** Enable interrupts.
 *
 * Enable interrupts and return previous
 * value of PSR.
 *
 * @return Old interrupt priority level.
 */
static ipl_t interrupts_enable(void)
{
	uint64_t v;
	
	__asm__ volatile (
		"mov %0 = psr\n"
		"ssm %1\n"
		";;\n"
		"srlz.d\n"
		: "=r" (v)
		: "i" (PSR_I_MASK)
	);
	
	return (ipl_t) v;
}

/** Restore interrupt priority level.
 *
 * Restore PSR.
 *
 * @param ipl Saved interrupt priority level.
 */
static inline void interrupts_restore(ipl_t ipl)
{
	if (ipl & PSR_I_MASK)
		(void) interrupts_enable();
	else
		(void) interrupts_disable();
}

/** Return interrupt priority level.
 *
 * @return PSR.
 */
static inline ipl_t interrupts_read(void)
{
	return (ipl_t) psr_read();
}

/** Disable protection key checking. */
static inline void pk_disable(void)
{
	__asm__ volatile ("rsm %0\n" : : "i" (PSR_PK_MASK));
}

extern void cpu_halt(void);
extern void cpu_sleep(void);
extern void asm_delay_loop(uint32_t t);

extern void switch_to_userspace(uintptr_t entry, uintptr_t sp, uintptr_t bsp, uintptr_t uspace_uarg, uint64_t ipsr, uint64_t rsc);

#endif

/** @}
 */
