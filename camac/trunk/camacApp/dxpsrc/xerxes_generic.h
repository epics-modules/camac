/*<##Wed Apr  3 17:20:53 2002--COUGAR--Do not remove--XIA##>*/

/*
 *  xerxes_generic.h
 *
 *  Major Mods 3-17-00 JW: Complete revamping of libraries
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Generic constant definitions
 */
#ifndef XERXES_GENERIC_H
#define XERXES_GENERIC_H


/* GLOBAL constants needed by both xia_xerxes.h and xerxes.h */

#define MAXDET				500
#define MAXSYM              500
#define MAXSYMBOL_LEN        15
#define MAXFILENAME_LEN     200
#define MAXDXP				100		/* Maximum number of DXP modules in system */
#define MAXBOARDNAME_LEN	 20


/* Definitions for DXP4C configurations */

/*
 *    Control Task codes
 */
#define CT_DXP4C_RESETASC	100			/* Reset Slope Generator */
#define CT_DXP4C_TRKDAC		102			/* TrackDAC calibration	*/
#define CT_DXP4C_DACSET		104			/* Set the tracking, slope and gain DACs */
#define CT_DXP4C_ADCTEST	105			/* Test the ADC Linearity */
#define CT_DXP4C_ASCMON		106			/* Monitor ASC interrupts */
#define CT_DXP4C_RESET		107			/* Reset calibration */
#define CT_DXP4C_FIPTRACE	109			/* Acquire Decimated Trace data */
#define CT_DXP4C_ADC        110			/* Acquire an ADC trace */
/* Kept for backwards compatibility */
#define CALIB_TRKDAC		  1
#define CALIB_RESET			  2

/* 
 * Definitions for DXP4C2X configurations 
 */
#define CT_DXP2X_SET_ASCDAC        200
#define CT_DXP2X_TRKDAC            202
#define CT_DXP2X_SLOPE_CALIB       203
#define CT_DXP2X_SLEEP_DSP         206
#define CT_DXP2X_PROGRAM_FIPPI     211
#define CT_DXP2X_SET_POLARITY      212
#define CT_DXP2X_CLOSE_INPUT_RELAY 213
#define CT_DXP2X_OPEN_INPUT_RELAY  214
#define CT_DXP2X_RC_BASELINE       215
#define CT_DXP2X_RC_EVENT	       216
#define CT_DXP2X_ADC			   217
#define CT_DXP2X_BASELINE_HIST     218	/* Special run for Baseline History		*/
#define CT_DXP2X_RESET             299

/* 
 * Definitions for DXPX10P configurations 
 */
#define CT_DXPX10P_SET_ASCDAC        300
#define CT_DXPX10P_TRKDAC            302
#define CT_DXPX10P_SLOPE_CALIB       303
#define CT_DXPX10P_SLEEP_DSP         306
#define CT_DXPX10P_PROGRAM_FIPPI     311
#define CT_DXPX10P_SET_POLARITY      312
#define CT_DXPX10P_CLOSE_INPUT_RELAY 313
#define CT_DXPX10P_OPEN_INPUT_RELAY  314
#define CT_DXPX10P_RC_BASELINE       315
#define CT_DXPX10P_RC_EVENT          316
#define CT_DXPX10P_ADC               317
#define CT_DXPX10P_BASELINE_HIST     318	/* Special run for Baseline History		*/
#define CT_DXPX10P_RESET             399

/* 
 * Definitions for DGFG200 configurations 
 */
#define CT_DGFG200_SETDACS            400
#define CT_DGFG200_CLOSE_INPUT_RELAY  401
#define CT_DGFG200_OPEN_INPUT_RELAY   402
#define CT_DGFG200_RAMP_OFFSET_DAC    403
#define CT_DGFG200_ADC		          404
#define CT_DGFG200_PROGRAM_FIPPI      405
#define CT_DGFG200_READ_MEMORY_FIRST  409
#define CT_DGFG200_READ_MEMORY_NEXT   410
#define CT_DGFG200_WRITE_MEMORY_FIRST 411
#define CT_DGFG200_WRITE_MEMORY_NEXT  412
#define CT_DGFG200_MEASURE_NOISE      419
#define CT_DGFG200_ADC_CALIB_FIRST    420
#define CT_DGFG200_ADC_CALIB_NEXT     421

/* Generic Special Run Definitions (Applicable to all boards) */

#define CT_ADC             5000		/* Special run for ADC trace readout	*/

#endif						/* Endif for XERXES_GENERIC_H */
