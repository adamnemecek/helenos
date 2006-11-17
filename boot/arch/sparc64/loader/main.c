/*
 * Copyright (C) 2005 Martin Decky
 * Copyright (C) 2006 Jakub Jermar 
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

#include "main.h" 
#include <printf.h>
#include "asm.h"
#include "_components.h"
#include <balloc.h>
#include <ofw.h>
#include <ofw_tree.h>
#include "ofwarch.h"
#include <align.h>

bootinfo_t bootinfo;
component_t components[COMPONENTS];

void bootstrap(void)
{
	printf("HelenOS SPARC64 Bootloader\n");
	
	init_components(components);

	if (!ofw_get_physmem_start(&bootinfo.physmem_start)) {
		printf("Error: unable to get start of physical memory.\n");
		halt();
	}

	if (!ofw_memmap(&bootinfo.memmap)) {
		printf("Error: unable to get memory map, halting.\n");
		halt();
	}
	
	if (bootinfo.memmap.total == 0) {
		printf("Error: no memory detected, halting.\n");
		halt();
	}
	
	printf("\nSystem info\n");
	printf(" memory: %dM starting at %P\n",
		bootinfo.memmap.total >> 20, bootinfo.physmem_start);

	printf("\nMemory statistics\n");
	printf(" kernel entry point at %P\n", KERNEL_VIRTUAL_ADDRESS);
	printf(" %P: boot info structure\n", &bootinfo);
	
	unsigned int i;
	for (i = 0; i < COMPONENTS; i++)
		printf(" %P: %s image (size %d bytes)\n", components[i].start,
			components[i].name, components[i].size);

	void * base = (void *) KERNEL_VIRTUAL_ADDRESS;
	unsigned int top = 0;

	printf("\nCopying components\n");
	bootinfo.taskmap.count = 0;
	for (i = 0; i < COMPONENTS; i++) {
		printf(" %s...", components[i].name);
		top = ALIGN_UP(top, PAGE_SIZE);
		memcpy(base + top, components[i].start, components[i].size);
		if (i > 0) {
			bootinfo.taskmap.tasks[bootinfo.taskmap.count].addr = base + top;
			bootinfo.taskmap.tasks[bootinfo.taskmap.count].size = components[i].size;
			bootinfo.taskmap.count++;
		}
		top += components[i].size;
		printf("done.\n");
	}

	balloc_init(&bootinfo.ballocs, ALIGN_UP(((uintptr_t) base) + top, PAGE_SIZE));

	printf("\nCanonizing OpenFirmware device tree...");
	bootinfo.ofw_root = ofw_tree_build();
	printf("done.\n");

	printf("\nChecking for secondary processors...");
	if (!ofw_cpu())
		printf("Error: unable to get CPU properties\n");
	printf("done.\n");

	printf("\nBooting the kernel...\n");
	jump_to_kernel((void *) KERNEL_VIRTUAL_ADDRESS,
		bootinfo.physmem_start | BSP_PROCESSOR,	&bootinfo, sizeof(bootinfo));
}
