/* devMCA_DXP.c */

/* devMCA_DXP.c - Device Support Routines for XIA DXP MCA */
/*
 *      Author:  Mark Rivers
 *      Date:    10/27/96
 *
 *      Copyright 1996, the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contract
 *      W-31-109-ENG-38 at Argonne National Laboratory.
 *
 * Modification Log:
 * -----------------
 * .01  10/26/96        mlr     Developed from AIM MCA device support code
 * .02  06/27/98        mlr     Minor fixes for compiler warnings
 * .03  05/18/01        mlr     Covert to XerXes version of DXP software
 */


#include	<vxWorks.h>
#include	<types.h>
#include        <stdlib.h>
#include	<stdioLib.h>
#include	<wdLib.h>
#include	<memLib.h>
#include	<string.h>
#include        <time.h>

#include	<alarm.h>
#include	<dbDefs.h>
#include	<dbAccess.h>
#include	<recSup.h>
#include	<devSup.h>
#include	<link.h>
#include	<dbCommon.h>

#include	"mcaRecord.h"
#include	"mca.h"
#include        "xerxes_structures.h"
#include        "xerxes.h"

/* Debug support */
#if 0
#define Debug(l,FMT,V) ;
#else
#define Debug(l,FMT,V...) {if (l <= devMCA_DXPDebug) \
			  { printf("%s(%d):",__FILE__,__LINE__); \
                            printf(FMT,##V);}}
#endif
volatile int devMCA_DXPDebug = 0;

/*
* This structure contains the device specific information for each record.
* It is allocated and pointed to by DPVT
*/
typedef struct {
	int    module;     /* Number returned by mdopen_bcn()                */
        int    erased;     /* Flag indicating if module has been erased      */
        int    acquiring;  /* Flag indicating if module is current acquiring */
        double start_time; /* The time that acquisition was started          */
} DXP_SETUP_STRUCT;

/* Create DSET */
static long init_record();
static long send_msg();
static long read_array();
typedef struct {
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	send_msg;
	DEVSUPFUN	read_array;
} MCA_DXP_DSET;

MCA_DXP_DSET devMCA_DXP = {
	6,
	NULL,
	NULL,
	init_record,
	NULL,
	send_msg,
	read_array
};
static double get_current_time();


static long init_record(pmca)
struct mcaRecord *pmca;
{
	DXP_SETUP_STRUCT *dxp_setup;

	Debug(5, "devMCA_DXP(init_record): entry\n");
	/* pmca->inp must be VME_IO */
	if (pmca->inp.type != VME_IO) {
            recGblRecordError(S_db_badField,(void *)pmca,
                "devMCA_DXP (init_record) Illegal INP field");
            return(S_db_badField);
        }
        dxp_setup = (DXP_SETUP_STRUCT *) malloc(sizeof(DXP_SETUP_STRUCT));
        pmca->dpvt = (void *)dxp_setup;
        pmca->nord = 0;
        dxp_setup->module = pmca->inp.value.vmeio.card;
        dxp_setup->erased = 1;
	return(0);
}

static long send_msg(pmca, msg, parg)
struct mcaRecord *pmca;
unsigned long msg;
void *parg;
{
        int i;
        int s=0;
        unsigned short resume;
        unsigned short gate=1;
	int nuse = pmca->nuse;
        double time_now;
        double livetime, icr, ocr;
        unsigned long nevent;
	long *lptr = (long *)pmca->bptr;
	DXP_SETUP_STRUCT *dxp_setup = (DXP_SETUP_STRUCT *) pmca->dpvt;

	switch (msg) {
	case MSG_ACQUIRE:
		/* start acquisition */
                dxp_setup->start_time = get_current_time();
                if (dxp_setup->erased) resume=0; else resume=1;
                dxp_start_one_run(&dxp_setup->module, &gate, &resume);
                dxp_setup->acquiring = 1;
                dxp_setup->erased = 0;
		Debug(5, "devMCA_DXP(send_msg): start acquisition %d\n", s);
		break;
	case MSG_READ:
		/* start read operation */
		/* This is a no-op. Read-array does everything. */
		break;
	case MSG_SET_CHAS_INT:
		/* set channel advance source to internal (timed) */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_CHAS_EXT:
		/* set channel advance source to external */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_NCHAN:
		/* set number of channels */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_DWELL:
		/* set dwell time per channel. Convert to centiseconds */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_REAL_TIME:
		/* set preset real time. */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_LIVE_TIME:
		/* set preset live time. */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_COUNTS:
		/* set preset counts */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_LO_CHAN:
		/* set lower side of region integrated for preset counts */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_HI_CHAN:
		/* set high side of region integrated for preset counts */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_NSWEEPS:
		/* set number of sweeps (for MCS mode) */
		/* This is a NOOP on current version of DXP */
		break;
	case MSG_SET_MODE_PHA:
		/* set mode to Pulse Height Analysis */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_MODE_MCS:
		/* set mode to MultiChannel Scaler */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_SET_MODE_LIST:
		/* set mode to LIST (record each incoming event) */
		/* This is a NOOP for current DXP hardware */
		break;
	case MSG_GET_ACQ_STATUS:
		/* Read the live time */
                s = dxp_get_statistics(&dxp_setup->module,
                                   &livetime,
                                   &icr,
                                   &ocr,
                                   &nevent);
		Debug(5, "devMCA_DXP(send_msg): get status=%d, livetime=%f, nevent=%ld\n", 
                          s, livetime, nevent);
                if (dxp_setup->acquiring) {
                    time_now = get_current_time();
		    pmca->ertm += time_now - dxp_setup->start_time;
                    dxp_setup->start_time = time_now;
                }
		pmca->eltm = livetime;
		pmca->act =  0.0;
                /* Compute counts in preset count window               */
                if ((pmca->pctl > 0) && 
                    (pmca->pcth > 0) && 
                    (pmca->pctl > pmca->pcth)) {
                    for (i=pmca->pctl; i<=pmca->pcth; i++) 
                            pmca->act += lptr[i];
                }
                /* See if any of the presets have been exceeded        */
                if (((pmca->prtm != 0.0) && (pmca->ertm > pmca->prtm)) ||
                    ((pmca->pltm != 0.0) && (pmca->eltm > pmca->pltm)) ||
                    ((pmca->pct  != 0.0) && (pmca->act  > pmca->pct))) {
                   dxp_setup->acquiring = 0;
                }
		pmca->acqg = dxp_setup->acquiring;
		break;
	case MSG_STOP_ACQUISITION:
		/* stop data acquisition */
		s = dxp_stop_one_run(&dxp_setup->module);
                if (dxp_setup->acquiring) {
                    time_now = get_current_time();
		    pmca->ertm += dxp_setup->start_time - time_now;
                    dxp_setup->start_time = time_now;
                    dxp_setup->acquiring = 0;
                }
		Debug(5, "devMCA_DXP(send_msg): stop acquisition %d\n", s);
		break;
	case MSG_ERASE:
		/* erase */
		Debug(5, "devMCA_DXP(send_msg): erase %d\n", s);
                dxp_setup->erased = 1;
                for (i=0; i<nuse; i++) lptr[i]=0;
                pmca->ertm = 0.;
                pmca->eltm = 0.;
                pmca->act  = 0.;
		break;
	}
	return(0);
}

static long read_array(pmca)
struct mcaRecord *pmca;
{
	int s=0;
	int nuse = pmca->nuse;
        unsigned short gate=1;
        unsigned short resume=1;
	unsigned long *lptr = (unsigned long *)pmca->bptr;
        DXP_SETUP_STRUCT *dxp_setup = (DXP_SETUP_STRUCT *) pmca->dpvt;

        s=dxp_readout_detector_run(&dxp_setup->module, NULL, NULL, lptr);
        Debug(5, "devMCA_DXP(read_array): %d\n", s);
	pmca->nord = nuse;
	return(s);
}

static double get_current_time()
{
    struct timespec timespec;
    clock_gettime(CLOCK_REALTIME, &timespec);
    return (timespec.tv_sec + timespec.tv_nsec*1.e-9);
}
