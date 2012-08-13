/*
 * Copyright (c) 2012 Jiri Svoboda
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

/** @addtogroup libc
 * @{
 */
/** @file
 */

#ifndef LIBC_BD_SRV_H_
#define LIBC_BD_SRV_H_

#include <async.h>
#include <fibril_synch.h>
#include <bool.h>
#include <sys/types.h>

typedef struct bd_ops bd_ops_t;

typedef struct {
	fibril_mutex_t lock;
	bool connected;
	bd_ops_t *ops;
	void *arg;
	async_sess_t *client_sess;
} bd_srv_t;

typedef struct bd_ops {
	int (*open)(bd_srv_t *);
	int (*close)(bd_srv_t *);
	int (*read_blocks)(bd_srv_t *, aoff64_t, size_t, void *, size_t);
	int (*read_toc)(bd_srv_t *, uint8_t, void *, size_t);
	int (*write_blocks)(bd_srv_t *, aoff64_t, size_t, const void *, size_t);
	int (*get_block_size)(bd_srv_t *, size_t *);
	int (*get_num_blocks)(bd_srv_t *, aoff64_t *);
} bd_ops_t;

extern void bd_srv_init(bd_srv_t *);

extern int bd_conn(ipc_callid_t, ipc_call_t *, void *);

#endif

/** @}
 */