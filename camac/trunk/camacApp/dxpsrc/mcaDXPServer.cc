//mcaDXPServer.cc

/*
    Author: Mark Rivers
    Date: 16-Jun-2001  Modified from mcaAIMServer.cc

    This module is an MPF server which receives messages from devMcaMpf.cc and
    devDxpMpf.cc and commmunicates with an XIA DXP module.  The code is written
    as an MPF server for 3 reasons:
     - Some calls to the XIA Xerxes software are relatively slow and so require
       that device support be asynchronous
     - If more than one task tries to talk to a single DXP4C module through the
       Xerxes software at the same time bad things happen.  By using one MPF
       message server per module we guarantee that access to the module is
       serialized, there will only be one task communicating with each module.
     - The server can perform optimizations, like stopping the module, reading
       and caching the information from each channel, and restarting the
       module.  This can be much more efficient than stopping the module 4
       times, once for each channel.
  
    Modifications:
    10-Feb-2002  MLR  Eliminated call to readoutRun(1) after writing a parameter,
                      rather just set forceRead so next time readoutRun is
                      called it will not use cached values.
    13-Feb-2002  MLR  Fixed bug in setting preset live time or real time.
*/

#include <vxWorks.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <taskLib.h>
#include <sysLib.h>
#include <tickLib.h>
#include <semLib.h>
#include <time.h>

#include <errlog.h>

#include "Message.h"
#include "Int32Message.h"
#include "Float64Message.h"
#include "Int32ArrayMessage.h"
#include "Float64ArrayMessage.h"
#include "mpfType.h"
#include "mca.h"
#include "DevMcaMpf.h"
#include "dxpServer.h"
#include "xerxes_structures.h"
#include "xerxes.h"


extern "C"
{
#ifdef NODEBUG
#define DEBUG(l,f,v) ;
#else
#define DEBUG(l,f,v...) { if(l<=mcaDXPServerDebug) errlogPrintf(f,## v); }
#endif
volatile int mcaDXPServerDebug = 0;
}

#define MAX_CHANS_PER_DXP 4
#define STOP_DELAY 0.01   // Number of seconds to wait after issuing stop 
                          // before reading
#define DXP4C_CACHE_TIME .2     // Maximum time to use cached values
#define DXP2X_CACHE_TIME .1

static int numDXP4C=0;
static SEM_ID dxpSEMID=NULL;

typedef struct {
    int exists;
    int detChan;
    double preal;
    double ereal;
    double plive;
    double elive;
    double ptotal;
    int ptschan;
    int ptechan;
    double etotal;
    unsigned short *params;
    unsigned short *baseline;
    unsigned long *counts;
} dxpChannel;

class mcaDXPServer {
public:
    void processMessages();
    void pauseRun();
    void stopRun();
    void resumeRun(unsigned short resume);
    void readoutRun();
    void setPreset(struct dxpChannel *dxpChan, int mode, double time);
    static void mcaDXPServerTask(mcaDXPServer *);
    void getAcquisitionStatus(struct dxpChannel *dxpChan, 
                              struct devMcaMpfStatus *pstat);
    double getCurrentTime();
    void dxpInterlock(int state);
    int detChan;
    MessageServer *pMessageServer;
    int moduleType;
    int stopDelayTicks;
    unsigned int statusTime;
    unsigned int maxStatusTime;
    int forceRead;
    struct dxpChannel *dxpChannel;
    int acquiring;
    int erased;
    double ereal;
    double start_time;
    double clockRate;
    unsigned short gate;
    unsigned short livetime_index;
    unsigned short realtime_index;
    unsigned short preset_index;
    unsigned short preset_mode_index;
    unsigned short busy_index;
    unsigned short mcalimhi_index;
    unsigned short nsymbols;
    unsigned int nbase;
    unsigned int nchans;
};


extern "C" int DXPConfig(const char *serverName, int chan1, int chan2, 
                         int chan3, int chan4, int queueSize)
{
    /* chan1 - chan4 are the Xerxes channel numbers for the 4 inputs of this
     * module.  If an input is unused or does not exists set chanN=-1
     * moduleType is 0 for DXP4C and 1 for DXP2X. */
    char taskname[80];
    int allChans[MAX_CHANS_PER_DXP];
    char boardString[MAXBOARDNAME_LEN];
    int i;
    struct dxpChannel *dxpChannel;
    int detChan;

    mcaDXPServer *p = new mcaDXPServer;
    p->pMessageServer = new MessageServer(serverName, queueSize);

    /* Copy parameters to object private */
    allChans[0] = chan1;
    allChans[1] = chan2;
    allChans[2] = chan3;
    allChans[3] = chan4;
    for (i=0; i<MAX_CHANS_PER_DXP; i++) {
       if (allChans[i] >= 0) {
          dxp_get_board_type(&allChans[i], boardString);
          if (strcmp(boardString, "dxp4c") == 0) p->moduleType = MODEL_DXP4C;
          else if (strcmp(boardString, "dxp4c2x") == 0) p->moduleType = MODEL_DXP2X;
          else DEBUG(1, "dxpConfig: unknown board type\n");
          break;
       }
    }

    // Keep track if there are any DXP4C modules
    if (p->moduleType == MODEL_DXP4C) numDXP4C++;
    if (numDXP4C == 1) {
       // First DXP4C, create semaphore
       dxpSEMID = semBCreate (      /* Create semaphore    */
                      SEM_Q_FIFO,   /* Queue in FIFO order */
                      SEM_FULL);	   /* Initially full		  */
    }
    
    if (p->moduleType == MODEL_DXP4C) {
    	/* DXP4C is 800ns clock period */
    	p->clockRate = 1./ 800.e-9;
    	/* Maximum time to use old data before a new query is sent. */
    	p->maxStatusTime = (int) (DXP4C_CACHE_TIME * sysClkRateGet());
    }
    else if (p->moduleType == MODEL_DXP2X) {
    	/* DXP2X is 400ns clock period */
    	p->clockRate = 1./ 400.e-9;
    	/* Maximum time to use old data before a new query is sent. */
    	p->maxStatusTime = (int) (DXP2X_CACHE_TIME * sysClkRateGet());
    }
    p->stopDelayTicks = (int) STOP_DELAY*sysClkRateGet();
    /* Minimum of 2 clocks ticks, since 1 tick can happen "instantly" */
    if (p->stopDelayTicks < 2) p->stopDelayTicks = 2;
    p->acquiring = 0;
    p->ereal = 0.;
    p->erased = 1;
    p->gate   = 1;
    p->dxpChannel = (struct dxpChannel *)
                calloc(MAX_CHANS_PER_DXP, sizeof(struct dxpChannel));
    /* Copy Xerxes channel numbers to structures */
    for (i=0; i<MAX_CHANS_PER_DXP; i++) p->dxpChannel[i].exists = 0;
    /* Allocate memory arrays for each channel if it is valid */
    for (i=0; i<MAX_CHANS_PER_DXP; i++) {
       dxpChannel = &p->dxpChannel[i];
       if (allChans[i] >= 0) {
          detChan = allChans[i];
          dxpChannel->detChan = detChan;
          dxpChannel->exists = 1;
          dxp_max_symbols(&detChan, &p->nsymbols);
          dxp_nbase(&detChan, &p->nbase);
          dxp_nspec(&detChan, &p->nchans);
          dxp_get_symbol_index(&detChan, "LIVETIME0", &p->livetime_index);
          if (p->moduleType == MODEL_DXP2X) {
             dxp_get_symbol_index(&detChan, "REALTIME0", &p->realtime_index);
             dxp_get_symbol_index(&detChan, "PRESETLEN0", &p->preset_index);
             dxp_get_symbol_index(&detChan, "PRESET", &p->preset_mode_index);
             dxp_get_symbol_index(&detChan, "BUSY", &p->busy_index);
             dxp_get_symbol_index(&detChan, "MCALIMHI", &p->mcalimhi_index);
          }             
          dxpChannel->params = (unsigned short *) calloc(p->nsymbols, 
                                      sizeof(unsigned short));
          dxpChannel->baseline = (unsigned short *) calloc(p->nbase, 
                                      sizeof(short));
          dxpChannel->counts = (unsigned long *) calloc(p->nchans, 
                                       sizeof(long));
          /* Set reasonable defaults for other parameters */
          dxpChannel->plive   = 0.;
          dxpChannel->ptotal  = 0.;
          dxpChannel->ptschan = 0;
          dxpChannel->ptechan = 0;
       }
    }
    p->statusTime = 0;
    p->forceRead = 1;  // Don't use cache first time

    strcpy(taskname, "t");
    strcat(taskname, serverName);
    int taskId = taskSpawn(taskname,100,VX_FP_TASK,4000,
        (FUNCPTR)mcaDXPServer::mcaDXPServerTask,(int)p,
        0,0,0,0,0,0,0,0,0);
    if(taskId==ERROR)
        errlogPrintf("%s mcaDXPServer taskSpawn Failure\n",
            p->pMessageServer->getName());
    return(0);
}


void mcaDXPServer::mcaDXPServerTask(mcaDXPServer *pmcaDXPServer)
{
   pmcaDXPServer->processMessages();
}

void mcaDXPServer::processMessages()
{
    while(true) {
        pMessageServer->waitForMessage();
        Message *inmsg;
        int cmd;
        int len;
        unsigned short resume;
        int s;
        int i;
        double time_now;
        int nparams;
        unsigned short offset;
        unsigned short short_value;
        int int_value;

        while((inmsg = pMessageServer->receive())) {
            Float64Message *pim = (Float64Message *)inmsg;
            Float64ArrayMessage *pf64m = NULL;
            Int32ArrayMessage *pi32m = NULL;
            struct dxpChannel *dxpChan = NULL;
            struct devMcaMpfStatus *pstat;
            pim->status = 0;
            cmd = pim->cmd;
            detChan = pim->address;
            DEBUG(3,
              "(mcaDXPServer [%s detChan=%d]): Got Message, cmd=%d, value=%f\n",
                           pMessageServer->getName(), detChan, cmd, pim->value)
            /* Find which channel on this module this detChan is */
            for (i=0; i<MAX_CHANS_PER_DXP; i++) {
               if (dxpChannel[i].detChan == detChan) dxpChan = &dxpChannel[i];
            }
            if (dxpChan == NULL) {
               DEBUG(1,
               "(mcaDXPServer [%s detChan=%d]): not a valid channel\n",
                           pMessageServer->getName(), detChan)
               pMessageServer->reply(pim);
               continue;
            }

            switch (cmd) {
               case MSG_ACQUIRE:
                  /* Start acquisition. 
                   * Don't do anything if already acquiring */
                  if (!acquiring) {
                     start_time = getCurrentTime();
                     if (erased) resume=0; else resume=1;
                     /* The DXP2X could be "acquiring" even though the
                      * flag is clear, because of internal presets.  
                      * Stop run. */
                     stopRun();
                     s = dxp_start_one_run(&detChan, &gate, &resume);
                     acquiring = 1;
                     erased = 0;
                     DEBUG(2,
                       "(mcaDXPServer [%s detChan=%d]): start acq. status=%d\n",
                       pMessageServer->getName(), detChan, s);
                  }
                  break;
               case MSG_READ:
                  pi32m = (Int32ArrayMessage *)pMessageServer->
                           allocReplyMessage(pim, messageTypeInt32Array);
                  pi32m->allocValue(nchans);
                  pi32m->setLength(nchans);
                  if (erased) {
                     memset(pi32m->value, 0, nchans*sizeof(unsigned long));
                  } else {
                     readoutRun();
                     memcpy(pi32m->value, dxpChan->counts, nchans*sizeof(long));
                  }
                  delete pim;
                  pMessageServer->reply(pi32m);
                  break;
               case MSG_SET_CHAS_INT:
                  /* set channel advance source to internal (timed) */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_CHAS_EXT:
                  /* set channel advance source to external */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_NCHAN:
                  /* set number of channels
                   * On DXP4C nothing to do, always 1024 channels.
                   * On DXP2X set MCALIMHI */
               	  if (moduleType == MODEL_DXP2X) {
               	     nparams = 1;
               	     nchans = (unsigned int)pim->value;
               	     short_value = nchans;
      		     dxp_download_one_params(&detChan, &nparams, 
      		     	&mcalimhi_index, &short_value);
                  }
                  break;
               case MSG_SET_DWELL:
                  /* set dwell time */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_REAL_TIME:
                  /* set preset real time. */
                  setPreset(dxpChan, MSG_SET_REAL_TIME, pim->value);
                  break;
               case MSG_SET_LIVE_TIME:
                  /* set preset live time. */
                  setPreset(dxpChan, MSG_SET_LIVE_TIME, pim->value);
                  break;
               case MSG_SET_COUNTS:
                  /* set preset counts */
                  dxpChan->ptotal = pim->value;
                  break;
               case MSG_SET_LO_CHAN:
                  /* set lower side of region integrated for preset counts */
                  dxpChan->ptschan = (int) pim->value;
                  break;
               case MSG_SET_HI_CHAN:
                  /* set high side of region integrated for preset counts */
                  dxpChan->ptechan = (int) pim->value;
                  break;
               case MSG_SET_NSWEEPS:
                  /* set number of sweeps (for MCS mode) */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_MODE_PHA:
                  /* set mode to Pulse Height Analysis */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_MODE_MCS:
                  /* set mode to MultiChannel Scaler */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_MODE_LIST:
                  /* set mode to LIST (record each incoming event) */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_GET_ACQ_STATUS:
                  pf64m = (Float64ArrayMessage *)pMessageServer->
                            allocReplyMessage(pim, messageTypeFloat64Array);
                  len = sizeof(struct devMcaMpfStatus) / sizeof(double);
                  pf64m->allocValue(len);
                  pf64m->setLength(len);
                  pstat = (struct devMcaMpfStatus *)pf64m->value;
                  getAcquisitionStatus(dxpChan, pstat);
                  delete pim;
                  pMessageServer->reply(pf64m);
                  break;
               case MSG_STOP_ACQUISITION:
                  /* stop data acquisition */
                  stopRun();
                  if (acquiring) {
                    time_now = getCurrentTime();
                    ereal += start_time - time_now;
                    start_time = time_now;
                    acquiring = 0;
                  }
                  DEBUG(2, "(mcaDXPServer [%s detChan=%d]): stop\n",
                           pMessageServer->getName(), detChan);
                  break;
               case MSG_ERASE:
                  ereal = 0.;
                  dxpChan->elive = 0.;
                  dxpChan->etotal = 0.;
                  /* If DXP is acquiring, turn it off and back on and don't 
                   * set the erased flag */
                  if (acquiring) {
                     stopRun();
                     resume = 0;
                     s = dxp_start_one_run(&detChan, &gate, &resume);
                  } else {
                     erased = 1;
                  }
                  break;
               case MSG_SET_SEQ:
                  /* set sequence number */
                  /* This is a NOOP for DXP */
                  break;
               case MSG_SET_PSCL:
                  /* This is a NOOP for DXP */
                  break;
               case MSG_DXP_READ_PARAMS:
                  /* Read parameter memory and baseline memory */
                  pi32m = (Int32ArrayMessage *)pMessageServer->
                           allocReplyMessage(pim, messageTypeInt32Array);
                  len = (nsymbols + nbase)/2;
                  pi32m->allocValue(len);
                  pi32m->setLength(len);
                  readoutRun();
                  memcpy(pi32m->value, dxpChan->params, 
                         nsymbols*sizeof(unsigned short));
                  memcpy((unsigned short *)pi32m->value + nsymbols, 
                         dxpChan->baseline, 
                         nbase*sizeof(unsigned short));
                  delete pim;
                  pMessageServer->reply(pi32m);
                  break;
               case MSG_DXP_SET_SHORT_PARAM:
                  /* Set a short parameter */
                  pauseRun();
                  offset = (unsigned short)pim->value;
                  short_value = pim->extra;
                  nparams = 1;
                  dxp_download_one_params(&detChan, &nparams, &offset, 
                                          &short_value);
                  resumeRun(1);
                  forceRead = 1; // Force a read of new parameters next time
                  break;
               case MSG_DXP_CALIBRATE:
                  /* Calibrate */
                  int_value = (int)pim->value;
                  dxp_calibrate_one_channel(&detChan, &int_value);
                  break;
               default:
                  errlogPrintf("%s mcaDXPServer got illegal command %d\n",
                     pMessageServer->getName(), pim->cmd);
                  break;
            }
            // If we have not already sent reply, do it now.
            if (cmd != MSG_READ && 
                cmd != MSG_GET_ACQ_STATUS && 
                cmd != MSG_DXP_READ_PARAMS)
               pMessageServer->reply(pim);
        }
    }
}


void mcaDXPServer::pauseRun()
{
   /* Pause the run if the module is acquiring. Wait for a short interval for 
    * DXP4C to finish processing */
   /* Don't do anything on DXP-2X, since this function is only needed
    * to read out module when acquiring, which is not a problem on DXP2X. */
   if (moduleType == MODEL_DXP2X) return;
   if (acquiring) dxp_stop_one_run(&detChan);
   taskDelay(stopDelayTicks);
}

void mcaDXPServer::stopRun()
{
   /* Stop the run.  Wait for a short interval to finish processing */
   dxp_stop_one_run(&detChan);
   /* On both the DXP4C and DXP2X wait a short time, so other commands won't
    * come in before module is done processing */
   /* Should check Busy parameter on DXP2X. */
   taskDelay(stopDelayTicks);
}

void mcaDXPServer::resumeRun(unsigned short resume)
{
   /* Resume the run.  resume=0 to erase, 1 to not erase. */
   /* Don't do anything on DXP-2X, since this function is only needed to resume
    * a run after pauseRun, used to read out module when acquiring */
   if (moduleType == MODEL_DXP2X) return;
   if (acquiring) dxp_start_one_run(&detChan, &gate, &resume);
}

void mcaDXPServer::dxpInterlock(int state)
{
   /* This function takes (state=1) or releases (state=0) a semaphore blocking
    * access to all DXP modules.  Its purpose is to increase the performance of
    * readoutRUN() whenever there are any DXP4C modules in the system.  Without this
    * interlock all of the mcDXPServer tasks are accessing the crate "simultaneously".
    * This is not a resource conflict at the hardware level, because the CAMAC driver
    * handles that.  However, it is a performance problem, because readoutRun stops
    * acquisition on a DXP4C (with pauseRun) and then reads out all 4 channels.  The
    * module is "dead" during this time, not acquiring.  We want to minimize this
    * time.  If other DXPs are being accessed during this time, they are increasing
    * the dead time for that module.  The solution is to prevent other DXP server tasks
    * from reading their modules while this module is disabled.  The semaphore does
    * this.  This semaphore is not needed if there are no DXP4C modules in the system
    * (i.e. all modules are DXP2X) because the DXP2X can be read out while it is
    * acquiring and pauseRun() does nothing on the DPX4 DXP2X. */
   if (numDXP4C == 0) return;
   if (state) semTake(dxpSEMID, WAIT_FOREVER);
   else       semGive(dxpSEMID);
}

void mcaDXPServer::readoutRun()
{
   struct dxpChannel *dxpChan;
   int i, s, tstart;

   /* Readout the run if it has not been done "recently */
   if (!forceRead && ((tickGet() - statusTime) < maxStatusTime)) return;
   dxpInterlock(1);
   pauseRun();
   tstart = tickGet();
   for (i=0; i<MAX_CHANS_PER_DXP; i++) {
      dxpChan = &dxpChannel[i];
      if (dxpChan->exists) {
         s=dxp_readout_detector_run(&dxpChan->detChan, dxpChan->params, 
                                    dxpChan->baseline, dxpChan->counts);
         DEBUG(5,
            "(mcaDXPServer [%s detChan=%d]): dxp_readout_detector_run=%d\n",
            pMessageServer->getName(), dxpChan->detChan, s);
      }
   }
   resumeRun(1);
   statusTime = tickGet();
   forceRead = 0;
   DEBUG(1, "(mcaDXPServer::readoutRun for det. %d, time=%d\n", 
             detChan, statusTime-tstart);
   dxpInterlock(0);
}

double mcaDXPServer::getCurrentTime()
{
    struct timespec timespec;
    clock_gettime(CLOCK_REALTIME, &timespec);
    return (timespec.tv_sec + timespec.tv_nsec*1.e-9);
}


void mcaDXPServer::getAcquisitionStatus(struct dxpChannel *dxpChan, 
                                        struct devMcaMpfStatus *pstat)
{
   int i;
   double time_now;

   if (erased) {
      dxpChannel->elive = 0.;
      dxpChannel->ereal = 0.;
      dxpChan->etotal = 0.;
      ereal = 0.;
   } else {
      readoutRun();
      dxpChan->etotal =  0.;
      switch(moduleType) {
         case MODEL_DXP4C:
            if (acquiring) {
               time_now = getCurrentTime();
               ereal += time_now - start_time;
               dxpChan->ereal = ereal;
               start_time = time_now;
            }
            /* Compute livetime from the parameter array */
            dxpChannel->elive = 
               ((dxpChan->params[livetime_index]<<16) +
               dxpChan->params[livetime_index+1]) /
               clockRate;
            break;
         case MODEL_DXP2X:
            /* Compute livetime from the parameter array. The DXP2X
             * uses 3 words to store it.*/
            dxpChannel->elive = 
               ((dxpChan->params[livetime_index+2] * 4294967296.) +
               (dxpChan->params[livetime_index] * 65536.) +
               (dxpChan->params[livetime_index+1])) /
               clockRate;
            /* Compute realtime from the parameter array. The DXP2X
             * uses 3 words to store it.*/
            dxpChannel->ereal = 
               ((dxpChan->params[realtime_index+2] * 4294967296.) +
               (dxpChan->params[realtime_index] * 65536.) +
               (dxpChan->params[realtime_index+1])) /
               clockRate;
            break;
      }
      /* Compute counts in preset count window */
      if ((dxpChan->ptschan > 0) &&
          (dxpChan->ptechan > 0) &&
          (dxpChan->ptechan > dxpChan->ptschan)) {
         for (i=dxpChan->ptschan; i<=dxpChan->ptechan; i++) {
              dxpChan->etotal += dxpChan->counts[i];
         }
      }
      /* See if any of the presets have been exceeded.  The DXP2X handles
       * preset live and real time internally, the DXP4C does not */
      
      switch (moduleType) {
      case MODEL_DXP4C:
         if (((dxpChannel->preal != 0.0) && 
              (ereal > dxpChannel->preal)) ||
             ((dxpChannel->plive != 0.0) &&
              (dxpChannel->elive > dxpChannel->plive)) ||
             ((dxpChan->ptotal != 0.0) && 
              (dxpChan->etotal > dxpChan->ptotal))) {
            stopRun();
            acquiring = 0;
         }
         break;
      case MODEL_DXP2X:
         if ((dxpChan->params[busy_index] == 0) || 
             (dxpChan->params[busy_index] == 99)) {
            acquiring = 0;
         } else {
            acquiring = 1;
         }
         if ((dxpChan->ptotal != 0.0) && 
             (dxpChan->etotal > dxpChan->ptotal)) {
            stopRun();
            acquiring = 0;
         }
         break;
      }
   }
   pstat->realTime = dxpChannel->ereal;
   pstat->liveTime = dxpChannel->elive;
   pstat->dwellTime = 0.;
   pstat->acquiring = acquiring;
   pstat->totalCounts = dxpChan->etotal;
   DEBUG(2, "(mcaDXPServer [%s detChan=%d]): acquiring=%d\n",
      pMessageServer->getName(), detChan, acquiring);
}



void mcaDXPServer::setPreset(struct dxpChannel *dxpChan, 
                             int mode, double time)
{
   int nparams=3;
   unsigned short offsets[3];
   unsigned short values[3];
   unsigned short time_mode=0;
   
   switch (mode) {
      case MSG_SET_REAL_TIME:
         dxpChan->preal = time;
         break;
      case MSG_SET_LIVE_TIME:
         dxpChan->plive = time;
         break;
   }
   
   if (moduleType == MODEL_DXP2X) {
      /* If preset live and real time are both zero set to count forever */
      if ((dxpChan->plive == 0.) && (dxpChan->preal == 0.)) time_mode = 0;
      /* If preset live time is zero and real time is non-zero use real time */
      if ((dxpChan->plive == 0.) && (dxpChan->preal != 0.)) {
         time = dxpChan->preal;
         time_mode = 1;
      }
      /* If preset live time is non-zero use live time */
      if (dxpChan->plive != 0.) {
         time = dxpChan->plive;
         time_mode = 2;
      }
      values[0] = ((int)(time*clockRate)) >> 16;
      offsets[0] = preset_index;
      values[1] = ((int)(time*clockRate)) & 0xffff;
      offsets[1] = preset_index+1;
      values[2] = time_mode;
      offsets[2] = preset_mode_index;
      dxp_download_one_params(&detChan, &nparams, offsets, values);
   }
}
