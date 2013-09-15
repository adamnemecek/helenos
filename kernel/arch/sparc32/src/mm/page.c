/*
 * Copyright (c) 2010 Martin Decky
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

/** @addtogroup abs32lemm
 * @{
 */
/** @file
 */

#include <arch/mm/page.h>
#include <genarch/mm/page_pt.h>
#include <arch/mm/frame.h>
#include <mm/frame.h>
#include <mm/page.h>
#include <mm/as.h>
#include <typedefs.h>
#include <align.h>
#include <config.h>
#include <func.h>
#include <arch/interrupt.h>
#include <arch/asm.h>
#include <debug.h>
#include <memstr.h>
#include <print.h>
#include <interrupt.h>
#include <macros.h>

void page_arch_init(void)
{
	int flags = PAGE_CACHEABLE | PAGE_EXEC;
	page_mapping_operations = &pt_mapping_operations;

	page_table_lock(AS_KERNEL, true);
	
	/* Kernel identity mapping */
	//FIXME: We need to consider the possibility that
	//identity_base > identity_size and physmem_end.
	//This might lead to overflow if identity_size is too big.
	for (uintptr_t cur = PHYSMEM_START_ADDR;
	    cur < min(KA2PA(config.identity_base) +
	        config.identity_size, config.physmem_end);
	    cur += FRAME_SIZE)
		page_mapping_insert(AS_KERNEL, PA2KA(cur), cur, flags);
	

	page_table_unlock(AS_KERNEL, true);
	as_switch(NULL, AS_KERNEL);

	printf("as_context_table=0x%08x\n", as_context_table);

	/* Switch MMU to new context table */
	asi_u32_write(ASI_MMUREGS, 0x100, KA2PA(as_context_table) >> 4);

	//boot_page_table_free();
}

void page_fault(unsigned int n __attribute__((unused)), istate_t *istate)
{
}

/** @}
 */
