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



#define ETCFV2_C

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "etcfv2.h"

  static void
_extract_xbits( unsigned char *s, uint64_t width, uint64_t offset,
                unsigned char *sout, int tgsz )
{
  int pos, pos_out;
  int byte, bit;

  pos     = 0;
  pos_out = tgsz - width;

  byte    = offset / 8;
  pos     = byte * 8;

  for (;;) {
    unsigned char c = s[byte];

    for (bit=0;bit<8;bit++) {
      /* End */
      if (pos>=(offset + width))
        return;

      /* InRange (post-offset) */
      if (pos>=offset) {
        if (c&0x80) {
          sout[ pos_out/8 ] |= (0x80) >> ((pos_out)%8);
        }
        pos_out ++;
      }

      c = c << 1;
      pos ++;
    }

    byte ++;
  }
}

  static uint64_t
_extract_intbits( void *b, int width, int pos )
{
  uint64_t res = 0;
  _extract_xbits( b, width, pos, (unsigned char*) &res, 64 );
  return __builtin_bswap64( res );
}

  static int
_etcfv2_bf_set_nb( void *b, int sz )
{
  int nb = 0;
  while (sz>=64) {
    nb += __builtin_popcountll( *((uint64_t*)b) );
    sz -= 64;
    b = (((uint64_t*)b)+1);
  }
  if (sz>=32) {
    nb += __builtin_popcount( *((uint32_t*)b) );
    sz -= 32;
    b = (((uint32_t*)b)+1);
  }

  if (sz>0) {
    int byte = 0, bit;
    unsigned char *s = b;
    for (;;) {
      unsigned char c = s[byte];
      for (bit=0;bit<8;bit++) {
        if (c&0x80) nb ++;
        c = c << 1;
        if (! (-- sz))
          return nb;
      }
      byte ++;
    }
  }
  return nb;
}

  static int
_etcfv2_bfrg_parser(  void  *b,  int  start,  int  bits,
                      unsigned  char  **ptg,  uint16_t  *ptgnb  )
{
  uint16_t  i, bf_maxvendorid = 0;
  uint8_t   is_range = 0;

#define __EXTRACT_XBITS( tg, len )     \
  if  (start  +  len  >  bits)  goto  err;                                              \
  _extract_xbits( b, len, start, tg, len ); start += len;

#define __EXTRACT_IBITS( tg, len )     \
  if  (start  +  len  >  bits)  goto  err;                                              \
  tg = _extract_intbits( b, len, start ); start += len;

  __EXTRACT_IBITS( bf_maxvendorid, 16 );
  __EXTRACT_IBITS( is_range, 1 );

  *ptg = calloc(1, sizeof(char) * (1 + (bf_maxvendorid/8)));

  if (is_range) {
    /* Range shit ass pet fuckers format */
    uint16_t nb_entries = 0;

    __EXTRACT_IBITS( nb_entries, 12 );

    for (i = 0; i < nb_entries; i++) {
      uint16_t is_arange = 0;
      uint16_t vendorid = 0;
      uint16_t lvendorid = 0;

      __EXTRACT_IBITS( is_arange, 1 );
      __EXTRACT_IBITS( vendorid, 16 );
      lvendorid = vendorid;
      if (is_arange) {
        __EXTRACT_IBITS( lvendorid, 16 );
      }

      while (vendorid <= lvendorid) {
        if (vendorid>bf_maxvendorid)
          break;

        ETCFV2_BF_SET( *ptg, vendorid );

        vendorid ++;
      }
    }
  }

  else {
    __EXTRACT_XBITS( *ptg, bf_maxvendorid );
  }

  *ptgnb = bf_maxvendorid;

  return start;
err:
  return  -1;
}

/*
 * B64
 *
 */
static const char Base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/-_";
  static int
_etcfv2_b64_dec( char *src, int srcsize, unsigned char *target, int targsize )
{
  int tarindex, state, ch;
  char *pos;
  char *base64 = (char*)Base64;

	if (src == NULL || target == NULL)
    return 0;

	state = tarindex = 0;

	while ((ch = *src++) != '\0' && srcsize--) {
		pos = strchr(base64, ch);
		if (pos == 0)     /* A non-base64 character. */
			return (-1);

    if (pos-base64 > 63) /* Regulate second extended block -_ */
      pos -= 2;

		switch (state) {
			case 0:
				if (target) {
					if ((size_t)tarindex >= targsize)
						return (-1);
					target[tarindex] = (pos - base64) << 2;
				}
				state = 1;
				break;
			case 1:
				if (target) {
					if ((size_t)tarindex + 1 >= targsize)
						return (-1);
					target[tarindex]   |=  (pos - base64) >> 4;
					target[tarindex+1]  = ((pos - base64) & 0x0f)
						<< 4 ;
				}
				tarindex++;
				state = 2;
				break;
			case 2:
				if (target) {
					if ((size_t)tarindex + 1 >= targsize)
						return (-1);
					target[tarindex]   |=  (pos - base64) >> 2;
					target[tarindex+1]  = ((pos - base64) & 0x03)
						<< 6;
				}
				tarindex++;
				state = 3;
				break;
			case 3:
				if (target) {
					if ((size_t)tarindex >= targsize)
						return (-1);
					target[tarindex] |= (pos - base64);
				}
				tarindex++;
				state = 0;
				break;
			default:
				abort();
		}
	}

  return (tarindex);
}


/**
 * @brief The TCdata core block parser
 *
 * @param etcfv2
 * @param s
 * @param l
 *
 * @return  0 on success, -1 on error
 */
  static int
_etcfv2_core_parser( etcfv2_t *etcfv2, char *s, int l )
{
  void *b;
  int b_sz, err = -1, bits = 0;
  int start = 0;

  b     = calloc( 1, sizeof(char) * (l+1) );
  b_sz  = _etcfv2_b64_dec( s, l, b, l );

  if (b_sz <= 0)
    goto err;

  bits = b_sz * 8;

#define _EXTRACT_XBITS( tg, len )                                   \
  if (start + len > bits) goto err;                                 \
  _extract_xbits( b, len, start, etcfv2->tg, len ); start += len;

#define _EXTRACT_IBITS( tg, len )                                   \
  if (start + len > bits) goto err;                                 \
  etcfv2->tg = _extract_intbits( b, len, start ); start += len;

  _EXTRACT_IBITS( version,                 6 );
  _EXTRACT_IBITS( epoch_created,          36 );
  _EXTRACT_IBITS( epoch_updated,          36 );
  _EXTRACT_IBITS( cmp_id,                 12 );
  _EXTRACT_IBITS( cmp_version,            12 );
  _EXTRACT_IBITS( consent_screen,          6 );
  _EXTRACT_IBITS( consent_lang[0],         6 );
  _EXTRACT_IBITS( consent_lang[1],         6 );
  _EXTRACT_IBITS( vendor_list_vers,             12 );
  _EXTRACT_IBITS( tcl_policy_vers,               6 );
  _EXTRACT_IBITS( is_service_specific,           1 );
  _EXTRACT_IBITS( use_nonstandard_stacks,        1 );

  _EXTRACT_XBITS( special_features_optins,      12 );
  _EXTRACT_XBITS( purpose_consent,              24 );
  _EXTRACT_XBITS( purpose_li_transparency,      24 );

  /* Specific Jurisdiction Disclosures */
  _EXTRACT_IBITS( purpose_one_treament,          1 );
  _EXTRACT_IBITS( publisher_cc[0],               6 );
  _EXTRACT_IBITS( publisher_cc[1],               6 );

  /* Vendor Consent Section */
  start = _etcfv2_bfrg_parser( b, start, bits,
                               &(etcfv2->vendor_consent),
                               &(etcfv2->vendor_consent_sz) );
  if  (start  <  0)
    goto  err;

  start = _etcfv2_bfrg_parser( b, start, bits,
                               &(etcfv2->vendor_legitimate_interest),
                               &(etcfv2->vendor_legitimate_interest_sz) );
  if  (start  <  0)
    goto  err;

  /* readable format */
  etcfv2->consent_lang[0] += 'A';
  etcfv2->consent_lang[1] += 'A';
  etcfv2->publisher_cc[0] += 'A';
  etcfv2->publisher_cc[1] += 'A';

  etcfv2->epoch_created /= 10;
  etcfv2->epoch_updated /= 10;

  err = 0;

err:
  if (b)
    free( b );

  return err;
}

/**
 * @brief Parse
 *
 * @param s
 *
 * @return etcfv2_t structure allocated
 */
  etcfv2_t *
etcfv2_parse( char *s )
{
  etcfv2_t *etcfv2 = NULL;
  char *tk;
  int l;

  if (! s)
    return NULL;

  /* New tcfv2 struct */
  etcfv2 = calloc( 1, sizeof(etcfv2_t) );

  /* Core String */
  tk = strchr( s, '.' );
  if (tk)
    l = tk - s;
  else
    l = strlen( s );

  if (_etcfv2_core_parser( etcfv2, s, l ))
    goto err;

  return etcfv2;

err:
  etcfv2_free(  etcfv2  );
  return NULL;
}


  static void
_etcfv2_bf2ids( unsigned char *in, int in_sz, etcfv2_ids_t *ids )
{
  int i = 1; /* start @1 */
  int x = 0, nb_left = 0;

  nb_left = _etcfv2_bf_set_nb( in, in_sz );
  if (! nb_left)
    return;

  ids->nb   = nb_left;
  ids->ids  = calloc( ids->nb, sizeof( *(ids->ids) ) );

  while (in_sz && x<nb_left) {
    int byte = 0, bit;
    unsigned char c;

    if (in_sz >= 32) {
      int pos;
      if ((pos = __builtin_ffs( *((uint32_t*)in) ))) {
        //pos = 32 - pos; /* msb */
        byte=pos/8;
        i += byte*8;
        for (; byte<4 && x<nb_left; byte++) {
          c = in[byte];
          for (bit=0;bit<8;bit++) {
            if (c & 0x80) {
              ids->ids[ x ++ ] = i;
            }
            c = c << 1;
            i ++;
          }
        }
      }
      else
        i += 32;
      in_sz -= 32;
      in = (unsigned char*)(((uint32_t*)in)+1);
    }

    if (in_sz) {
      c = in[0];
      for (bit=0;bit<8 && in_sz && x<nb_left;bit++) {
        if (c & 0x80) {
          ids->ids[ x ++ ] = i;
        }
        c = c << 1;
        in_sz --;
        i ++;
      }
      in ++;
    }
  }
}

/**
 * @brief build ids version of bitfields
 *
 * @param etcfv2
 */
  void
etcfv2_ids( etcfv2_t *etcfv2 )
{
  _etcfv2_bf2ids( etcfv2->vendor_consent,
                  etcfv2->vendor_consent_sz,
                  &etcfv2->ids_vendor_consent );

  _etcfv2_bf2ids( etcfv2->vendor_legitimate_interest,
                  etcfv2->vendor_legitimate_interest_sz,
                  &etcfv2->ids_vendor_legitimate_interest );

  _etcfv2_bf2ids( etcfv2->special_features_optins,
                  ETCFV2_SPECIAL_FEATURES_BF_SZ,
                  &etcfv2->ids_special_features_optins );
  _etcfv2_bf2ids( etcfv2->purpose_consent,
                  ETCFV2_PURPOSES_BF_SZ,
                  &etcfv2->ids_purpose_consent );
  _etcfv2_bf2ids( etcfv2->purpose_li_transparency,
                  ETCFV2_PURPOSES_LI_BF_SZ,
                  &etcfv2->ids_purpose_li_transparency );
}

/**
 * @brief free etcfv2 allocated structure from 'etcfv2_parse'
 *
 * @param etcfv2
 */
  void
etcfv2_free( etcfv2_t *etcfv2 )
{
  if (etcfv2) {
    int i;
    void *v[] = {
      etcfv2->vendor_consent,
      etcfv2->vendor_legitimate_interest,
      etcfv2->ids_vendor_consent.ids,
      etcfv2->ids_vendor_legitimate_interest.ids,
      etcfv2->ids_special_features_optins.ids,
      etcfv2->ids_purpose_consent.ids,
      etcfv2->ids_purpose_li_transparency.ids
    };
    for (i=0; i<sizeof(v)/sizeof(void*); i++) {
      if (v[i])
        free(v[i]);
    }
    free(etcfv2);
  }
}

