/*
 *  xia_handel.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *  Major Mods 3-17-00 JW: Complete revamping of libraries
 *  Copied 6-25-01 JW: copied xia_xerxes.h to xia_handel.h
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for HanDeL library routines
 */
#ifndef XIA_HANDEL_H
#define XIA_HANDEL_H

#ifndef HANDELDEF_H
#include <handeldef.h>
#endif

#define HANDEL_CODE_VERSION				0
#define HANDEL_CODE_REVISION			1

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _HANDEL_PROTO_
#include <stdio.h>

/*
 * following are internal prototypes for HANDEL.c routines
 */
HANDEL_IMPORT int HANDEL_API xiaInit(void);
HANDEL_IMPORT int HANDEL_API xiaInitHandel(void);

#else									/* Begin old style C prototypes */
/*
 * following are internal prototypes for handel layer subset of xerxes.c routines
 */
HANDEL_IMPORT int HANDEL_API xiaInit();
HANDEL_IMPORT int HANDEL_API xiaInitHandel();

#endif                                  /*   end if _HANDEL_PROTO_ */

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
}
#endif

#endif						/* Endif for XIA_HANDEL_H */
