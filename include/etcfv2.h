/*
 * Copyright (c) 2020 by Eulerian Technologies SAS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 * * Neither the name of Eulerian Technologies nor the names of its
 *   contributors may be used to endorse or promote products
 *   derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * @file etcfv2.h
 * @brief ETCFV2 include
 * @author Guillaume Fougnies <guillaume@eulerian.com>
 * @version 0.1
 * @date 2020-08-01
 */

#ifndef ETCFV2_H
#define ETCFV2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct etcfv2_ids_s {
    uint16_t      *ids;
    uint16_t      nb;
  } etcfv2_ids_t;

  typedef struct etcfv2_s {
    uint8_t     version;
    uint64_t    epoch_created;
    uint64_t    epoch_updated;
    uint16_t    cmp_id;
    uint16_t    cmp_version;
    uint8_t     consent_screen;
    char        consent_lang[3];
    uint16_t    vendor_list_vers;
    uint8_t     tcl_policy_vers;
    uint8_t     is_service_specific:1;
    uint8_t     use_nonstandard_stacks:1;
    unsigned char special_features_optins[2];    /* bitfield */
    unsigned char purpose_consent[3];            /* bitfield */
    unsigned char purpose_li_transparency[3];    /* bitfield */
    uint8_t     purpose_one_treament:1;
    char        publisher_cc[3];

    unsigned char   *vendor_consent;              /* bitfield */
    uint16_t        vendor_consent_sz;
    unsigned char   *vendor_legitimate_interest;  /* bitfield */
    uint16_t        vendor_legitimate_interest_sz;

    etcfv2_ids_t  ids_special_features_optins;
    etcfv2_ids_t  ids_purpose_consent;
    etcfv2_ids_t  ids_purpose_li_transparency;
    etcfv2_ids_t  ids_vendor_consent;
    etcfv2_ids_t  ids_vendor_legitimate_interest;

  } etcfv2_t;

  etcfv2_t * etcfv2_parse( char *s );
  void etcfv2_free( etcfv2_t * );
  void etcfv2_ids( etcfv2_t * );

#define _ETCFV2_ELT_BF_SZ(n)          (sizeof(((etcfv2_t*)0)->n)*8)

#define ETCFV2_PURPOSES_BF_SZ         _ETCFV2_ELT_BF_SZ(purpose_consent)
#define ETCFV2_PURPOSES_LI_BF_SZ      _ETCFV2_ELT_BF_SZ(purpose_li_transparency)
#define ETCFV2_SPECIAL_FEATURES_BF_SZ _ETCFV2_ELT_BF_SZ(special_features_optins)

#define ETCFV2_BF_HAS( tg, p )  \
  ((((unsigned char*)(tg))[(p-1)/8] & (((char)0x1)<<(7 - (p-1)%8)))?1:0)

#define ETCFV2_BF_SET( tg, p )  \
  (((unsigned char*)(tg))[(p-1)/8]) |= ((char)0x1)<<(7 - (p-1)%8)

#ifdef __cplusplus
}
#endif

#endif
