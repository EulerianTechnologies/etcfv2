
#include <stdio.h>
#include <etcfv2.h>

char *as[] = {
//  "COzmYxSOzmYxSAtABBENAmCsAP_AAH_AAAAAF8NX-T5eL2vju2Zdt7JEaYwfZxyjgOgThhSIsW8NwIeF7BoGP2MwHBW4JCQAGBAkkgKBAQMsHGBcCQABgIgRiTKMYE2MjzNKBJJAikkbc0FQCDVunsHS3ZCY70--u__bPAu8gkwVLwCRIUwgJJsQIhTABCOIAIBwCUEIQEClhoACAnYFAR6gAAAIDAACAAAAEEEBAIABAAAgIgAAAEBACACIBAACAEaAhAABIEAkAJEgCAAQA0JACCIIQBCDgwCjhAAA.YAAAAAAAAAAA",
//  "CLcVDxRMWfGmWAVAHCENAXCkAKDAADnAABRgA5mdfCKZuYJez-NQm0TBMYA4oCAAGQYIAAAAAAEAIAEgAA.argAC0gAAAAAAAAAAAA",
//  "COvFyGBOvFyGBAbAAAENAPCAAOAAAAAAAAAAAEEUACCKAAA"
//"CO4EneNO4EneNLaAAAENAwCAAAAAAAAAAAAAEvQAgDOgGfAAAAAA.IGLtV_T9fb2vj-_Z99_tkeYwf95y3p-wzhheMs-8NyZeH_B4Wv2MyvBX4JiQKGRgksjLBAQdtHGlcTQgBwIlViTLMYk2MjzNKJrJEilsbO2dYGD9Pn8HT3ZCY70-vv__7v3ff_3g"
//"CO4EneNO4EneNLaAAAENAwCAAA8AAAAAAAAAEvQA4DMAM1AZwBnQDPgAaACAAA.IGLtV_T9fb2vj-_Z99_tkeYwf95y3p-wzhheMs-8NyZeH_B4Wv2MyvBX4JiQKGRgksjLBAQdtHGlcTQgBwIlViTLMYk2MjzNKJrJEilsbO2dYGD9Pn8HT3ZCY70-vv__7v3ff_3g"
//"CO4EneNO4EneNLaAAAENAwCEAA8AAAcAAAAAEvQA4DMAM1AZwBnQDPgAaACAAA.IGLtV_T9fb2vj-_Z99_tkeYwf95y3p-wzhheMs-8NyZeH_B4Wv2MyvBX4JiQKGRgksjLBAQdtHGlcTQgBwIlViTLMYk2MjzNKJrJEilsbO2dYGD9Pn8HT3ZCY70-vv__7v3ff_3g"
//"COvFyGBOvFyGBAbAAAENAPCAAOAAAAAAAAAAAEEUACCKAAA.IFoEUQQgAIQwgIwQABAEAAAAOIAACAIAAAAQAIAgEAACEAAAAAgAQBAAAAAAAGBAAgAAAAAAAFAAECAAAgAAQARAEQAAAAAJAAIAAgAAAYQEAAAQmAgBC3ZAYzUw"
//"CO5owUhO5owUhAHABBENA2CsAP_AAH_AAAAAGetf_X_fb2vj-_5999t0eY1f9_63v-wzjgeNs-8NyZ_X_L4Xr2MyvB36pq4KuR4Eu3LBAQdlHOHcTQmQwIkVqTLsbk2Mq7NKJ7LEilMbM2dYGH9vn8XTuZCY70_sf__z_3-_-___67b-IGeEEmGpfAQJCWMBJNmlUKIEIVxIVAOACihGFo0sNCRwU7K4CPUECABAagIwIgQYgoxZBAAAAAElEQAkBwIBEARAIAAQArQEIACJAEFgBIGAQACgGhYARRBKBIQZHBUcogQFSLRQTzRgSQAA.f_gAD_gAAAAA" ,
"CO5qoVvO5qoVvAHABBENA2CgAAAAAH_AAAAAAAAM8IJMNS-AgSEsYCSbNKoUQIQriQqAcAFFCMLRpYaEjgp2VwEeoIEACA1ARgRAgxBRiyCAAAAAJKIgBIDgQCIAiAQAAgBWgIQAESAILACQMAgAFANCwAiiCUCQgyOCo5RAgKkWignmjAkgAAAA.YAAAD_gAAAAA"

};

//#define SPEEDTEST   1000000
//#define SPEEDTEST_WITH_IDS


int main( void )
{
  int i;
  etcfv2_t *etcfv2;

#ifdef SPEEDTEST
  int proc = 0;
  for (i=0; i<(sizeof(as) / sizeof(char*)); i++) {
    int j;
    for (j=0; j<SPEEDTEST; j++) {
      etcfv2 = etcfv2_parse( as[i] );
#ifdef SPEEDTEST_WITH_IDS
      etcfv2_ids( etcfv2 );
#endif
      if (etcfv2->version == 2)
        proc ++;
      etcfv2_free( etcfv2 );
    }
  }
  printf("Proc: %d\n", proc);

#else

  for (i=0; i<(sizeof(as) / sizeof(char*)); i++) {

    printf("\n");


    etcfv2 = etcfv2_parse( as[i] );
    if (! etcfv2) {
      fprintf(stderr, "Failed to parse: %s\n", as[i]);
      break;
    }

    printf("%s"
           "\n >Vers: %d"
           "\n >Created: %"PRIu64
           "\n >Updated: %"PRIu64
           "\n >CMPID: %"PRIu16
           "\n >CMPVERSION: %"PRIu16
           "\n >CONSENTSCREEN: %"PRIu8
           "\n >CONSENTLANG: %s"
           "\n >vendor_list_vers: %d"
           "\n >tcl_policy_vers: %d"
           "\n >is_serv_specific: %d"
           "\n >nonstand stacks: %d"
           "\n >purpose_one_treament: %d"
           "\n >publisher_cc: %s"
           "\n >vendor_consent_sz: %d"
           "\n >vendor_legitimate_interest_sz: %d"
           "\n"
           ,

           as[i],
           etcfv2->version,
           etcfv2->epoch_created,
           etcfv2->epoch_updated,
           etcfv2->cmp_id,
           etcfv2->cmp_version,
           etcfv2->consent_screen,
           etcfv2->consent_lang,

           etcfv2->vendor_list_vers,
           etcfv2->tcl_policy_vers,
           etcfv2->is_service_specific,
           etcfv2->use_nonstandard_stacks,
           etcfv2->purpose_one_treament,
           etcfv2->publisher_cc,
           etcfv2->vendor_consent_sz,
           etcfv2->vendor_legitimate_interest_sz
           );

    printf("\n Loop on bitfields\n");
    int vendorid;
    printf(" Vendors: ");
    for (vendorid=0; vendorid<=etcfv2->vendor_consent_sz; vendorid++) {
      if (ETCFV2_BF_HAS( etcfv2->vendor_consent, vendorid )) {
        printf("%d ", vendorid);
      }
    }
    printf("\n");

    printf(" Vendors LI: ");
    for (vendorid=1; vendorid<=etcfv2->vendor_legitimate_interest_sz; vendorid++)
    {
      if (ETCFV2_BF_HAS( etcfv2->vendor_legitimate_interest, vendorid )) {
        printf("%d ", vendorid);
      }
    }
    printf("\n");

    int purposeid;
    printf(" Purposes: ");
    for (purposeid=1; purposeid<=ETCFV2_PURPOSES_BF_SZ; purposeid++)
    {
      if (ETCFV2_BF_HAS( etcfv2->purpose_consent, purposeid )) {
        printf("%d ", purposeid);
      }
    }
    printf("\n");

    int purposeliid;
    printf(" Purposes LI Transparency: ");
    for (purposeliid=1; purposeliid<=ETCFV2_PURPOSES_LI_BF_SZ; purposeliid++)
    {
      if (ETCFV2_BF_HAS( etcfv2->purpose_li_transparency, purposeliid )) {
        printf("%d ", purposeliid);
      }
    }
    printf("\n");

    int specialfeaturesid;
    printf(" SpecialFeatures: ");
    for (specialfeaturesid=1; specialfeaturesid<=ETCFV2_SPECIAL_FEATURES_BF_SZ;
         specialfeaturesid++) {
      if (ETCFV2_BF_HAS( etcfv2->special_features_optins, specialfeaturesid )) {
        printf("%d ", specialfeaturesid);
      }
    }
    printf("\n");

    printf("\n Direct ids crunch\n");
    /* Crunch direct ids */

    etcfv2_ids( etcfv2 );

    printf(" Vendors: ");
    for (i=0; i<etcfv2->ids_vendor_consent.nb; i++) {
      printf("%d ", etcfv2->ids_vendor_consent.ids[ i ]);
    }
    printf("\n");

    printf(" Vendors LI: ");
    for (i=0; i<etcfv2->ids_vendor_legitimate_interest.nb; i++) {
      printf("%d ", etcfv2->ids_vendor_legitimate_interest.ids[ i ]);
    }
    printf("\n");

    printf(" Purposes: ");
    for (i=0; i<etcfv2->ids_purpose_consent.nb; i++) {
      printf("%d ", etcfv2->ids_purpose_consent.ids[ i ]);
    }
    printf("\n");

    printf(" Purposes LI Transparency: ");
    for (i=0; i<etcfv2->ids_purpose_li_transparency.nb; i++) {
      printf("%d ", etcfv2->ids_purpose_li_transparency.ids[ i ]);
    }
    printf("\n");

    printf(" SpecialFeatures: ");
    for (i=0; i<etcfv2->ids_special_features_optins.nb; i++) {
      printf("%d ", etcfv2->ids_special_features_optins.ids[ i ]);
    }
    printf("\n");


    etcfv2_free( etcfv2 );
    printf("\n");
  }

#endif

  exit(0);
}

