/*
 *  xerxesdef.h
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
#ifndef XERXESDEF_H
#define XERXESDEF_H


/* Have we defined the EXPORT and IMPORT macros? */
#ifndef XERXES_PORTING_DEFINED
#define XERXES_PORTING_DEFINED



#define XERXES_STATIC static
#define XERXES_SHARED


#define DXP_API
#define MD_API

#ifdef XERXES_USE_DLL		/* Linking to a DLL libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef XERXES_MAKE_DLL
#define XERXES_EXPORT __declspec(dllexport)
#define XERXES_IMPORT __declspec(dllimport)

#ifndef WIN32_VBA		/* Libraries for Visual Basic require STDCALL */
#define XERXES_API
#else
#define XERXES_API    _stdcall
#endif					/* Endif for WIN32_VBA */

#else					/* Then we are making a static link library */
#define XERXES_EXPORT 	
#define XERXES_IMPORT __declspec(dllimport)

#ifndef WIN32_VBA		/* Libraries for Visual Basic require STDCALL */
#define XERXES_API
#else
#define XERXES_API    _stdcall
#endif					/* Endif for WIN32_VBA */

#endif					/* Endif for XERXES_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef XERXES_MAKE_DLL
#define XERXES_EXPORT 
#define XERXES_IMPORT extern
#define XERXES_API
#else					/* Then we are making a static link library */
#define XERXES_EXPORT 	
#define XERXES_IMPORT extern
#define XERXES_API
#endif					/* Endif for XERXES_MAKE_DLL */

#endif					/* Endif for WIN32 */

#else					/* We are using static libraries */

#ifdef WIN32			/* If we are on a Windoze platform */

#ifdef XERXES_MAKE_DLL
#define XERXES_EXPORT __declspec(dllexport)
#define XERXES_IMPORT extern
#define XERXES_API    
#else					/* Then we are making a static link library */
#define XERXES_EXPORT 	
#define XERXES_IMPORT extern
#define XERXES_API    
#endif					/* Endif for XERXES_MAKE_DLL */

#else					/* Not on a Windoze platform */

#ifdef XERXES_MAKE_DLL
#define XERXES_EXPORT 
#define XERXES_IMPORT extern
#define XERXES_API    
#else					/* Then we are making a static link library */
#define XERXES_EXPORT 	
#define XERXES_IMPORT extern
#define XERXES_API
#endif					/* Endif for XERXES_MAKE_DLL */

#endif					/* Endif for WIN32 */

#endif					/* Endif for XERXES_USE_DLL */

#endif					/* Endif for XERXES_DXP_DLL_DEFINED */

#ifndef _XERXES_SWITCH_
#define _XERXES_SWITCH_ 1

#ifdef __STDC__
#define _XERXES_PROTO_  1
#endif                /* end of __STDC__    */

#ifdef _MSC_VER
#ifndef _XERXES_PROTO_
#define _XERXES_PROTO_  1
#endif
#endif                /* end of _MSC_VER    */

#ifdef _XERXES_PROTO_
#define VOID void
#else
#define VOID
#endif               /* end of _XERXES_PROTO_ */

#endif               /* end of _XERXES_SWITCH_*/

#endif				 /* Endif for XERXESDEF_H */
