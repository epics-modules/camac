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
HANDEL_EXPORT int HANDEL_API xiaInit(void);
HANDEL_EXPORT int HANDEL_API xiaInitHandel(void);

HANDEL_STATIC int HANDEL_API xiaInitHandelDS(void);
HANDEL_STATIC int HANDEL_API xiaReadIniFile(void);
HANDEL_STATIC int HANDEL_API xiaFindEntryLimits(FILE *fp, const char *section, fpos_t *start, fpos_t *end);
HANDEL_STATIC int HANDEL_API xiaGetLine(FILE *fp, char *line);
HANDEL_STATIC int HANDEL_API xiaGetLineData(char *line, char *name, char *value);
HANDEL_STATIC int HANDEL_API xiaInitDetectorDS(void);
HANDEL_STATIC int HANDEL_API xiaFreeDetector(Detector *detector);
HANDEL_STATIC int HANDEL_API xiaInitFirmwareSetDS(void);
HANDEL_STATIC int HANDEL_API xiaFreeFirmwareSet(FirmwareSet *firmwareSet);
HANDEL_STATIC int HANDEL_API xiaFreeFirmware(Firmware *firmware);
HANDEL_STATIC int HANDEL_API xiaInitDaqValuesDS(void);
HANDEL_STATIC int HANDEL_API xiaFreeDaqValues(Board *board);

#else									/* Begin old style C prototypes */
/*
 * following are internal prototypes for handel layer subset of xerxes.c routines
 */
HANDEL_EXPORT int HANDEL_API xiaInit();
HANDEL_EXPORT int HANDEL_API xiaInitHandel();

HANDEL_STATIC int HANDEL_API xiaInitHandelDS();
HANDEL_STATIC int HANDEL_API xiaReadIniFile();
HANDEL_STATIC int HANDEL_API xiaFindEntryLimits();
HANDEL_STATIC int HANDEL_API xiaGetLine();
HANDEL_STATIC int HANDEL_API xiaGetLineData();
HANDEL_STATIC int HANDEL_API xiaInitDetectorDS();
HANDEL_STATIC int HANDEL_API xiaFreeDetector();
HANDEL_STATIC int HANDEL_API xiaInitFirmwareSetDS();
HANDEL_STATIC int HANDEL_API xiaFreeFirmwareSet();
HANDEL_STATIC int HANDEL_API xiaFreeFirmware();
HANDEL_STATIC int HANDEL_API xiaInitDaqValuesDS();
HANDEL_STATIC int HANDEL_API xiaFreeDaqValues();

#endif                                  /*   end if _HANDEL_PROTO_ */

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
}
#endif

#endif						/* Endif for XIA_HANDEL_H */
