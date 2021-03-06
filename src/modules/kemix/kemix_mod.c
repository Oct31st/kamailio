/**
 * Copyright (C) 2019 Daniel-Constantin Mierla (asipto.com)
 *
 * This file is part of Kamailio, a free SIP server.
 *
 * Kamailio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * Kamailio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/sr_module.h"
#include "../../core/dprint.h"
#include "../../core/kemi.h"
#include "../../core/parser/parse_uri.h"

MODULE_VERSION

struct module_exports exports = {
	"kemix",         /* module name */
	DEFAULT_DLFLAGS, /* dlopen flags */
	0,               /* cmd (cfg function) exports */
	0,               /* param exports */
	0,               /* RPC method exports */
	0,               /* pseudo-variables exports */
	0,               /* response handling function */
	0,               /* module init function */
	0,               /* per-child init function */
	0                /* module destroy function */
};


/**
 *
 */
static sr_kemi_xval_t _sr_kemi_kx_xval = {0};

/**
 *
 */
static sr_kemi_xval_t* ki_kx_get_ruri(sip_msg_t *msg)
{
	memset(&_sr_kemi_kx_xval, 0, sizeof(sr_kemi_xval_t));

	if(msg==NULL) {
		sr_kemi_xval_null(&_sr_kemi_kx_xval, 0);
		return &_sr_kemi_kx_xval;
	}

	if(msg->first_line.type == SIP_REPLY) {
		/* REPLY doesnt have a ruri */
		sr_kemi_xval_null(&_sr_kemi_kx_xval, 0);
		return &_sr_kemi_kx_xval;
	}

	if(msg->parsed_uri_ok==0 /* R-URI not parsed*/ && parse_sip_msg_uri(msg)<0) {
		LM_ERR("failed to parse the R-URI\n");
		sr_kemi_xval_null(&_sr_kemi_kx_xval, 0);
		return &_sr_kemi_kx_xval;
	}

	_sr_kemi_kx_xval.vtype = SR_KEMIP_STR;
	if (msg->new_uri.s!=NULL) {
		_sr_kemi_kx_xval.v.s = msg->new_uri;
	} else {
		_sr_kemi_kx_xval.v.s = msg->first_line.u.request.uri;
	}
	return &_sr_kemi_kx_xval;
}

/**
 *
 */
/* clang-format off */
static sr_kemi_t sr_kemi_kx_exports[] = {
	{ str_init("kx"), str_init("get_ruri"),
		SR_KEMIP_XVAL, ki_kx_get_ruri,
		{ SR_KEMIP_NONE, SR_KEMIP_NONE, SR_KEMIP_NONE,
			SR_KEMIP_NONE, SR_KEMIP_NONE, SR_KEMIP_NONE }
	},

	{ {0, 0}, {0, 0}, 0, NULL, { 0, 0, 0, 0, 0, 0 } }
};

/**
 *
 */
int mod_register(char *path, int *dlflags, void *p1, void *p2)
{
	sr_kemi_modules_add(sr_kemi_kx_exports);
	return 0;
}
