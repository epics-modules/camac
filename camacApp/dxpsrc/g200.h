/*
 *  g200.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for g200.c routines
 */
#ifndef G200_H
#define G200_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

#ifndef XERXES_GENERIC_H
#include <xerxes_generic.h>
#endif

#define CODE_VERSION					   0
#define CODE_REVISION		  			   1
#define LATEST_BOARD_TYPE		  "DGF-G200"
#define LATEST_BOARD_VERSION		     "D"
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
 * Control words for the RUNTASKS parameter
 */
#define CONTROL_TASK_RUN      0
#define DAQ_RUN               1
#define FAST_LIST_MODE_RUN    2
#define MCA_RUN               3
#define SHORT_PSA_RUN         4
/*
 *    Calibration control codes
 */
#define CONTROLTASK_SETDACS             0
#define CONTROLTASK_CLOSE_INPUT_RELAY   1
#define CONTROLTASK_OPEN_INPUT_RELAY    2
#define CONTROLTASK_RAMP_OFFSET_DAC     3
#define CONTROLTASK_ADC_TRACE           4
#define CONTROLTASK_PROGRAM_FIPPI       5
#define CONTROLTASK_READ_MEMORY_FIRST   9
#define CONTROLTASK_READ_MEMORY_NEXT   10
#define CONTROLTASK_WRITE_MEMORY_FIRST 11
#define CONTROLTASK_WRITE_MEMORY_NEXT  12
#define CONTROLTASK_MEASURE_NOISE      19
#define CONTROLTASK_ADC_CALIB_FIRST    20
#define CONTROLTASK_ADC_CALIB_NEXT     21
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
#define MASK_UNUSED2      0x0004
#define MASK_LAM_ENABLE   0x0008
#define MASK_DSPRESET     0x0010
#define MASK_FIPRESET     0x0020
#define MASK_UNUSED6      0x0040
#define MASK_SYNCH_FLAG   0x0080
#define MASK_FPGA_ERROR   0x0100
#define MASK_UNUSED9      0x0200
#define MASK_UNUSED10     0x0400
#define MASK_UNUSED11     0x0800
#define MASK_DSP_ERROR    0x1000
#define MASK_RUN_ACTIVE   0x2000
#define MASK_LAM_REQUEST  0x4000
#define MASK_G200_LIVE    0x8000

/*
 *     CHANCSRA parameter bits
 */
#define CCSRA_EXTTRIG     0x0001
#define CCSRA_SPARE1      0x0002
#define CCSRA_SPARE2      0x0004
#define CCSRA_SPARE3      0x0008
#define CCSRA_SPARE4      0x0010
#define CCSRA_POLARITY    0x0020
#define CCSRA_GATE        0x0040
#define CCSRA_HISTOGRAM   0x0080
#define CCSRA_SPARE8      0x0100
#define CCSRA_BDEFICIT    0x0200
#define CCSRA_CFD         0x0400
#define CCSRA_MULT        0x0800
#define CCSRA_PRESET_LIVE 0x1000
#define CCSRA_SPARE13     0x2000
#define CCSRA_SPARE14     0x4000
#define CCSRA_BIPOLAR     0x8000

/* Definitions of EPP addresses for the G200 boards 
 * F code for write is 1, read is 0
 * A code is the EPP register for the xfer
 *    0=Data register,    1=Address register
 *    2=Control register, 3=Status register
 */
#define G200_TSAR_F_WRITE         1
#define G200_TSAR_A_WRITE         1
#define G200_TSAR_F_READ          0
#define G200_TSAR_A_READ          1
#define G200_CSR_F_WRITE          1
#define G200_CSR_A_WRITE          0
#define G200_CSR_F_READ           0
#define G200_CSR_A_READ           0
#define G200_DATA_F_READ          0
#define G200_DATA_A_READ          0
#define G200_DATA_F_WRITE         1
#define G200_DATA_A_WRITE         0
#define G200_WCR_F_READ           0
#define G200_WCR_A_READ           0
#define G200_WCR_F_WRITE          1
#define G200_WCR_A_WRITE          0
#define G200_FIPPI_F_WRITE        1
#define G200_FIPPI_A_WRITE        0

/* Now define the addresses */
#define G200_CSR_ADDRESS     0x8000
#define G200_FIPPI_ADDRESS   0x8003
#define G200_WCR_ADDRESS     0x8004

#endif						/* Endif for X10P_H */
