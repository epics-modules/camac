/*
 *  md_epics.h
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *  If host computer is not big-endian (e.g. VAX) be sure to define
 *    LITTLE_ENDIAN
 *
 *
 *  Modified 10/4/97: Added compiler directives for compatibility with traditional
 *  C;  Remove LITTLE_ENDIAN: now determined dynamically. Remove MAXBLK, replace with
 *  dxp_md_get_maxblk and dxp_md_set_maxblk -- allows dynamically setting..
 *
 *  Major mods 11/30/99:  JEW:  documented, added a couple routines.
 *  2/19/2002             Mark Rivers.  Created from md_windows.h
 */
#ifndef DXP_MD_H
#define DXP_MD_H

#ifndef XIA_MDDEF_H
#include <xia_mddef.h>
#endif

/* Some constants */
#define MAXMOD 100

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DXP_PROTO_						/* ANSI C prototypes */
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_util(Xia_Util_Functions *funcs, char *type);
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_io(Xia_Io_Functions *funcs, char *type);
XIA_MD_STATIC int XIA_MD_API dxp_md_initialize(unsigned int *, char *);
XIA_MD_STATIC int XIA_MD_API dxp_md_open(char *, int *);
XIA_MD_STATIC int XIA_MD_API dxp_md_open_bcn(int *, int *, int *, int *);
XIA_MD_STATIC int XIA_MD_API dxp_md_io(int *,unsigned int *,unsigned int *,unsigned short *,unsigned int *);
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_initialize(unsigned int *, char *);
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_open(char *, int *);
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_io(int *,unsigned int *,unsigned int *,unsigned short *,unsigned int *);
XIA_MD_STATIC int XIA_MD_API dxp_md_lock_resource(int *ioChan, int *modChan, short *lock);
XIA_MD_STATIC int XIA_MD_API dxp_md_wait(float *);
XIA_MD_STATIC void XIA_MD_API dxp_md_error_control(char *,int *);

/* Enhanced logging routines added 8/21/01 -- PJF */
XIA_MD_STATIC void XIA_MD_API dxp_md_error(char *,char *, int *, char *file, int line);
XIA_MD_STATIC void XIA_MD_API dxp_md_warning(char *routine, char *message, char *file, int line);
XIA_MD_STATIC void XIA_MD_API dxp_md_info(char *routine, char *message, char *file, int line);
XIA_MD_STATIC void XIA_MD_API dxp_md_debug(char *routine, char *message, char *file, int line);
XIA_MD_STATIC void XIA_MD_API dxp_md_output(char *filename);
XIA_MD_STATIC int XIA_MD_API dxp_md_enable_log(void);
XIA_MD_STATIC int XIA_MD_API dxp_md_suppress_log(void);
XIA_MD_STATIC int XIA_MD_API dxp_md_set_log_level(int level);
XIA_MD_STATIC void XIA_MD_API dxp_md_log(int level, char *routine, char *message, int error, char *file, int line);

XIA_MD_STATIC int XIA_MD_API dxp_md_get_maxblk(void);
XIA_MD_STATIC int XIA_MD_API dxp_md_set_maxblk(unsigned int *);
XIA_MD_STATIC void XIA_MD_API *dxp_md_alloc(size_t);
XIA_MD_STATIC void XIA_MD_API dxp_md_free(void *);
XIA_MD_STATIC int XIA_MD_API dxp_md_puts(char *);

XIA_MD_IMPORT int XIA_MD_API xia_camxfr(short *camadr, short func, long nbytes, short mode, short *buf);
XIA_MD_IMPORT int XIA_MD_API xia_caminit(short *buf);

XIA_MD_IMPORT int XIA_MD_API DxpInitEPP(int );
XIA_MD_IMPORT int XIA_MD_API DxpWriteWord(unsigned short,unsigned short);
XIA_MD_IMPORT int XIA_MD_API DxpWriteBlock(unsigned short,unsigned short *,int);
XIA_MD_IMPORT int XIA_MD_API DxpWriteBlocklong(unsigned short,unsigned long *, int);
XIA_MD_IMPORT int XIA_MD_API DxpReadWord(unsigned short,unsigned short *);
XIA_MD_IMPORT int XIA_MD_API DxpReadBlock(unsigned short, unsigned short *,int);
XIA_MD_IMPORT int XIA_MD_API DxpReadBlockd(unsigned short, double *,int);
XIA_MD_IMPORT int XIA_MD_API DxpReadBlocklong(unsigned short,unsigned long *,int);
XIA_MD_IMPORT int XIA_MD_API DxpReadBlocklongd(unsigned short, double *,int);
XIA_MD_IMPORT void XIA_MD_API DxpSetID(unsigned short id);
XIA_MD_IMPORT int XIA_MD_API DxpWritePort(unsigned short port, unsigned short data);
XIA_MD_IMPORT int XIA_MD_API DxpReadPort(unsigned short port, unsigned short *data);


#else									/* old style prototypes */
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_util();
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_io();
XIA_MD_STATIC int XIA_MD_API dxp_md_initialize();
XIA_MD_STATIC int XIA_MD_API dxp_md_open();
XIA_MD_STATIC int XIA_MD_API dxp_md_open_bcn();
XIA_MD_STATIC int XIA_MD_API dxp_md_io();
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_initialize();
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_open();
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_io();
XIA_MD_STATIC int XIA_MD_API dxp_md_lock_resource();
XIA_MD_STATIC int XIA_MD_API dxp_md_wait();
XIA_MD_STATIC void XIA_MD_API dxp_md_error_control();

XIA_MD_STATIC void XIA_MD_API dxp_md_error();
XIA_MD_STATIC void XIA_MD_API dxp_md_warning();
XIA_MD_STATIC void XIA_MD_API dxp_md_info();
XIA_MD_STATIC void XIA_MD_API dxp_md_debug();
XIA_MD_STATIC void XIA_MD_API dxp_md_output();
XIA_MD_STATIC int XIA_MD_API dxp_md_enable_log();
XIA_MD_STATIC int XIA_MD_API dxp_md_suppress_log();
XIA_MD_STATIC int XIA_MD_API dxp_md_set_log_level();
XIA_MD_STATIC void XIA_MD_API dxp_md_log();
XIA_MD_STATIC int XIA_MD_API dxp_md_get_maxblk();
XIA_MD_STATIC int XIA_MD_API dxp_md_set_maxblk();
XIA_MD_STATIC void XIA_MD_API *dxp_md_alloc();
XIA_MD_STATIC void XIA_MD_API dxp_md_free();
XIA_MD_STATIC int XIA_MD_API dxp_md_puts();

XIA_MD_IMPORT int XIA_MD_API xia_camxfr();
XIA_MD_IMPORT int XIA_MD_API xia_caminit();

XIA_MD_IMPORT int XIA_MD_API DxpInitEPP();
XIA_MD_IMPORT int XIA_MD_API DxpWriteWord();
XIA_MD_IMPORT int XIA_MD_API DxpWriteBlock();
XIA_MD_IMPORT int XIA_MD_API DxpWriteBlocklong();
XIA_MD_IMPORT int XIA_MD_API DxpReadWord();
XIA_MD_IMPORT int XIA_MD_API DxpReadBlock();
XIA_MD_IMPORT int XIA_MD_API DxpReadBlockd();
XIA_MD_IMPORT int XIA_MD_API DxpReadBlocklong();
XIA_MD_IMPORT int XIA_MD_API DxpReadBlocklongd();
XIA_MD_IMPORT void XIA_MD_API DxpSetID();
XIA_MD_IMPORT int XIA_MD_API DxpWritePort();
XIA_MD_IMPORT int XIA_MD_API DxpReadPort();
#endif
#ifdef __cplusplus
}
#endif

/* Current output for the logging routines. By default, this is set to stdout */
extern FILE *out_stream;

/* Logging macro wrappers */
#define dxp_md_log_error(x, y, z)	dxp_md_log(MD_ERROR, (x), (y), (z), __FILE__, __LINE__)
#define dxp_md_log_warning(x, y)		dxp_md_log(MD_WARNING, (x), (y), 0, __FILE__, __LINE__)
#define dxp_md_log_info(x, y)			dxp_md_log(MD_INFO, (x), (y), 0, __FILE__, __LINE__)
#define dxp_md_log_debug(x, y)		dxp_md_log(MD_DEBUG, (x), (y), 0, __FILE__, __LINE__)
/* Useful Macros */
#define STREQ(x, y)		(strcmp((x), (y)) == 0)

/* Boolean support */
typedef unsigned char boolean;
#define TRUE_	(1==1)
#define FALSE_	(1==0)

#endif					/* End if for DXP_MD_H */

