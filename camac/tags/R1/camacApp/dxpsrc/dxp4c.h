/*
 *  dxp4c.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp4c.c routines
 */
#ifndef DXP4C_H
#define DXP4C_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

#ifndef XERXES_GENERIC_H
#include <xerxes_generic.h>
#endif

#define CODE_VERSION			     0
#define CODE_REVISION		  	     1
#define LATEST_BOARD_TYPE	"DXP-4C\0"
#define LATEST_BOARD_VERSION	 "C\0"
/*
 *    
 */
#define MAXDET				500
#define START_PARAMS     0x0000
#define START_SPECTRUM   0x0000
#define START_BASELINE   0x0200
#define START_EVENT      0x0400
#define MAXSYM              256
#define MAXSPEC            1024
#define MAXBASE             512
#define MAXEVENT           1024
#define MAXDSP_LEN       0x2000
#define MAXFIP_LEN   0x00020000
#define LIVECLOCK_TICK_TIME 8.e-7f
/*
 * ASC parameters:
 */
#define ADC_RANGE		 2000.0
#define ADC_BITS			 10
#define OFFDAC_FACTOR      24.0
#define FINEGAIN_FACTOR  426.67
#define GAIN3_MAX          35.1
#define GAIN3_MIN          9.38
#define GAIN3_FAC          8.82
#define GAIN2_MIN          2.89
#define GAIN2_FAC          2.49
#define GAIN1_MIN         0.903
#define GAIN1_FAC         0.820
#define GAIN0_MIN         0.246
/*
 *    CAMAC status Register control codes
 */
#define XMEM                  0
#define YMEM                  1
#define ALLCHAN              -1
#define INCRADD               0
#define CONSTADD              1
#define IGNOREGATE            1
#define USEGATE               0
#define CLEARMCA              0
#define UPDATEMCA             1
/*
 *     CAMAC status Register bit positions
 */
#define MASK_YMEM         0x0001
#define MASK_WRITE        0x0002
#define MASK_CAMXFER      0x0004
#define MASK_RUNENABLE    0x0008
#define MASK_RESETMCA     0x0010
#define MASK_CONSTADD     0x0020
#define MASK_ALLCHAN      0x0100
#define MASK_DSPRESET     0x0200
#define MASK_FIPRESET     0x0400
#define MASK_IGNOREGATE   0x0800

/* Definitions of CAMAC addresses for the DXP boards */
#define DXP_TSAR_F_WRITE		17
#define DXP_TSAR_A_WRITE		1
#define DXP_CSR_F_WRITE			17
#define DXP_CSR_A_WRITE			0
#define DXP_CSR_F_READ			1
#define DXP_CSR_A_READ			0
#define DXP_DATA_F_READ			0
#define DXP_DATA_A_READ			0
#define DXP_DATA_F_WRITE		16
#define DXP_DATA_A_WRITE		0
#define DXP_DISABLE_LAM_F		24
#define DXP_DISABLE_LAM_A		0
#define DXP_ENABLE_LAM_F		26
#define DXP_ENABLE_LAM_A		0
#define DXP_TEST_LAM_SOURCE_F	27
#define DXP_TEST_LAM_SOURCE_A	0
#define DXP_TEST_LAM_F			8
#define DXP_TEST_LAM_A			0
#define DXP_CLEAR_LAM_F			10
#define DXP_CLEAR_LAM_A			0
#define DXP_FIPPI_F_WRITE		17
#define DXP_FIPPI_A_WRITE		3
#define DXP_DSP_F_WRITE			17
#define DXP_DSP_A_WRITE			2

#endif						/* Endif for DXP4C_H */

