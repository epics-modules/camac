/*
 *  xerxes_structures.h
 *
 *  Created 6-22-00:  JW: Header for user use, it has better name space protection
 *							This header is needed for the Machine Dependent code.
 *
 *  Copyright 2000 X-ray Instrumentation Associates
 *  All rights reserved
 *
 */
#ifndef XERXES_STRUCTURES_H
#define XERXES_STRUCTURES_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif


/*
 *  Structure that points at the functions within the MD Interface
 */
typedef int (*DXP_MD_IO)(int *, unsigned int *, unsigned int *, unsigned short *, 
						 unsigned int *);
typedef int (*DXP_MD_INITIALIZE)(unsigned int *, char *);
typedef int (*DXP_MD_OPEN)(char *, int *);
typedef int (*DXP_MD_GET_MAXBLK)(void);
typedef int (*DXP_MD_SET_MAXBLK)(unsigned int *);
typedef int (*DXP_MD_LOCK_RESOURCE)(int *, int*, short *);
struct Xia_Io_Functions {
	DXP_MD_IO dxp_md_io;
	DXP_MD_INITIALIZE dxp_md_initialize;
	DXP_MD_OPEN dxp_md_open;
	DXP_MD_GET_MAXBLK dxp_md_get_maxblk;
	DXP_MD_SET_MAXBLK dxp_md_set_maxblk;
	DXP_MD_LOCK_RESOURCE dxp_md_lock_resource;
};
typedef struct Xia_Io_Functions Xia_Io_Functions;
/*
 *  Structure that points at the utility functions that are machine dependent
 */
typedef void (*DXP_MD_ERROR_CONTROL)(char *, int *);
typedef void (*DXP_MD_ERROR)(char *, char *, int *);
typedef void* (*DXP_MD_ALLOC)(size_t);
typedef void (*DXP_MD_FREE)(void *);
typedef int (*DXP_MD_PUTS)(char *);
typedef int (*DXP_MD_WAIT)(float *);
struct Xia_Util_Functions {
	DXP_MD_ERROR_CONTROL dxp_md_error_control;
	DXP_MD_ERROR dxp_md_error;
	DXP_MD_ALLOC dxp_md_alloc;
	DXP_MD_FREE dxp_md_free;
	DXP_MD_PUTS dxp_md_puts;
	DXP_MD_WAIT dxp_md_wait;
};
typedef struct Xia_Util_Functions Xia_Util_Functions;

#endif						/* Endif for XERXES_STRUCTURES_H */
