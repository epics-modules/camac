/*<##Wed Apr  3 17:20:53 2002--COUGAR--Do not remove--XIA##>*/

/*
 *  xiadef.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *  Major Mods 3-17-00 JW: Complete revamping of libraries
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp driver routines
 */
#ifndef XIA_MDDEF_H
#define XIA_MDDEF_H

/* Have we allready defined EXPORTS and IMPORTS? */
#ifndef XIA_MD_PORTING_DEFINED
#define XIA_MD_PORTING_DEFINED

#define XIA_MD_STATIC static

#define XIA_MD_SHARED


#ifdef XIA_MD_USE_DLL		/* Linking to a DLL libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef XIA_MD_MAKE_DLL
#define XIA_MD_EXPORT __declspec(dllexport)
#define XIA_MD_IMPORT __declspec(dllimport)
#define XIA_MD_API
#else					/* Then we are making a static link library */
#define XIA_MD_EXPORT 	
#define XIA_MD_IMPORT __declspec(dllimport)
#define XIA_MD_API
#endif					/* Endif for XIA_MD_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef XIA_MD_MAKE_DLL
#define XIA_MD_EXPORT 
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#else					/* Then we are making a static link library */
#define XIA_MD_EXPORT 	
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#endif					/* Endif for XIA_MD_MAKE_DLL */

#endif					/* Endif for WIN32 */

#else					/* We are using static libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef XIA_MD_MAKE_DLL
#define XIA_MD_EXPORT __declspec(dllexport)
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#else					/* Then we are making a static link library */
#define XIA_MD_EXPORT 	
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#endif					/* Endif for XIA_MD_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef XIA_MD_MAKE_DLL
#define XIA_MD_EXPORT 
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#else					/* Then we are making a static link library */
#define XIA_MD_EXPORT 	
#define XIA_MD_IMPORT extern
#define XIA_MD_API
#endif					/* Endif for XIA_MD_MAKE_DLL */

#endif					/* Endif for WIN32 */

#endif					/* Endif for XIA_MD_USE_DLL */

#endif					/* Endif for XIA_MD_DXP_DLL_DEFINED */

#ifndef _DXP_SWITCH_
#define _DXP_SWITCH_ 1

#ifdef __STDC__
#define _DXP_PROTO_  1
#endif                /* end of __STDC__    */

#ifdef _MSC_VER
#ifndef _DXP_PROTO_
#define _DXP_PROTO_  1
#endif
#endif                /* end of _MSC_VER    */

#ifdef _DXP_PROTO_
#define VOID void
#else
#define VOID
#endif               /* end of _DXP_PROTO_ */

#endif               /* end of _DXP_SWITCH_*/

#endif				 /* Endif for XIA_MDDEF_H */
