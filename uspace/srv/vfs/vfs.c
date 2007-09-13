/*
 * Copyright (c) 2007 Jakub Jermar
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

/** @addtogroup fs
 * @{
 */ 

/**
 * @file	vfs.c
 * @brief	VFS multiplexer for HelenOS.
 */

#include <ipc/ipc.h>
#include <ipc/services.h>
#include <async.h>
#include <errno.h>
#include "vfs.h"

static void vfs_connection(ipc_callid_t iid, ipc_call_t *icall)
{
	ipcarg_t iarg1, iarg2;

	/*
	 * The connection was opened via the IPC_CONNECT_ME_TO call.
	 * This call needs to be answered.
	 *
	 * The protocol is that the requested action is specified in ARG1
	 * of the opening call. If the request has a single integer argument,
	 * it is passed in ARG2.
	 */
	iarg1 = IPC_GET_ARG1(*icall);
	iarg2 = IPC_GET_ARG2(*icall);

	/*
	 * Now, the connection can either be from an individual FS,
	 * which is trying to register itself and pass us its capabilities.
	 * Or, the connection is a regular connection from a client that wants
	 * us to do something for it (e.g. open a file, mount a fs etc.).
	 */
	switch (iarg1) {
	case VFS_REGISTER:
	case VFS_MOUNT:
	case VFS_UNMOUNT:
	case VFS_OPEN:
	default:
		ipc_answer_fast(iid, ENOTSUP, 0, 0);
		break;
	}
}

int main(int argc, char **argv)
{
	ipcarg_t phonead;

	async_set_client_connection(vfs_connection);
	ipc_connect_to_me(PHONE_NS, SERVICE_VFS, 0, &phonead);
	async_manager();
	return 0;
}

/**
 * @}
 */ 
