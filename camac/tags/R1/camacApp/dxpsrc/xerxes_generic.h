/*
 *  xerxes_generic.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
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

#define MAXSYMBOL_LEN		13
#define MAXFILENAME_LEN		200
#define MAXBOARDNAME_LEN	20

/* Definitions for DXP4C configurations */

/*
 *    Calibration control codes
 */
#define SPECIAL_RUN_TRKDAC	100			/* Special run for TrackDAC calibration	*/
#define SPECIAL_RUN_RESET	101			/* Special run for Reset calibration	*/
#define CALIB_TRKDAC	1
#define CALIB_RESET		2

/* Definitions for DXP4C2X configurations */

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
#define SPECIAL_TASK	  0x100
#define DELTA_BASELINE	  0x200
#define BASELINE_CUT	  0x400

/* Definitions for DXPX10P configurations */

/*
 *    Calibration control codes
 */
#define SET_ASCDAC			  0
#define ACQUIRE_ADC			  1
#define TRKDAC                2
#define SLOPE_CALIB			  3
#define SLEEP_DSP			  6
#define PROGRAM_FIPPI		 11
#define SET_POLARITY		 12
#define CLOSE_INPUT_RELAY    13
#define OPEN_INPUT_RELAY	 14
#define RC_BASELINE			 15
#define RC_EVENT			 16
#define RESET                99

/* Generic Special Run Definitions (Applicable to all boards) */

#define SPECIAL_RUN_ADC		5000		/* Special run for ADC trace readout	*/
#define SPECIAL_RUN_BLH		5001		/* Special run for Baseline History		*/

#endif						/* Endif for XERXES_H */
