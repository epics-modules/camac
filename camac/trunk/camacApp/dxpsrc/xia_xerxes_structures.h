/*
 *  xia_xerxes_structures.h
 *
 *  Created 3-17-00:  JW: File to contain Data Structures for DXP drivers.
 *
 *  Copyright 2000 X-ray Instrumentation Associates
 *  All rights reserved
 *
 */
#ifndef XIA_XERXES_STRUCTURES_H
#define XIA_XERXES_STRUCTURES_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

/* 
 * Begin XerXes structures 
 */

/*
 * Structure containing the DSP parameter names
 */
struct Dsp_Params {
/* parameter names */
	char **pnames;
/* parameter access mode 0=RO, 1=RW, 2=WO */
	unsigned short *access;
/* parameter lower bound value */
	unsigned short *lbound;
/* parameter upper bound value */
	unsigned short *ubound;
/* number of parameters */
	unsigned short nsymbol;
/* Need the maximum number of symbols for allocating memory */
	unsigned short maxsym;
/* Need the maximum symbol length for allocating memory */
	unsigned short maxsymlen;
};
typedef struct Dsp_Params Dsp_Params;
/*
 * Linked list containing the default parameter information
 * obtained from DSP parameter configuration files
 */
struct Dsp_Defaults {
/* Filename identifying this entry */
	char *filename;
/* Structure containing the parameter information, only really used to store
 * the names of the parameters that change from their defaults. */
	struct Dsp_Params *params;
/* parameter values */
	unsigned short *data;
/* Pointer to the next entry */
	struct Dsp_Defaults *next;
};
typedef struct Dsp_Defaults Dsp_Defaults;
/*
 *	Linked list to contain all DSP configurations
 */
struct Dsp_Info {
	char *filename;
	unsigned short *data;
/* Need the number of words in the dsp program (length in size unsigned short) */
	unsigned int proglen;
/* Need the maximum program length for allocating memory */
	unsigned int maxproglen;
/* Structure containing parameter information */
	struct Dsp_Params *params;
	struct Dsp_Info *next;
};
typedef struct Dsp_Info Dsp_Info;
/*
 *	Linked list to contain all FiPPi configurations
 */ 
struct Fippi_Info {
	char *filename;
	unsigned short *data;
/* Need the program length for allocating memory */
	unsigned int proglen;
/* Need the maximum program length for general information */
	unsigned int maxproglen;
	struct Fippi_Info *next;
};
typedef struct Fippi_Info Fippi_Info;
/*
 *	Linked list to contain the Preamp Limits
 */ 
struct Preamp_Info {
/* Typical X-Ray Energy (KeV) */
	float energy;
/* Fraction of total ADC range to use for a single X-Ray step */
	float adcrule;
/* Factor to use to modify the gain for this channel */
	float gainmod;
/* Minimum Voltage level for the Preamp Output (V) */
	float vmin;
/* Maximum Voltage level for the Preamp Output (V) */
	float vmax;
/* Polarity of the X-Ray step: 0=negative, 1=positive */
	unsigned short polarity;
/* Typical step size for a single incident X-Ray (mV) */
	float vstep;
};
typedef struct Preamp_Info Preamp_Info;
/*
 *	Structure containing information about utility routines
 */
struct Utils {
	char *dllname;
	struct Xia_Util_Functions *funcs;
};
typedef struct Utils Utils;
/*
 *	Linked list containing pointers to libraries to interface with boards
 */
struct Interface {
	char *dllname;
	char *ioname;
	struct Xia_Io_Functions *funcs;
	struct Interface *next;
};
typedef struct Interface Interface;
/*
 *  Structure that points at the functions within a board type DLL
 */
typedef int (*DXP_INIT_DRIVER)(Interface *);
typedef int (*DXP_INIT_UTILS)(Utils *);
typedef int (*DXP_GET_DSPCONFIG)(Dsp_Info *);
typedef int (*DXP_GET_DSPINFO)(Dsp_Info *);
typedef int (*DXP_GET_FIPINFO)(Fippi_Info *);
typedef int (*DXP_GET_DEFAULTSINFO)(Dsp_Defaults *);
typedef int (*DXP_GET_FIPCONFIG)(Fippi_Info *);
typedef int (*DXP_DOWNLOAD_FIPCONFIG)(int *, int *, Fippi_Info *);
typedef int (*DXP_DOWNLOAD_FIPPI_DONE)(int *, int *, unsigned short *);
typedef int (*DXP_GET_DSPDEFAULTS)(Dsp_Defaults *);
typedef int (*DXP_DOWNLOAD_DSPCONFIG)(int *, int *, Dsp_Info *);
typedef int (*DXP_DOWNLOAD_DSP_DONE)(int *, int *, int*, Dsp_Info *, unsigned short *, float *);
typedef int (*DXP_CALIBRATE_CHANNEL)(int *, int *, unsigned short *, int *, Dsp_Info **);
typedef int (*DXP_CALIBRATE_ASC)(int *, int *, unsigned short *, Dsp_Info **);
typedef int (*DXP_LOOK_AT_ME)(int *, int *);
typedef int (*DXP_IGNORE_ME)(int *, int *);
typedef int (*DXP_CLEAR_LAM)(int *, int *);
typedef int (*DXP_LOC)(char *, Dsp_Info *, unsigned short *);
typedef int (*DXP_SYMBOLNAME)(unsigned short *, Dsp_Info *, char *);
typedef int (*DXP_READ_SPECTRUM)(int *, int *, Dsp_Info *, unsigned short *, unsigned long *);
typedef int (*DXP_TEST_SPECTRUM_MEMORY)(int *, int *, int *, Dsp_Info *, unsigned short *);
typedef unsigned int (*DXP_GET_SPECTRUM_LENGTH)(Dsp_Info *, unsigned short *);
typedef int (*DXP_READ_BASELINE)(int *, int *, Dsp_Info *, unsigned short *, unsigned short *);
typedef int (*DXP_TEST_BASELINE_MEMORY)(int *, int *, int *, Dsp_Info *, unsigned short *);
typedef unsigned int (*DXP_GET_BASELINE_LENGTH)(Dsp_Info *, unsigned short *);
typedef int (*DXP_TEST_EVENT_MEMORY)(int *, int *, int *, Dsp_Info *, unsigned short *);
typedef unsigned int (*DXP_GET_EVENT_LENGTH)(Dsp_Info *, unsigned short *);
typedef int (*DXP_WRITE_DSPPARAMS)(int *, int *, Dsp_Info *, unsigned short *);
typedef int (*DXP_WRITE_DSP_PARAM_ADDR)(int *, int *, unsigned short *, unsigned short *);
typedef int (*DXP_READ_DSPPARAMS)(int *, int *, Dsp_Info *, unsigned short *);
typedef int (*DXP_READ_DSPSYMBOL)(int *, int *, char *, Dsp_Info *, unsigned long *);
typedef int (*DXP_MODIFY_DSPSYMBOL)(int *, int *, char *, unsigned short *, Dsp_Info *);
typedef int (*DXP_DSPPARAM_DUMP)(int *, int *, Dsp_Info *);
typedef int (*DXP_BEGIN_RUN)(int *, int *, unsigned short *, unsigned short *);
typedef int (*DXP_END_RUN)(int *, int *);
typedef int (*DXP_RUN_ACTIVE)(int *, int *, int *);
typedef int (*DXP_GET_RUNSTATS)(unsigned short *, Dsp_Info *, unsigned int *, unsigned int *, 
								unsigned int *, unsigned int *, unsigned int *, double *);
typedef int (*DXP_DECODE_ERROR)(unsigned short *, Dsp_Info *, unsigned short *, unsigned short *);
typedef int (*DXP_CLEAR_ERROR)(int *, int *, Dsp_Info *);
typedef int (*DXP_CHANGE_GAINS)(int *, int *, int *, float *, Dsp_Info *);
typedef int (*DXP_SETUP_ASC)(int *, int *, int *, float *, float *, unsigned short *, 
								float *, float *, float *, Dsp_Info *);
typedef int (*DXP_PREP_FOR_READOUT)(int *, int *);
typedef int (*DXP_DONE_WITH_READOUT)(int *, int *, short [2]);
typedef int (*DXP_GET_ADC)(int *ioChan, int *modChan, Dsp_Info *dsp, unsigned short *data); 

struct Functions {
	DXP_INIT_DRIVER dxp_init_driver;
	DXP_INIT_UTILS dxp_init_utils;
	DXP_GET_DSPINFO dxp_get_dspinfo;
	DXP_GET_FIPINFO dxp_get_fipinfo;
	DXP_GET_DEFAULTSINFO dxp_get_defaultsinfo;
	DXP_GET_DSPCONFIG dxp_get_dspconfig;
	DXP_GET_FIPCONFIG dxp_get_fipconfig;
	DXP_GET_DSPDEFAULTS dxp_get_dspdefaults;
	DXP_DOWNLOAD_FIPCONFIG dxp_download_fipconfig;
	DXP_DOWNLOAD_FIPPI_DONE dxp_download_fippi_done;
	DXP_DOWNLOAD_DSPCONFIG dxp_download_dspconfig;
	DXP_DOWNLOAD_DSP_DONE dxp_download_dsp_done;
	DXP_CALIBRATE_CHANNEL dxp_calibrate_channel;
	DXP_CALIBRATE_ASC dxp_calibrate_asc;

	DXP_LOOK_AT_ME dxp_look_at_me;
	DXP_IGNORE_ME dxp_ignore_me;
	DXP_CLEAR_LAM dxp_clear_LAM;
	DXP_LOC dxp_loc;
	DXP_SYMBOLNAME dxp_symbolname;

	DXP_READ_SPECTRUM dxp_read_spectrum;
	DXP_TEST_SPECTRUM_MEMORY dxp_test_spectrum_memory;
	DXP_GET_SPECTRUM_LENGTH dxp_get_spectrum_length;
	DXP_READ_BASELINE dxp_read_baseline;
	DXP_TEST_BASELINE_MEMORY dxp_test_baseline_memory;
	DXP_GET_BASELINE_LENGTH dxp_get_baseline_length;
	DXP_TEST_EVENT_MEMORY dxp_test_event_memory;
	DXP_GET_EVENT_LENGTH dxp_get_event_length;
	DXP_WRITE_DSPPARAMS dxp_write_dspparams;
	DXP_WRITE_DSP_PARAM_ADDR dxp_write_dsp_param_addr;
	DXP_READ_DSPPARAMS dxp_read_dspparams;
	DXP_READ_DSPSYMBOL dxp_read_dspsymbol;
	DXP_MODIFY_DSPSYMBOL dxp_modify_dspsymbol;
	DXP_DSPPARAM_DUMP dxp_dspparam_dump;

	DXP_BEGIN_RUN dxp_begin_run;
	DXP_END_RUN dxp_end_run;
	DXP_RUN_ACTIVE dxp_run_active;
	DXP_DECODE_ERROR dxp_decode_error;
	DXP_CLEAR_ERROR dxp_clear_error;
	DXP_GET_RUNSTATS dxp_get_runstats;
	DXP_CHANGE_GAINS dxp_change_gains;
	DXP_SETUP_ASC dxp_setup_asc;

	DXP_PREP_FOR_READOUT dxp_prep_for_readout;
	DXP_DONE_WITH_READOUT dxp_done_with_readout;

	DXP_GET_ADC dxp_get_adc;
};
typedef struct Functions Functions;
/*
 *	Linked list to contain information about different board types
 */
struct Board_Info {
	char *name;
	char *pointer;
	int type;
	struct Functions *funcs;
	struct Board_Info *next;
};
typedef struct Board_Info Board_Info;
/*
 *	Define a system linked list for all boards in use
 */
struct Board {
/* CAMAC IO channel*/
	int ioChan;
/* Bit packed integer of which channels are used */
	unsigned short used;
/* Detector channel ID numbers (defined in DXP_MODULE) */
	int *detChan;
/* Module ID number (start counting at 0) */
	int mod;
/* Total number of channels on this board */
	unsigned int nchan;
/* String passed to the interface library, used to identify the board
 * or if need to restart the interface, for a typical CAMAC system contains
 * the SCSI bus number, crate number and slot number.  */
	char *iostring;
/* Array to hold misc information about the board state 
 * state[0] = run status (1=run started)
 * state[1] = gate used (1=ignore gate)
 * state[2] = lock value (1=locked, 0=free)
 * state[3] = HanDeL update status (1=needs update, 0=up to date)
 */
	short state[4];
/* Pointer to the IO interface library structure */
	struct Interface *iface;
/* Parameter Memory for these channels */
	unsigned short **params;
/* Pointer to the DSP Program file for each channel */
	struct Dsp_Info **dsp;
/* Pointer to the DSP Parameter Defaults for each channel */
	struct Dsp_Defaults **defaults;
/* Pointer to the FiPPi Program file for each channel */
	struct Fippi_Info **fippi;
/* Information about the Preamp associated with each channel */
	struct Preamp_Info *preamp;
/* Pointer to the structure of function calls */
	struct Board_Info *btype;
/* Pointer to the array of structures of DAQ information */
	struct DaqValues *daq;
/* Pointer to the array of structures of Firmware Deifinitions */
	struct FirmwareSet **firmware;
/* Pointer to next board in the linked list */
	struct Board *next;
};
typedef struct Board Board;
/* 
 * Define a struct to contain global information 
 */
struct System_Info {
/* Pointer to the first defined board type in the system */
	struct Board_Info *btypes;
/* The file that contains the PREAMP configuration */
	char *preamp;
/* The file that contains the module configurations */
	char *modules;
/* Pointer to the utils structure */
	Utils *utils;
/* Tell if the system was started from configuration or restore_config()
 * Hold the status bits of the system
 *
 * 0 = startup from default configuration
 * 1 = startup from saved configuration
 */
	short status;
};
typedef struct System_Info System_Info;

/* Begin Structure definitions for HanDeL (Hardware Dependent Layer)
 * While these routines are incorporated into XerXes, they are treated
 * as semi-autonomous.
 */

/*
 * Linked list containing the default DAQ settings
 * retained for the HanDeL library.  name-value pairs only.
 */
struct XiaDefaults {
/* Filename identifying this entry */
	char *alias;
/* Total number of entries in the arrays */
	unsigned short n;
/* Array containing the default DAQ settting names */
	char **name;
/* DAQ setting values array */
	float *data;
/* Pointer to the next entry */
	struct XiaDefaults *next;
};
typedef struct XiaDefaults XiaDefaults;
/* 
 * Define the linked list used to track peaking time ranges for firmware
 * definitions.  Peaking times are specified in nanoseconds. 
 */
struct Firmware {
/* Decimation */
	unsigned short decimation;
/* Min Peaking time */
	float min_ptime;
/* Max Peaking time */
	float max_ptime;
/* Point to the Fippi_Info for this definition */
	struct Fippi_Info *fippi;
/* Point to the Dsp_Info for this definition */
	struct Dsp_Info *dsp;
	struct Firmware *next;
};
typedef struct Firmware Firmware;
/* 
 * Define a linked list of firmware sets.  These are 'sets' of peaking
 * time definitions that can be referenced within the board structure
 * to allow boards to have arbitrary firmware definitions for arbitrary 
 * board combinations.
 */
struct FirmwareSet {
/* Give this set a name as a reference */
	char *alias;
/* Point to a Firmware structure */
	struct Firmware *firmware;
/* Point to the next element of the linked list */
	struct FirmwareSet *next;
};
typedef struct FirmwareSet FirmwareSet;
/* 
 * Define a linked list of Detectors.
 */
struct Detector {
/* Give this set a name as a reference */
	char *alias;
/* Number of channels for this detector */
	unsigned short nchan;
/* Array of polarities for all channels
 * 1 = positive
 * 0 = negative 
 */
	unsigned short *polarity;
/* Array of preamp gains for the channels in 
 * mv/KeV.
 */
	double *gain;
	struct Detector *next;
};
typedef struct Detector Detector;
/* 
 * Define a struct to contain acquisition values for each channel
 */
struct DaqValues {
/* Store the peaking time (in usec) */
	float peak;
/* Store the gap time (in usec) */
	float gap;
/* Store the trigger peaking time (in usec) */
	float tpeak;
/* Store the trigger gap time (in usec) */
	float tgap;
/* Store the trigger threshold (in eV) */
	float thresh;
/* Store the gain (in V/V) */
	float gain;
/* Store the binWidth (in eV) */
	float binwidth;
/* Store the number of MCA channels */
	unsigned long nmca;
/* Store the energy of the minimum MCA bin (in eV) */
	float min_mca;
/* Store the energy of the maximum MCA bin (in eV) */
	float max_mca;
/* Store the number of ROIs */
	unsigned int nroi;
/* Store the lower thresh for the ROIs (in eV) */
	float *low_roi;
/* Store the high thresh for the ROIs (in eV) */
	float *hi_roi;
};
typedef struct DaqValues DaqValues;

#endif						/* Endif for XIA_XERXES_STRUCTURES_H */

