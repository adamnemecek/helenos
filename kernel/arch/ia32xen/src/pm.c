/*
 * Copyright (c) 2006 Martin Decky
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

/** @addtogroup ia32xen	
 * @{
 */
/** @file
 */

#include <arch/pm.h>
#include <config.h>
#include <arch/types.h>
#include <typedefs.h>
#include <arch/interrupt.h>
#include <arch/asm.h>
#include <arch/context.h>
#include <panic.h>
#include <arch/mm/page.h>
#include <mm/slab.h>
#include <memstr.h>
#include <arch/boot/boot.h>
#include <interrupt.h>

/*
 * Early ia32xen configuration functions and data structures.
 */

/*
 * We have no use for segmentation so we set up flat mode. In this
 * mode, we use, for each privilege level, two segments spanning the
 * whole memory. One is for code and one is for data.
 *
 * One is for GS register which holds pointer to the TLS thread
 * structure in it's base.
 */
descriptor_t gdt[GDT_ITEMS] = {
	/* NULL descriptor */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* KTEXT descriptor */
	{ 0xffff, 0, 0, AR_PRESENT | AR_CODE | DPL_KERNEL, 0xf, 0, 0, 1, 1, 0 },
	/* KDATA descriptor */
	{ 0xffff, 0, 0, AR_PRESENT | AR_DATA | AR_WRITABLE | DPL_KERNEL, 0xf, 0, 0, 1, 1, 0 },
	/* UTEXT descriptor */
	{ 0xffff, 0, 0, AR_PRESENT | AR_CODE | DPL_USER, 0xf, 0, 0, 1, 1, 0 },
	/* UDATA descriptor */
	{ 0xffff, 0, 0, AR_PRESENT | AR_DATA | AR_WRITABLE | DPL_USER, 0xf, 0, 0, 1, 1, 0 },
	/* TSS descriptor - set up will be completed later */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	/* TLS descriptor */
	{ 0xffff, 0, 0, AR_PRESENT | AR_DATA | AR_WRITABLE | DPL_USER, 0xf, 0, 0, 1, 1, 0 },
};

static trap_info_t traps[IDT_ITEMS + 1];

static tss_t tss;

tss_t *tss_p = NULL;

/* gdtr is changed by kmp before next CPU is initialized */
ptr_16_32_t bootstrap_gdtr = { .limit = sizeof(gdt), .base = KA2PA((uintptr_t) gdt) };
ptr_16_32_t gdtr = { .limit = sizeof(gdt), .base = (uintptr_t) gdt };

void gdt_setbase(descriptor_t *d, uintptr_t base)
{
	d->base_0_15 = base & 0xffff;
	d->base_16_23 = ((base) >> 16) & 0xff;
	d->base_24_31 = ((base) >> 24) & 0xff;
}

void gdt_setlimit(descriptor_t *d, uint32_t limit)
{
	d->limit_0_15 = limit & 0xffff;
	d->limit_16_19 = (limit >> 16) & 0xf;
}

void tss_initialize(tss_t *t)
{
	memsetb((uintptr_t) t, sizeof(struct tss), 0);
}

static void trap(void)
{
}

void traps_init(void)
{
	index_t i;
	
	for (i = 0; i < IDT_ITEMS; i++) {
		traps[i].vector = i;
		
		if (i == VECTOR_SYSCALL)
			traps[i].flags = 3;
		else
			traps[i].flags = 0;
		
		traps[i].cs = XEN_CS;
		traps[i].address = trap;
	}
	traps[IDT_ITEMS].vector = 0;
	traps[IDT_ITEMS].flags = 0;
	traps[IDT_ITEMS].cs = 0;
	traps[IDT_ITEMS].address = NULL;
}


/* Clean IOPL(12,13) and NT(14) flags in EFLAGS register */
static void clean_IOPL_NT_flags(void)
{
//	__asm__ volatile (
//		"pushfl\n"
//		"pop %%eax\n"
//		"and $0xffff8fff, %%eax\n"
//		"push %%eax\n"
//		"popfl\n"
//		: : : "eax"
//	);
}

/* Clean AM(18) flag in CR0 register */
static void clean_AM_flag(void)
{
//	__asm__ volatile (
//		"mov %%cr0, %%eax\n"
//		"and $0xfffbffff, %%eax\n"
//		"mov %%eax, %%cr0\n"
//		: : : "eax"
//	);
}

void pm_init(void)
{
	descriptor_t *gdt_p = (descriptor_t *) gdtr.base;

//	gdtr_load(&gdtr);
	
	if (config.cpu_active == 1) {
		traps_init();
		xen_set_trap_table(traps);
		/*
		 * NOTE: bootstrap CPU has statically allocated TSS, because
		 * the heap hasn't been initialized so far.
		 */
		tss_p = &tss;
	} else {
		tss_p = (tss_t *) malloc(sizeof(tss_t), FRAME_ATOMIC);
		if (!tss_p)
			panic("could not allocate TSS\n");
	}

//	tss_initialize(tss_p);
	
	gdt_p[TSS_DES].access = AR_PRESENT | AR_TSS | DPL_KERNEL;
	gdt_p[TSS_DES].special = 1;
	gdt_p[TSS_DES].granularity = 0;
	
	gdt_setbase(&gdt_p[TSS_DES], (uintptr_t) tss_p);
	gdt_setlimit(&gdt_p[TSS_DES], TSS_BASIC_SIZE - 1);

	/*
	 * As of this moment, the current CPU has its own GDT pointing
	 * to its own TSS. We just need to load the TR register.
	 */
//	tr_load(selector(TSS_DES));
	
	clean_IOPL_NT_flags();    /* Disable I/O on nonprivileged levels and clear NT flag. */
	clean_AM_flag();          /* Disable alignment check */
}

void set_tls_desc(uintptr_t tls)
{
	ptr_16_32_t cpugdtr;
	descriptor_t *gdt_p;

	gdtr_store(&cpugdtr);
	gdt_p = (descriptor_t *) cpugdtr.base;
	gdt_setbase(&gdt_p[TLS_DES], tls);
	/* Reload gdt register to update GS in CPU */
	gdtr_load(&cpugdtr);
}

/** @}
 */
