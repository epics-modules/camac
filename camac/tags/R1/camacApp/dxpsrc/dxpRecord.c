/* recdxp.c - Record Support Routines for DXP record
 *
 * This record works with both the DXP4C and the DXP2X modules.
 *
 *      Author:         Mark Rivers
 *      Date:           1/24/97
 *
 * Modification Log:
 * -----------------
 * 06/27/98     mlr    Minor fixes for compiler warnings
 * 06/01/01     mlr    Complete rewrite for Xerxes version of XIA software and
 *                     to use MPF server
 */

#include        <stdlib.h>
#include        <vxWorks.h>
#include        <types.h>
#include        <rngLib.h>
#include        <stdioLib.h>
#include        <lstLib.h>
#include        <string.h>

#include        <alarm.h>
#include        <dbDefs.h>
#include        <dbAccess.h>
#include        <dbFldTypes.h>
#include        <dbEvent.h>
#include        <devSup.h>
#include        <drvSup.h>
#include        <errMdef.h>
#include        <recSup.h>
#define GEN_SIZE_OFFSET
#include        "dxpRecord.h"
#undef GEN_SIZE_OFFSET
#include        "mca.h"
#include        "dxpServer.h"
#include        "xerxes_structures.h"
#include        "xerxes.h"


/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
static long init_record();
static long process();
static long special();
#define get_value NULL
static long cvt_dbaddr();
static long get_array_info();
#define put_array_info NULL
#define get_units NULL
#define get_precision NULL
static long get_enum_str();
static long get_enum_strs();
static long put_enum_str();
static long get_graphic_double();
static long get_control_double();
#define get_alarm_double NULL

static long monitor(struct dxpRecord *pdxp);
static void setDxpTasks(struct dxpRecord *pdxp);
static void setPeakingTime(struct dxpRecord *pdxp);

struct rset dxpRSET={
        RSETNUMBER,
        report,
        initialize,
        init_record,
        process,
        special,
        get_value,
        cvt_dbaddr,
        get_array_info,
        put_array_info,
        get_units,
        get_precision,
        get_enum_str,
        get_enum_strs,
        put_enum_str,
        get_graphic_double,
        get_control_double,
        get_alarm_double };

struct dxpDSET { /* DXP DSET */
    long            number;
    DEVSUPFUN       dev_report;
    DEVSUPFUN       init;
    DEVSUPFUN       init_record; /*returns: (-1,0)=>(failure,success)*/
    DEVSUPFUN       get_ioint_info;
    DEVSUPFUN       send_msg; /*returns: (-1,0)=>(failure,success)*/
    DEVSUPFUN       read_array; /*returns: (-1,0)=>(failure,success)*/
};


/* The dxp record contains many short integer parameters which control the
 * DXP analog signal processing and digital filtering.
 * The record .dbd file defines 3 fields for each of these parameters:
    short xxnV
        ; The value of the parameter. Default values for some parameters are
        ; set in the .dbd file.
    char  xxnL[DXP_STRING_SIZE]
        ; The label for the parameter, which is how we look up its address
        ; with the DXP software.  These labels are defined in dxp.db, since one
        ; cannot assign the initial value of DBF_STRING fields in the .dbd
        ; file, and the values are different for the DXP4C and DXP2X.
    short xxnO
        ; The offset of this parameter within the DXP memory.  These offsets are
        ; obtained during the record initialization phase from the labels
        ; described above.
        ; Given the above fields, much of the record processing is done using
        ; loops and a pointer to a structure which is this field
 */
/* The dxp record contains many long integer parameters which are used for
 *  reading out statistical information about the run.
 * The record .dbd file defines 3 fields for each of these parameters:
    long xxnV
        ; The value of the parameter.
    char  xxnL[DXP_STRING_SIZE]
        ; The label for the first 16 bit word of the parameter, which is how 
        ; we look up its address with the DXP software.  These labels are 
        ; defined in dxp.db, since one cannot assign the initial value of 
        ; DBF_STRING fields in the .dbd file, and the values are different 
        ; for the DXP4C and DXP2X.
    short xxnO
        ; The offset of this parameter within the DXP memory.  These offsets are
        ; obtained during the record initialization phase from the labels
        ; described above.
        ; Given the above fields, much of the record processing is done using
        ; loops and a pointer to a structure which is this field
 */
/* The dxp record contains many Yes/No menu parameters which are used for 
 * defining the "Runtasks" tasks to be perfomed.  The parameters must be 
 * defined in the order in which the bits occur in the "Runtasks" word.
 * The record .dbd file defines 2 fields for each of these parameters:
    DBF_MENU TnnV
        ; The value of the parameter. 0/1 corresponding to No/Yes.
    char  TnnL[DXP_TASK_STRING_SIZE]
        ; A descriptive string for the task.
 */
#define DXP_STRING_SIZE      16
#define DXP_TASK_STRING_SIZE 25

typedef struct  {
    unsigned short   val;
    char             label[DXP_STRING_SIZE];
    short            offset;
} DXP_SHORT_PARAM;

typedef struct  {
    long     val;
    char    label[DXP_STRING_SIZE];
    short   offset;
} DXP_LONG_PARAM;

typedef struct  {
    unsigned short   val;
    char    label[DXP_TASK_STRING_SIZE];
} DXP_TASK_PARAM;

#define NUM_ASC_PARAMS       9 /* The number of short ASC parameters described 
                                * above in the record */
#define NUM_FIPPI_PARAMS    10 /* The number of short FIPPI parameters described
                                * above in the record */
#define NUM_DSP_PARAMS      10 /* The number of short DSP parameters described
                                *  above in the record */
#define NUM_BASELINE_PARAMS  6 /* The number of short BASELINE parameters 
                                * described above in the record */
#define NUM_READONLY_PARAMS 15 /* The number of short READ-ONLY parameters 
                                * described above in the record */
#define NUM_DXP_LONG_PARAMS 14 /* The number of long (32 bit) statistics 
                                * parametersdescribed above in the record. 
                                * These are also read-only */
#define NUM_TASK_PARAMS     10 /* The number of task parameters described above
                                *  in the record. */


#define NUM_SHORT_WRITE_PARAMS \
   (NUM_ASC_PARAMS + NUM_FIPPI_PARAMS + NUM_DSP_PARAMS + NUM_BASELINE_PARAMS)
#define NUM_SHORT_READ_PARAMS (NUM_SHORT_WRITE_PARAMS +  NUM_READONLY_PARAMS)

/* The number of module types.  Currently 2: DXP4C and DXP2X. */
#define MAX_MODULE_TYPES 2
#define DXP4C 0
#define DXP2X 1

#define MAX_PEAKING_TIMES 5
static int peakingTimes[] = {5,10,15,20,25};

/* Debug support */
#if 0
#define Debug(l,FMT,V) ;
#else
#define Debug(l,FMT,V...) {if (l <= dxpRecordDebug) \
                          { printf("%s(%d):",__FILE__,__LINE__); \
                            printf(FMT,##V);}}
#endif
volatile int dxpRecordDebug = 0;

typedef struct {
   unsigned short runtasks;
   unsigned short decimation;
   unsigned short slowlen;
   unsigned short slowgap;
   unsigned short peaksamp;
   unsigned short peakint;
} PARAM_OFFSETS;

typedef struct {
   unsigned short *access;
   unsigned short *lbound;
   unsigned short *ubound;
   unsigned short nsymbols;
   double clock;
   unsigned int nbase;
   PARAM_OFFSETS offsets;
} MODULE_INFO;

static MODULE_INFO moduleInfo[MAX_MODULE_TYPES];



static long init_record(struct dxpRecord *pdxp, int pass)
{
    struct dxpDSET *pdset;
    DXP_SHORT_PARAM *short_param;
    DXP_LONG_PARAM *long_param;
    int i;
    int module;
    MODULE_INFO *minfo;
    unsigned short offset;
    int status;
    char boardString[MAXBOARDNAME_LEN];


    if (pass != 0) return(0);
    Debug(5, "(init_record): entry\n");

    /* pdxp->inp must be VME_IO */
    if (pdxp->inp.type != VME_IO) {
        recGblRecordError(S_db_badField,(void *)pdxp,
                "dxpRecord (init_record) Illegal INP field");
        return(S_db_badField);
    }
    module = pdxp->inp.value.vmeio.signal;

    /* Initialize values for each type of module */
    moduleInfo[DXP4C].clock = .050;
    moduleInfo[DXP2X].clock = .025;

    /* Figure out what kind of module this is */
    dxp_get_board_type(&module, boardString);
    if (strcmp(boardString, "dxp4c") == 0) pdxp->mtyp = MODEL_DXP4C;
    else if (strcmp(boardString, "dxp4c2x") == 0) pdxp->mtyp = MODEL_DXP2X;
    else Debug(1, "(init_record), unknown board type\n");
    minfo = &moduleInfo[pdxp->mtyp];

    /* must have dset defined */
    if (!(pdset = (struct dxpDSET *)(pdxp->dset))) {
        recGblRecordError(S_dev_noDSET,(void *)pdxp,"mca: init_record1");
        return(S_dev_noDSET);
    }
    /* must have read_array function defined */
    if ( (pdset->number < 6) || (pdset->read_array == NULL) ) {
        recGblRecordError(S_dev_missingSup,(void *)pdxp,"dxp: init_record2");
        printf("%ld %p\n",pdset->number, pdset->read_array);
        return(S_dev_missingSup);
    }
    if (pdset->init_record) {
        if ((status=(*pdset->init_record)(pdxp))) return(status);
    }

    /* If minfo->nsymbols=0 then this is the first DXP record of this 
     * module type, allocate global (not record instance) structures */
    if (minfo->nsymbols == 0) {
        dxp_max_symbols(&module, &minfo->nsymbols);
        Debug(5, "(init_record): allocating memory, nsymbols=%d\n", minfo->nsymbols);
        dxp_nbase(&module, &minfo->nbase);
        minfo->access =
               (unsigned short *) malloc(minfo->nsymbols * 
                                         sizeof(unsigned short));
        minfo->lbound =
               (unsigned short *) malloc(minfo->nsymbols * 
                                         sizeof(unsigned short));
        minfo->ubound =
               (unsigned short *) malloc(minfo->nsymbols * 
                                         sizeof(unsigned short));
        dxp_symbolname_limits(&module, minfo->access, minfo->lbound, 
                               minfo->ubound);
    }

    /* Look up the address of each parameter */
    /* Address of first short parameter */
    short_param = (DXP_SHORT_PARAM *)&pdxp->a01v;
    for (i=0; i<NUM_SHORT_READ_PARAMS; i++) {
       if (strcmp(short_param[i].label, "Unused") == 0)
          offset=-1;
       else
          dxp_get_symbol_index(&module, short_param[i].label, &offset);
       short_param[i].offset = offset;
    }
    /* Address of first long parameter */
    long_param = (DXP_LONG_PARAM *)&pdxp->s01v;
    for (i=0; i<NUM_DXP_LONG_PARAMS; i++) {
       if (strcmp(long_param[i].label, "Unused") == 0)
          offset=-1;
       else
          dxp_get_symbol_index(&module, long_param[i].label, &offset);
       long_param[i].offset = offset;
    }

    /* Get the offsets of specific parameters that the record will use. */
    dxp_get_symbol_index(&module, "RUNTASKS",   &minfo->offsets.runtasks);
    dxp_get_symbol_index(&module, "DECIMATION", &minfo->offsets.decimation);
    dxp_get_symbol_index(&module, "SLOWLEN",    &minfo->offsets.slowlen);
    dxp_get_symbol_index(&module, "SLOWGAP",    &minfo->offsets.slowgap);
    dxp_get_symbol_index(&module, "PEAKINT",    &minfo->offsets.peakint);
    /* For some reason the DXP2X uses PEAKSAM rather than PEAKSAMP */
    switch (pdxp->mtyp) {
       case DXP4C:
          dxp_get_symbol_index(&module, "PEAKSAMP",   &minfo->offsets.peaksamp);
          break;
       case DXP2X:
          dxp_get_symbol_index(&module, "PEAKSAM",   &minfo->offsets.peaksamp);
          break;
    }

    /* Allocate the space for the baseline array */
    pdxp->bptr = (unsigned short *)calloc(minfo->nbase, sizeof(short));

    /* Allocate the space for the parameter array */
    pdxp->pptr = (unsigned short *)calloc(minfo->nsymbols, 
                                          sizeof(unsigned short));

    /* Allocate space for the peaking time strings */
    pdxp->pkl = (char *)calloc(DXP_STRING_SIZE * MAX_PEAKING_TIMES, 
                               sizeof(char));

    /* Note, the following 2 calls don't work yet, because the MPF server 
     * will not yet be running when init_record is run.  
     * This is a serious problem. */

    /* Initialize the tasks */
    setDxpTasks(pdxp);

    /* Call process().  This will update read-only fields */
    process(pdxp);

    Debug(5, "(init_record): exit\n");
    return(0);
}


static long cvt_dbaddr(paddr)
struct dbAddr *paddr;
{
   struct dxpRecord *pdxp=(struct dxpRecord *)paddr->precord;
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];

   if (paddr->pfield == &(pdxp->base)) {
      paddr->pfield = (void *)(pdxp->bptr);
      paddr->no_elements = minfo->nbase;
      paddr->field_type = DBF_SHORT;
      paddr->field_size = sizeof(short);
      paddr->dbr_field_type = DBF_SHORT;
      Debug(5, "(cvt_dbaddr): field=base\n");
   } else if (paddr->pfield == &(pdxp->parm)) {
         paddr->pfield = (void *)(pdxp->pptr);
         paddr->no_elements = minfo->nsymbols;
         paddr->field_type = DBF_SHORT;
         paddr->field_size = sizeof(short);
         paddr->dbr_field_type = DBF_SHORT;
         Debug(5, "(cvt_dbaddr): field=parm\n");
   } else {
      Debug(1, "(cvt_dbaddr): field=unknown\n");
   }
   return(0);
}


static long get_array_info(paddr,no_elements,offset)
struct dbAddr *paddr;
long *no_elements;
long *offset;
{
   struct dxpRecord *pdxp=(struct dxpRecord *)paddr->precord;
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];

   if (paddr->pfield == pdxp->bptr) {
      *no_elements = minfo->nbase;
      *offset = 0;
      Debug(5, "(get_array_info): field=base\n");
   } else if (paddr->pfield == pdxp->pptr) {
      *no_elements =  minfo->nsymbols;
      *offset = 0;
      Debug(5, "(get_array_info): field=parm\n");
   } else {
     Debug(1, "(get_array_info):field=unknown,paddr->pfield=%p,pdxp->bptr=%p\n",
               paddr->pfield, pdxp->bptr);
   }

   return(0);
}


static long process(pdxp)
        struct dxpRecord        *pdxp;
{
    struct dxpDSET *pdset = (struct dxpDSET *)(pdxp->dset);
    int pact=pdxp->pact;
    int status;

    Debug(5, "dxpRecord(process): entry\n");

    /*** Check existence of device support ***/
    if ( (pdset==NULL) || (pdset->read_array==NULL) ) {
        pdxp->pact=TRUE;
        recGblRecordError(S_dev_missingSup,(void *)pdxp,"read_array");
        return(S_dev_missingSup);
    }

    /* Read the parameter information from the DXP */
    status = (*pdset->read_array)(pdxp);
    /* See if device support set pact=true, meaning it will call us back */
    if (!pact && pdxp->pact) return(0);
    pdxp->pact = TRUE;
    recGblGetTimeStamp(pdxp);
    monitor(pdxp);
    recGblFwdLink(pdxp);
    pdxp->pact=FALSE;

    Debug(5, "dxpRecord(process): exit\n");
    return(0);
}


static long monitor(struct dxpRecord *pdxp)
{
   int i;
   int offset;
   DXP_SHORT_PARAM *short_param;
   DXP_LONG_PARAM *long_param;
   unsigned short short_val;
   long long_val;
   unsigned short monitor_mask = recGblResetAlarms(pdxp) | DBE_VALUE | DBE_LOG;
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];

   /* Get the value of each parameter, post monitor if it is different
    * from current value */
   /* Address of first short parameter */
   short_param = (DXP_SHORT_PARAM *)&pdxp->a01v;
   for (i=0; i<NUM_SHORT_READ_PARAMS; i++) {
      offset = short_param[i].offset;
      if (offset < 0) continue;
      short_val = pdxp->pptr[offset];
      if(short_param[i].val != short_val) {
         Debug(5, "dxpRecord: New value of short parameter %s\n",
            short_param[i].label);
         Debug(5,"  old (record)=%d\n", short_param[i].val)
         Debug(5,"  new (dxp)=%d\n",short_val);
         short_param[i].val = short_val;

         /* Check for changes to parameters we need to handle in the record */
         if (offset == minfo->offsets.decimation) {
            /* Decimation has changed, construct peaking time strings */
            double peakingTime;
            int decimation = pdxp->pptr[offset];
            for (i=0; i < MAX_PEAKING_TIMES; i++) {
               peakingTime = peakingTimes[i] * minfo->clock * (1<<decimation);
               /* Minimum peaking time on the DXP4C of .5 microsecond */
               if ((pdxp->mtyp == DXP4C) && (peakingTime < 0.5)) 
                  peakingTime = 0.5;
               sprintf(pdxp->pkl+DXP_STRING_SIZE*i, "%.2f us", peakingTime);
            }
         }
         db_post_events(pdxp,&short_param[i].val, monitor_mask);
      }
   }
   /* Address of first long parameter */
   long_param = (DXP_LONG_PARAM *)&pdxp->s01v;
   for (i=0; i<NUM_DXP_LONG_PARAMS; i++) {
      offset = long_param[i].offset;
      if (offset == -1) continue;
      long_val = (pdxp->pptr[offset]<<16) + pdxp->pptr[offset+1];
      if(long_param[i].val != long_val) {
         Debug(5,"dxpRecord: New value of long parameter %s\n",
            long_param[i].label);
         Debug(5,"  old (record)=%ld", long_param[i].val)
         Debug(5,"  new (dxp)=%ld\n", long_val);
         long_param[i].val = long_val;
         db_post_events(pdxp,&long_param[i].val, monitor_mask);
      }
   }

   /* If rcal==1 then clear it, since the calibration is done */
   if (pdxp->rcal) {
      pdxp->rcal=0;
      db_post_events(pdxp,&pdxp->rcal, monitor_mask);
   }

   return(0);
}

static long special(struct dbAddr *paddr, int after)
/* Called whenever a field is changed */
{
    struct dxpRecord *pdxp=(struct dxpRecord *)paddr->precord;
    struct dxpDSET *pdset = (struct dxpDSET *)(pdxp->dset);
    short offset;
    int i;
    int status;
    DXP_SHORT_PARAM *short_param;
    DXP_TASK_PARAM *task_param;
    int nparams;

    if (!after) return(0);  /* Don't do anything if field not yet changed */

    Debug(5, "dxpRecord(special): entry\n");

    /* Loop through seeing which field was changed.  Write the new value.*/
    /* Address of first short parameter */
    short_param = (DXP_SHORT_PARAM *)&pdxp->a01v;
    for (i=0; i<NUM_SHORT_READ_PARAMS; i++) {
       if (paddr->pfield == (void *) &short_param[i].val) {
          offset = short_param[i].offset;
          if (offset == -1) continue;
          nparams = 1;
          status = (*pdset->send_msg)
                   (pdxp,  MSG_DXP_SET_SHORT_PARAM, offset, short_param[i].val);
          Debug(5,
         "dxpRecord: special found new value of short parameter %s, value=%d\n",
                  short_param[i].label, short_param[i].val);
          goto found_param;
       }
    }

    /* The field was not a simple parameter */
     if (paddr->pfield == (void *) &pdxp->strt) {
        if (pdxp->strt != 0) {
            status = (*pdset->send_msg)
                     (pdxp, MSG_ACQUIRE, NULL, NULL);
            pdxp->strt=0;
        }
        goto found_param;
     }
     if (paddr->pfield == (void *) &pdxp->stop) {
        if (pdxp->stop != 0) {
            status = (*pdset->send_msg)
                     (pdxp, MSG_STOP_ACQUISITION, NULL, NULL);
            pdxp->stop=0;
        }
        goto found_param;
     }

     if (paddr->pfield == (void *) &pdxp->eras) {
        if (pdxp->eras != 0) {
            status = (*pdset->send_msg)
                     (pdxp, MSG_ERASE, NULL, NULL);
            pdxp->eras=0;
        }
        goto found_param;
     }

     if (paddr->pfield == (void *) &pdxp->pktm) {
        setPeakingTime(pdxp);
        goto found_param;
     }

    /* Check if the tasks have changed */
    task_param = (DXP_TASK_PARAM *)&pdxp->t01v;
    for (i=0; i<NUM_TASK_PARAMS; i++) {
       if (paddr->pfield == (void *) &task_param[i].val) {
          if (strcmp(task_param[i].label, "Unused") == 0) continue;
          setDxpTasks(pdxp);
          goto found_param;
       }
    }

    if ((paddr->pfield == (void *) &pdxp->rcal) ||
        (paddr->pfield == (void *) &pdxp->calr)) {
          setDxpTasks(pdxp);
          goto found_param;
     }

found_param:
    /* Call "process" so changes to parameters are displayed */
    process(pdxp);

    Debug(5, "dxpRecord(special): exit\n");
    return(0);
}


static void setDxpTasks(struct dxpRecord *pdxp)
{
   struct dxpDSET *pdset = (struct dxpDSET *)(pdxp->dset);
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];
   DXP_TASK_PARAM *task_param;
   int cal;
   unsigned short runtasks;
   int i;
   int status;

   Debug(5, "dxpRecord(setDxpTasks): entry\n");
   if (pdxp->rcal) {
      if (pdxp->calr == dxpCAL_TRACKRST) cal=2; else cal=1;
      status = (*pdset->send_msg) (pdxp, MSG_DXP_CALIBRATE, cal, NULL);
   } else {
      task_param = (DXP_TASK_PARAM *)&pdxp->t01v;
      runtasks = 0;
       for (i=0; i<NUM_TASK_PARAMS; i++) {
          if (strcmp(task_param[i].label, "Unused") == 0) continue;
          if (task_param[i].val) runtasks |= (1 << i);
       }
       status = (*pdset->send_msg)
         (pdxp,  MSG_DXP_SET_SHORT_PARAM, minfo->offsets.runtasks, runtasks);
   }
   Debug(5, "dxpRecord(setDxpTasks): exit\n");
}


static void setPeakingTime(struct dxpRecord *pdxp)
{
   struct dxpDSET *pdset = (struct dxpDSET *)(pdxp->dset);
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];

   /* New value of peaking time.  
    * Compute new values of SLOWLEN, PEAKSAMP, etc. */
   unsigned short slowlen=0, peaksamp=0, slowgap=0, peakint=0;
   unsigned int index=pdxp->pktm;
   int decimation = pdxp->pptr[minfo->offsets.decimation];

   /* The shortest shaping time is not allowed on DXP4C */
   if ((pdxp->mtyp == DXP4C) && (decimation == 0) && (index == 0)) index=1;
   slowlen = peakingTimes[index];
   slowgap = pdxp->pptr[minfo->offsets.slowgap];
   switch (decimation) {
      /* This algorithm is from page 38 of the DXP4C User's Manual */
      case 0:
         peaksamp = slowlen + (slowgap+1)/2 - 2;
         peakint = slowlen + slowgap;
         break;
      case 2:
         peaksamp = slowlen + (slowgap+1)/2 - 1;
         peakint = slowlen + slowgap + 1;
         break;
      case 4:
      case 6:
         peaksamp = slowlen + (slowgap+1)/2 + 1;
         peakint = slowlen + slowgap + 1;
         break;
      default:
         Debug(1, "(special) Unexpected decimation = %d\n", decimation)
   }
   (*pdset->send_msg)
            (pdxp,  MSG_DXP_SET_SHORT_PARAM, minfo->offsets.slowlen, slowlen);
   (*pdset->send_msg)
            (pdxp,  MSG_DXP_SET_SHORT_PARAM, minfo->offsets.peaksamp, peaksamp);
   (*pdset->send_msg)
            (pdxp,  MSG_DXP_SET_SHORT_PARAM, minfo->offsets.peakint, peakint);
}


static long get_graphic_double(struct dbAddr *paddr, struct dbr_grDouble *pgd)
{
   struct dxpRecord   *pdxp=(struct dxpRecord *)paddr->precord;
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];
   int fieldIndex = dbGetFieldIndex(paddr);
   int offset, i;
   DXP_SHORT_PARAM *short_param;
   DXP_LONG_PARAM *long_param;

   if ((fieldIndex < dxpRecordA01V) || (fieldIndex > dxpRecordS13V)) {
      recGblGetGraphicDouble(paddr,pgd);
      return(0);
   } else {
      short_param = (DXP_SHORT_PARAM *)&pdxp->a01v;
      for (i=0; i<NUM_SHORT_READ_PARAMS; i++) {
         if (paddr->pfield == (void *) &short_param[i].val) {
            offset = short_param[i].offset;
            if (offset < 0) continue;
            pgd->upper_disp_limit = minfo->ubound[offset];
            pgd->lower_disp_limit = minfo->lbound[offset];
            return(0);
         }
      }
      long_param = (DXP_LONG_PARAM *)&pdxp->s01v;
      for (i=0; i<NUM_DXP_LONG_PARAMS; i++) {
         if (paddr->pfield == (void *) &long_param[i].val) {
            offset = long_param[i].offset;
            if (offset < 0) continue;
            pgd->upper_disp_limit = minfo->ubound[offset];
            pgd->lower_disp_limit = minfo->lbound[offset];
            return(0);
         }
      }
   }
   return(0);
}


static long get_control_double(struct dbAddr *paddr, struct dbr_ctrlDouble *pcd)
{
   struct dxpRecord   *pdxp=(struct dxpRecord *)paddr->precord;
   int fieldIndex = dbGetFieldIndex(paddr);
   int offset, i;
   MODULE_INFO *minfo = &moduleInfo[pdxp->mtyp];
   DXP_SHORT_PARAM *short_param;
   DXP_LONG_PARAM *long_param;

   if ((fieldIndex < dxpRecordA01V) || (fieldIndex > dxpRecordS13V)) {
      recGblGetControlDouble(paddr,pcd);
      return(0);
   } else {
      short_param = (DXP_SHORT_PARAM *)&pdxp->a01v;
      for (i=0; i<NUM_SHORT_READ_PARAMS; i++) {
         if (paddr->pfield == (void *) &short_param[i].val) {
            offset = short_param[i].offset;
            if (offset < 0) continue;
            pcd->upper_ctrl_limit = minfo->ubound[offset];
            pcd->lower_ctrl_limit = minfo->lbound[offset];
            return(0);
         }
      }
      long_param = (DXP_LONG_PARAM *)&pdxp->s01v;
      for (i=0; i<NUM_DXP_LONG_PARAMS; i++) {
         if (paddr->pfield == (void *) &long_param[i].val) {
            offset = long_param[i].offset;
            if (offset < 0) continue;
            pcd->upper_ctrl_limit = minfo->ubound[offset];
            pcd->lower_ctrl_limit = minfo->lbound[offset];
            return(0);
         }
      }
   }
   return(0);
}


static long get_enum_str(struct dbAddr *paddr, char *pstring)
{
    struct dxpRecord   *pdxp=(struct dxpRecord *)paddr->precord;
    int index;
    unsigned short *pfield = (unsigned short *)paddr->pfield;
    unsigned short val=*pfield;

    index = dbGetFieldIndex(paddr);
    if (index != dxpRecordPKTM) {
        strcpy(pstring,"Illegal_Value");
    } else if (val < MAX_PEAKING_TIMES) {
       strncpy(pstring, pdxp->pkl+DXP_STRING_SIZE*val, DXP_STRING_SIZE);
    } else {
        strcpy(pstring,"Illegal Value");
    }
    return(0);
}

static long get_enum_strs(struct dbAddr *paddr, struct dbr_enumStrs *pes)
{
    struct dxpRecord *pdxp=(struct dxpRecord *)paddr->precord;
    int i;

    int index;

    index = dbGetFieldIndex(paddr);
    if (index != dxpRecordPKTM) return(S_db_badChoice);

    pes->no_str = MAX_PEAKING_TIMES;
    for (i=0; i < pes->no_str; i++) {
       strncpy(pes->strs[i], pdxp->pkl+DXP_STRING_SIZE*i, DXP_STRING_SIZE);
    }
    return(0);
}


static long put_enum_str(struct dbAddr *paddr, char *pstring)
{
    struct dxpRecord *pdxp=(struct dxpRecord *)paddr->precord;
    short i;
    int index;

    index = dbGetFieldIndex(paddr);
    if (index != dxpRecordPKTM) return(S_db_badChoice);
    for (i=0; i < MAX_PEAKING_TIMES; i++){
       if(strncmp(pdxp->pkl+DXP_STRING_SIZE*i,pstring,DXP_STRING_SIZE)==0) {
          pdxp->pktm = i;
          return(0);
       }
    }
    return(S_db_badChoice);
}