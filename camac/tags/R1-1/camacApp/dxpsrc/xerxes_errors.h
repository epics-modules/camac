/*
 *  xerxes_errors.h
 *
 *  Created 3-17-00:  JW: File to contain Error codes for DXP drivers.
 *
 *  Copyright 2000 X-ray Instrumentation Associates
 *  All rights reserved
 *
 */
#ifndef XERXES_ERROR_H
#define XERXES_ERROR_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

/*
 *  some error codes
 */
#define DXP_SUCCESS            0
/* I/O level error codes 1-100*/
#define DXP_MDOPEN             1
#define DXP_MDIO               2
#define DXP_MDINITIALIZE       3
#define DXP_MDLOCK			   4
/*  primitive level error codes (due to mdio failures) 101-200*/
#define DXP_WRITE_TSAR       101
#define DXP_WRITE_CSR        102
#define DXP_WRITE_WORD       103
#define DXP_READ_WORD        104
#define DXP_WRITE_BLOCK      105
#define DXP_READ_BLOCK       106
#define DXP_DISABLE_LAM      107
#define DXP_CLEAR_LAM        108
#define DXP_TEST_LAM         109
#define DXP_READ_CSR         110
#define DXP_WRITE_FIPPI      111
#define DXP_WRITE_DSP        112
#define DXP_WRITE_DATA       113
#define DXP_READ_DATA        114
#define DXP_ENABLE_LAM       115
#define DXP_READ_GSR         116
#define DXP_WRITE_GCR        117
/*  DSP/FIPPI level error codes 201-300  */
#define DXP_MEMERROR         201
#define DXP_DSPRUNERROR      202
#define DXP_FIPDOWNLOAD      203
#define DXP_DSPDOWNLOAD      204
#define DXP_INTERNAL_GAIN    205
#define DXP_RESET_WARNING    206
#define DXP_DETECTOR_GAIN    207
#define DXP_NOSYMBOL         208
#define DXP_SPECTRUM         209
#define DXP_DSPLOAD          210
#define DXP_DSPPARAMS        211
#define DXP_DSPACCESS        212
#define DXP_DSPPARAMBOUNDS   213
#define DXP_ADC_RUNACTIVE    214
#define DXP_ADC_READ		 215
#define DXP_ADC_TIMEOUT      216
/*  configuration errors  301-400  */
#define DXP_BAD_PARAM        301
#define DXP_NODECIMATION     302
#define DXP_OPEN_FILE        303
#define DXP_NORUNTASKS       304
#define DXP_OUTPUT_UNDEFINED 305
#define DXP_INPUT_UNDEFINED  306
#define DXP_ARRAY_TOO_SMALL  307
#define DXP_NOCHANNELS       308
#define DXP_NODETCHAN        309
#define DXP_NOIOCHAN         310
#define DXP_NOMODCHAN        311
#define DXP_NEGBLOCKSIZE     312
#define DXP_FILENOTFOUND     313
#define DXP_NOFILETABLE		 314
#define DXP_INITIALIZE	     315
#define DXP_UNKNOWN_BTYPE    316
#define DXP_NOMATCH          317
#define DXP_BADCHANNEL       318
#define DXP_DSPTIMEOUT       319
#define DXP_INITORDER        320
/*  host machine errors codes:  401-500 */
#define DXP_NOMEM            401
#define DXP_CLOSE_FILE       403
#define DXP_INDEXOOB         404
#define DXP_RUNACTIVE		 405
/* Debug support */
#define DXP_DEBUG           1001

#endif						/* Endif for XERXES_ERRORS_H */
