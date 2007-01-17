/*
 * Copyright (c) 2001-2004 Jakub Jermar
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

/** @addtogroup ia32interrupt
 * @{
 */
/** @file
 */

#ifndef KERN_ia32_INTERRUPT_H_
#define KERN_ia32_INTERRUPT_H_

#include <arch/types.h>
#include <arch/pm.h>

#define IVT_ITEMS		IDT_ITEMS
#define IVT_FIRST		0

#define EXC_COUNT	32
#define IRQ_COUNT	16

#define IVT_EXCBASE		0
#define IVT_IRQBASE		(IVT_EXCBASE + EXC_COUNT)
#define IVT_FREEBASE	(IVT_IRQBASE + IRQ_COUNT)

#define IRQ_CLK			0
#define IRQ_KBD			1
#define IRQ_PIC1		2
#define IRQ_PIC_SPUR	7
#define IRQ_MOUSE		12

/* this one must have four least significant bits set to ones */
#define VECTOR_APIC_SPUR	(IVT_ITEMS - 1)

#if (((VECTOR_APIC_SPUR + 1) % 16) || VECTOR_APIC_SPUR >= IVT_ITEMS)
#error Wrong definition of VECTOR_APIC_SPUR
#endif

#define VECTOR_DEBUG				1
#define VECTOR_CLK					(IVT_IRQBASE + IRQ_CLK)
#define VECTOR_PIC_SPUR				(IVT_IRQBASE + IRQ_PIC_SPUR)
#define VECTOR_SYSCALL				IVT_FREEBASE
#define VECTOR_TLB_SHOOTDOWN_IPI	(IVT_FREEBASE + 1)
#define VECTOR_DEBUG_IPI			(IVT_FREEBASE + 2)

struct istate {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t ebx;

	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

	uint32_t error_word;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t stack[];
};

/** Return true if exception happened while in userspace */
static inline int istate_from_uspace(istate_t *istate)
{
	return !(istate->eip & 0x80000000);
}

static inline void istate_set_retaddr(istate_t *istate, uintptr_t retaddr)
{
	istate->eip = retaddr;
}

static inline unative_t istate_get_pc(istate_t *istate)
{
	return istate->eip;
}

extern void (* disable_irqs_function)(uint16_t irqmask);
extern void (* enable_irqs_function)(uint16_t irqmask);
extern void (* eoi_function)(void);

extern void decode_istate(istate_t *istate);
extern void interrupt_init(void);
extern void trap_virtual_enable_irqs(uint16_t irqmask);
extern void trap_virtual_disable_irqs(uint16_t irqmask);

#endif

/** @}
 */
