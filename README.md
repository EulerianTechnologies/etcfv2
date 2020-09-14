# Eulerian Technologies IAB TCFv2 high speed reader implementation (in C)

(gcc linux)


**API**

```

  etcfv2_t * etcfv2_parse( char *s );
  void etcfv2_free( etcfv2_t * );
  void etcfv2_ids( etcfv2_t * );

```

and direct access to public structure etcfv2_t (in include/etcfv2.h)



**High speed decode:**

Result on a simple i5-3570
+500K TCData per second

**without ids remap: bitfield access**
```
# time ./src/test1
Proc: 1000000

real	0m1.225s
user	0m1.225s
sys	0m0.000s
```

**with direct ids**
```
time ./src/test1
Proc: 1000000

real	0m1.661s
user	0m1.660s
sys	0m0.000s
```


**Extract format:**

```
# ./test1
CO4EneNO4EneNLaAAAENAwCEAA8AAAcAAAAAEvQA4DMAM1AZwBnQDPgAaACAAA.IGLtV_T9fb2vj-_Z99_tkeYwf95y3p-wzhheMs-8NyZeH_B4Wv2MyvBX4JiQKGRgksjLBAQdtHGlcTQgBwIlViTLMYk2MjzNKJrJEilsbO2dYGD9Pn8HT3ZCY70-vv__7v3ff_3g
 >Vers: 2
 >Created: 1597311988
 >Updated: 1597311988
 >CMPID: 730
 >CMPVERSION: 0
 >CONSENTSCREEN: 0
 >CONSENTLANG: EN
 >vendor_list_vers: 48
 >tcl_policy_vers: 2
 >is_serv_specific: 0
 >nonstand stacks: 0
 >purpose_one_treament: 0
 >publisher_cc: AA
 >vendor_consent_sz: 606
 >vendor_legitimate_interest_sz: 13

 Loop on bitfields
 Vendors: 408 409 410 412 413 415 
 Vendors LI: 11 
 Purposes: 5 6 7 8 
 Purposes LI Transparency: 6 7 8 
 SpecialFeatures: 2 

 Direct ids crunch
 Vendors: 415 413 412 410 409 408 
 Vendors LI: 11 
 Purposes: 8 7 6 5 
 Purposes LI Transparency: 8 7 6 
 SpecialFeatures: 2 
```
