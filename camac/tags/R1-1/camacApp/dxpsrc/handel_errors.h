/*
 *  handel_errors.h
 *
 *  Created 3-17-00:  JW: File to contain Error codes for XIA drivers.
 *  Copied 6-25-01 JW: copied xerxes_errors.h to handel_errors.h
 *
 *  Copyright 2000 X-ray Instrumentation Associates
 *  All rights reserved
 *
 */
#ifndef HANDEL_ERROR_H
#define HANDEL_ERROR_H

#ifndef HANDELDEF_H
#include <handeldef.h>
#endif

/*
 *  some error codes
 */
#define XIA_SUCCESS				0
/* I/O level error codes 1-100*/
#define XIA_OPEN_FILE			1
#define XIA_FILEERR				2
#define XIA_NOSECTION			3
#define XIA_FORMAT_ERROR		4
/*  primitive level error codes (due to mdio failures) 101-200*/
/*  DSP/FIPPI level error codes 201-300  */
/*  configuration errors  301-400  */
#define XIA_INITIALIZE			301
/*  host machine errors codes:  401-500 */
#define XIA_NOMEM				401
/* miscellaneous errors 501-600 */
#define XIA_UNKNOWN				501
/* Debug support */
#define XIA_DEBUG           1001

#endif						/* Endif for HANDEL_ERRORS_H */
