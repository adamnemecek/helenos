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

/** @addtogroup ia32	
 * @{
 */
/** @file
 */

#ifndef KERN_ia32_PM_H_
#define KERN_ia32_PM_H_

#define IDT_ITEMS 64
#define GDT_ITEMS 7

#define VESA_INIT_SEGMENT 0x8000

#define NULL_DES	0
#define KTEXT_DES	1
#define	KDATA_DES	2
#define UTEXT_DES	3
#define UDATA_DES	4
#define TSS_DES		5
#define TLS_DES		6 /* Pointer to Thread-Local-Storage data */

#ifdef CONFIG_FB

#define VESA_INIT_SEGMENT 0x8000
#define VESA_INIT_DES 7
#undef GDT_ITEMS
#define GDT_ITEMS 8

#endif /* CONFIG_FB */


#define selector(des)	((des) << 3)

#define PL_KERNEL	0
#define PL_USER		3

#define AR_PRESENT	(1<<7)
#define AR_DATA		(2<<3)
#define AR_CODE		(3<<3)
#define AR_WRITABLE	(1<<1)
#define AR_INTERRUPT	(0xe)
#define AR_TSS		(0x9)

#define DPL_KERNEL	(PL_KERNEL<<5)
#define DPL_USER	(PL_USER<<5)

#define TSS_BASIC_SIZE	104
#define TSS_IOMAP_SIZE	(16*1024+1)	/* 16K for bitmap + 1 terminating byte for convenience */

#define IO_PORTS	(64*1024)

#ifndef __ASM__

#include <arch/types.h>
#include <typedefs.h>
#include <arch/context.h>

struct ptr_16_32 {
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));
typedef struct ptr_16_32 ptr_16_32_t;

struct descriptor {
	unsigned limit_0_15: 16;
	unsigned base_0_15: 16;
	unsigned base_16_23: 8;
	unsigned access: 8;
	unsigned limit_16_19: 4;
	unsigned available: 1;
	unsigned unused: 1;
	unsigned special: 1;
	unsigned granularity : 1;
	unsigned base_24_31: 8;
} __attribute__ ((packed));
typedef struct descriptor  descriptor_t;

struct idescriptor {
	unsigned offset_0_15: 16;
	unsigned selector: 16;
	unsigned unused: 8;
	unsigned access: 8;
	unsigned offset_16_31: 16;
} __attribute__ ((packed));
typedef struct idescriptor idescriptor_t;

struct tss {
	uint16_t link;
	unsigned : 16;
	uint32_t esp0;
	uint16_t ss0;
	unsigned : 16;
	uint32_t esp1;
	uint16_t ss1;
	unsigned : 16;
	uint32_t esp2;
	uint16_t ss2;
	unsigned : 16;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es;
	unsigned : 16;
	uint16_t cs;
	unsigned : 16;
	uint16_t ss;
	unsigned : 16;
	uint16_t ds;
	unsigned : 16;
	uint16_t fs;
	unsigned : 16;
	uint16_t gs;
	unsigned : 16;
	uint16_t ldtr;
	unsigned : 16;
	unsigned : 16;
	uint16_t iomap_base;
	uint8_t iomap[TSS_IOMAP_SIZE];
} __attribute__ ((packed));
typedef struct tss tss_t;

extern ptr_16_32_t gdtr;
extern ptr_16_32_t bootstrap_gdtr;
extern ptr_16_32_t protected_ap_gdtr;
extern struct tss *tss_p;

extern descriptor_t gdt[];

extern void pm_init(void);

extern void gdt_setbase(descriptor_t *d, uintptr_t base);
extern void gdt_setlimit(descriptor_t *d, uint32_t limit);

extern void idt_init(void);
extern void idt_setoffset(idescriptor_t *d, uintptr_t offset);

extern void tss_initialize(tss_t *t);
extern void set_tls_desc(uintptr_t tls);

#endif /* __ASM__ */

#endif

/** @}
 */
