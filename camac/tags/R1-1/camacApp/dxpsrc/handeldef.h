/*
 *  handeldef.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *  Major Mods 3-17-00 JW: Complete revamping of libraries
 *  Copied 6-25-01 JW: copied xerxesdef.h to handeldef.h
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp driver routines
 */
#ifndef HANDELDEF_H
#define HANDELDEF_H


/* Have we defined the EXPORT and IMPORT macros? */
#ifndef HANDEL_PORTING_DEFINED
#define HANDEL_PORTING_DEFINED



#define HANDEL_STATIC static


#define DXP_API
#define MD_API

#ifdef HANDEL_USE_DLL		/* Linking to a DLL libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef HANDEL_MAKE_DLL
#define HANDEL_EXPORT __declspec(dllexport)
#define HANDEL_IMPORT __declspec(dllimport)

#ifndef WIN32_VBA		/* Libraries for Visual Basic require STDCALL */
#define HANDEL_API
#else
#define HANDEL_API    _stdcall
#endif					/* Endif for WIN32_VBA */

#else					/* Then we are making a static link library */
#define HANDEL_EXPORT 	
#define HANDEL_IMPORT __declspec(dllimport)

#ifndef WIN32_VBA		/* Libraries for Visual Basic require STDCALL */
#define HANDEL_API
#else
#define HANDEL_API    _stdcall
#endif					/* Endif for WIN32_VBA */

#endif					/* Endif for HANDEL_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef HANDEL_MAKE_DLL
#define HANDEL_EXPORT 
#define HANDEL_IMPORT extern
#define HANDEL_API
#else					/* Then we are making a static link library */
#define HANDEL_EXPORT 	
#define HANDEL_IMPORT extern
#define HANDEL_API
#endif					/* Endif for HANDEL_MAKE_DLL */

#endif					/* Endif for WIN32 */

#else					/* We are using static libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef HANDEL_MAKE_DLL
#define HANDEL_EXPORT __declspec(dllexport)
#define HANDEL_IMPORT extern
#define HANDEL_API    
#else					/* Then we are making a static link library */
#define HANDEL_EXPORT 	
#define HANDEL_IMPORT extern
#define HANDEL_API    
#endif					/* Endif for HANDEL_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef HANDEL_MAKE_DLL
#define HANDEL_EXPORT 
#define HANDEL_IMPORT extern
#define HANDEL_API    
#else					/* Then we are making a static link library */
#define HANDEL_EXPORT 	
#define HANDEL_IMPORT extern
#define HANDEL_API
#endif					/* Endif for HANDEL_MAKE_DLL */

#endif					/* Endif for WIN32 */

#endif					/* Endif for HANDEL_USE_DLL */

#endif					/* Endif for HANDEL_DXP_DLL_DEFINED */

#ifndef _HANDEL_SWITCH_
#define _HANDEL_SWITCH_ 1

#ifdef __STDC__
#define _HANDEL_PROTO_  1
#endif                /* end of __STDC__    */

#ifdef _MSC_VER
#ifndef _HANDEL_PROTO_
#define _HANDEL_PROTO_  1
#endif
#endif                /* end of _MSC_VER    */

#ifdef _HANDEL_PROTO_
#define VOID void
#else
#define VOID
#endif               /* end of _HANDEL_PROTO_ */

#endif               /* end of _HANDEL_SWITCH_*/

#endif				 /* Endif for HANDELDEF_H */
