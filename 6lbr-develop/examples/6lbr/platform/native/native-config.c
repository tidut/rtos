/*
 * Copyright (c) 2016, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         6LBR configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CONFIG"

#include "node-config.h"
#include "slip-config.h"
#include "log-6lbr.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "er-coap.h"
#include "cetic-6lbr.h"

#include "native-config.h"
#include "ini.h"

LIST(callbacks);

static native_config_callback_t global_config_cb;

/*---------------------------------------------------------------------------*/
static int native_config_global_handler(void* user, const char* section, const char* name,
    const char* value) {
  return 0;
}
/*---------------------------------------------------------------------------*/
static int native_config_handler(void* user, const char* section, const char* name,
    const char* value) {
  native_config_callback_t *cb;
  for(cb = list_head(callbacks);
      cb != NULL;
      cb = list_item_next(cb)) {
    if(strcmp(section, cb->section) == 0) {
      break;
    }
  }
  if(cb) {
    return cb->callback(cb->user, cb->section, name, value);
  } else {
    LOG6LBR_WARN("Invalid section : %s\n", name);
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
void native_config_load(void)
{
  int result;

  if (config_file_name) {
    LOG6LBR_INFO("Loading configuration : %s\n",config_file_name);
    result = ini_parse(config_file_name, native_config_handler, NULL);
    if (result < 0) {
      LOG6LBR_WARN("Can not open %s : %s\n", config_file_name, strerror(errno));
    }
    else if (result) {
      LOG6LBR_FATAL("Syntax error in %s at line %d\n", config_file_name, result);
      exit(1);
    }
  } else {
    LOG6LBR_WARN("No configuration file specified\n");
  }
}
/*---------------------------------------------------------------------------*/
void native_config_add_callback(native_config_callback_t *cb_info,
    char const * section, config_callback c, void *user)
{
  if(cb_info != NULL && c != NULL) {
    cb_info->callback = c;
    cb_info->section = section;
    cb_info->user = user;
    list_add(callbacks, cb_info);
  }
}
/*---------------------------------------------------------------------------*/
void native_config_init(void)
{
  list_init(callbacks);
  native_config_add_callback(&global_config_cb, "global", native_config_global_handler, NULL);
}
