/**
 *
 * xia_md.h
 *
 * Created 02/19/02 -- PJF
 *
 * Copyright (c) 2002
 * X-ray Instrumentation Associates
 * All rights reserved
 *
 * Prototypes for MD layer
 *
 */

#include "xerxes_structures.h"

#include "xia_mddef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _DXP_PROTO_ /* ANSI C Prototypes go here */
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_util(Xia_Util_Functions *funcs, char *type);
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_io(Xia_Io_Functions *funcs, char *type);
XIA_MD_SHARED void XIA_MD_API dxp_md_error(char *,char *, int *, char *file, int line);

/* Enhanced logging routines added 8/21/01 -- PJF */
XIA_MD_SHARED void XIA_MD_API dxp_md_warning(char *routine, char *message, char *file, int line);
XIA_MD_SHARED void XIA_MD_API dxp_md_info(char *routine, char *message, char *file, int line);
XIA_MD_SHARED void XIA_MD_API dxp_md_debug(char *routine, char *message, char *file, int line);
XIA_MD_SHARED void XIA_MD_API dxp_md_output(char *filename);
XIA_MD_SHARED int XIA_MD_API dxp_md_enable_log(void);
XIA_MD_SHARED int XIA_MD_API dxp_md_suppress_log(void);
XIA_MD_SHARED int XIA_MD_API dxp_md_set_log_level(int level);
XIA_MD_SHARED void XIA_MD_API dxp_md_log(int level, char *routine, char *message, int error, char *file, int line);

XIA_MD_SHARED void XIA_MD_API *dxp_md_alloc(size_t);
XIA_MD_SHARED void XIA_MD_API dxp_md_free(void *);


#else /* _DXP_PROTO_ */

XIA_MD_EXPORT int XIA_MD_API dxp_md_init_util();
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_io();

XIA_MD_SHARED void XIA_MD_API dxp_md_error();
XIA_MD_SHARED void XIA_MD_API dxp_md_warning();
XIA_MD_SHARED void XIA_MD_API dxp_md_info();
XIA_MD_SHARED void XIA_MD_API dxp_md_debug();
XIA_MD_SHARED void XIA_MD_API dxp_md_output();
XIA_MD_SHARED int XIA_MD_API dxp_md_enable_log();
XIA_MD_SHARED int XIA_MD_API dxp_md_suppress_log();
XIA_MD_SHARED int XIA_MD_API dxp_md_set_log_level();
XIA_MD_SHARED void XIA_MD_API dxp_md_log();


#endif /* _DXP_PROTO_ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* Current output for the logging routines. By default, this is set to stdout */
extern FILE *out_stream;

/** Logging macros **/
#define dxp_md_log_error(x, y, z)	dxp_md_log(MD_ERROR,   (x), (y), (z), __FILE__, __LINE__)
#define dxp_md_log_warning(x, y)	dxp_md_log(MD_WARNING, (x), (y), 0,   __FILE__, __LINE__)
#define dxp_md_log_info(x, y)		dxp_md_log(MD_INFO,    (x), (y), 0,   __FILE__, __LINE__)
#define dxp_md_log_debug(x, y)		dxp_md_log(MD_DEBUG,   (x), (y), 0,   __FILE__, __LINE__)

/* Useful Macros */
#define STREQ(x, y)		(strcmp((x), (y)) == 0)

/* Boolean support */
typedef unsigned char boolean;
#define TRUE_	(1==1)
#define FALSE_	(1==0)

