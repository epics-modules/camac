/*<##Wed Apr  3 17:20:53 2002--COUGAR--Do not remove--XIA##>*/

/*
 *  dxp4c2x.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp4c2x.c routines
 */
#ifndef DXP4C2X_H
#define DXP4C2X_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

#ifndef XERXES_GENERIC_H
#include <xerxes_generic.h>
#endif

#define CODE_VERSION					   0
#define CODE_REVISION		  			   1
#define LATEST_BOARD_TYPE		  "DXP-4C2X"
#define LATEST_BOARD_VERSION		     "C"
/*
 *    
 */
#define MAXDET				500
#define PROGRAM_BASE     0x0000
#define DATA_BASE        0x4000
#define START_PARAMS     0x4000
#define MAXDSP_LEN       0x8000
#define MAXFIP_LEN   0x00020000
#define MAX_FIPPI			  5
#define LIVECLOCK_TICK_TIME 4.e-7f
/* 
 * Control words for the RUNTASKS parameter
 */
#define UPDATE_DAC		  0x002
#define USE_FIR			  0x004
#define ACQUIRE_BASELINE  0x008
#define ADJUST_FAST_FILT  0x010
#define BASELINE_SHIFT	  0x020
#define RESIDUAL_BASE	  0x040
#define WRITE_BASELINE	  0x080
#define CONTROL_TASK	  0x100
#define DELTA_BASELINE	  0x200
#define BASELINE_CUT	  0x400

/*
 *    Calibration control codes
 */
#define WHICHTEST_SET_ASCDAC         0
#define WHICHTEST_ACQUIRE_ADC        1
#define WHICHTEST_TRKDAC             2
#define WHICHTEST_SLOPE_CALIB        3
#define WHICHTEST_SLEEP_DSP          6
#define WHICHTEST_PROGRAM_FIPPI     11
#define WHICHTEST_SET_POLARITY      12
#define WHICHTEST_CLOSE_INPUT_RELAY 13
#define WHICHTEST_OPEN_INPUT_RELAY  14
#define WHICHTEST_RC_BASELINE       15
#define WHICHTEST_RC_EVENT          16
#define WHICHTEST_RESET             99
/*
 * ASC parameters:
 */
#define ADC_RANGE		 1000.0
#define ADC_BITS			 12
#define GINPUT              1.0		/* Input attenuator Gain	*/
#define GINPUT_BUFF	        1.0		/* Input buffer Gain		*/
#define GINVERTING_AMP 3240./499.	/* Inverting Amp Gain		*/
#define GV_DIVIDER  124.9/498.9		/* Voltage Divider after 
									   Inverting AMP */
#define GGAINDAC_BUFF       1.0		/* GainDAC buffer Gain		*/
#define GNYQUIST      422./613.		/* Nyquist Filter Gain		*/
#define GADC_BUFF           2.0		/* ADC buffer Gain			*/
#define GADC          250./350.		/* ADC Input Gain			*/
#define GAINDAC_RANGE	 3000.0
#define GAINDAC_BITS		 16
#define GAINDAC_MIN			-6.
#define GAINDAC_MAX		    30.
/*
 *    CAMAC status Register control codes
 */
#define ALLCHAN              -1
#define IGNOREGATE            1
#define USEGATE               0
#define CLEARMCA              0
#define UPDATEMCA             1
/*
 *     CAMAC status Register bit positions
 */
#define MASK_RUNENABLE    0x0001
#define MASK_RESETMCA     0x0002
#define MASK_UNUSED1      0x0004
#define MASK_LAMENABLE    0x0008
#define MASK_DSPRESET     0x0010
#define MASK_FIPRESET     0x0020
#define MASK_ALLCHAN      0x0100
#define MASK_UNUSED2      0x0200
#define MASK_UNUSED3      0x0400
#define MASK_IGNOREGATE   0x0800
#define MASK_UNUSED5      0x1000
#define MASK_UNUSED6      0x2000
#define MASK_UNUSED7      0x4000
#define MASK_UNUSED8      0x8000

/* Definitions of CAMAC addresses for the DXP boards */
#define DXP_TSAR_F_WRITE		17
#define DXP_TSAR_A_WRITE		1
#define DXP_CSR_F_WRITE			17
#define DXP_CSR_A_WRITE			0
#define DXP_CSR_F_READ			1
#define DXP_CSR_A_READ			0
#define DXP_GSR_F_READ			1
#define DXP_GSR_A_READ			2
#define DXP_CHAN_GCR_F_WRITE	17
#define DXP_CHAN_GCR_A_WRITE	5
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

#endif						/* Endif for DXP4C2X_H */
