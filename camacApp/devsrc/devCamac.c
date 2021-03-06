/******************************************************************************
 * devCamac.c -- Device Support Routines for Generic CAMAC Modules
 *
 *----------------------------------------------------------------------------
 * Author:	Eric Bjorklund & Rozelle Wright (LANL)
 *		Adapted from the work of Dave Barker & Johnny Tang (CEBAF)
 *
 * Date:	10/26/94
 *
 *----------------------------------------------------------------------------
 * MODIFICATION LOG:
 *
 * 10/26/94	Bjo	Original release
 * 08/16/95	Bjo	Added external device support for longin and longout
 * 3/12/96	MLR	Added support for waveform record
 *
 *----------------------------------------------------------------------------
 * MODULE DESCRIPTION:
 *
 * Generic CAMAC device support for EPICS.  Record types supported are:
 *  o Analog input and output (ai, ao)
 *  o Binary input and output (bi, bo)
 *  o Longword input and output (longin, longout)
 *  o Multi-bit binary input and output (mbbi, mbbo)
 *  o Waveform (wf)
 *
 * The generic CAMAC device support routines can be used for cards that
 * only require a single BCNAF sequence to read or command (but see allowances
 * for bo and mbbo records below) and that do not require LAMs or any special
 * conversion.
 *
 * The routines in this module are all globally accessible so that they may
 * be invoked if desired by device-support routines for more specialized
 * CAMAC cards.  See the header file, devCamac.h, for further information.
 *
 *----------------------------------------------------------------------------
 * RECORD FIELDS OF INTEREST:
 *
 * The following fields should be set during database configuration:
 *	DTYP	= Camac
 *	INP/OUT	= B -> Branch number of card
 *		  C -> Crate number of card
 *		  N -> Slot number of card
 *		  A -> Subaddress to access
 *		  F -> CAMAC Function code
 *		  PARM -> Other information, e.g. range values, bit number,
 *		          readback function code, etc.
 *
 * The contents and format of the PARM field depend on the record type as
 * follows:
 *
 *---------------------
 * Analog and Longword Input (AI, LI) --
 *
 *   F    = Function code to read raw value
 *
 *   PARM = Expected range of input value.
 *	For unipolar devices, the PARM field is a single number representing
 *	how many bits of precision are contained in the incomming word.  This
 *	number may be in either decimal or hexidecimal notation.  For example,
 *	the PARM field for a 12-bit unipolar card may be specified as either
 *	"4095" or "0xfff".  A 16-bit value may be either "0xffff" or "65535".
 *
 *	For bipolar devices, the PARM field has two values representing the
 *	positive and negative limits of the input value.  Alternatively, one
 *	of the PARM values may be the word "Bipolar", indicating that the
 *	second range parameter is the negative of the first.  Twos-compliment
 *	format is assumed for bipolar devices.
 *
 *	The PARM values may be separated by a commas, colons, semi-colons, or
 *	white space.  Values may be specified in any order.  The following
 *	examples are all valid ways to specify a 12-bit bipolar input:
 *		-2048,2047
 *		0x7ff -0x7ff
 *		2047:bipolar
 *		Bipolar,-2048
 *
 *---------------------
 * Analog and Longword Output (AO, LO) --
 *
 *   F    = Function code to write raw value
 *
 *   PARM = Expected range of output value, plus (optional) readback function
 *	code.  The range parameters are the same as for analog or longword
 *	input records.  If the card has a readback function code (and the
 *	readback uses the same subaddress as the write), it should be specified
 *	in the PARM field as "Rn", where "n" is the readback function code.
 *
 *	Specifying a readback function code will enable the device-support
 *	software to read the current state of the hardware at record
 *	initialization time and store it in the RVAL field (VAL field for
 *	longword records).  This aids in performing "bumpless" reboots of the
 *	IOC.  In addition, for AO records, the readback is issued after every
 *	write and stored in the RBV field.
 *
 *	Following are examples of valid PARM fields for AO and LO records:
 *		0xffffff:R0		(24-bit, unipolar, readback is F0)
 *		R1:Bipolar:2047		(12-bit, bipolar, readback is F1)
 *		0xffff bipolar		(16-bit, unipolar, no readback)
 *
 *---------------------
 * Binary and Multi-Bit Binary Input (BI, MBBI) --
 *
 *   F    = Function code to read raw value.  Note that if the function code
 *	    does not use the R/W dataway lines (e.g. F27), device-support will
 *	    return the Q response.
 *
 *   PARM = Bit number of desired bit to read (BI) or bit number of low-order
 *	    bit to read (MBBI).  Bit numbering is from 0 to 23.
 *
 *---------------------
 * Binary and Multi-Bit Binary Output (BO, MBBO) --
 *
 *   F = Function code to either read, write, set, or clear output value.
 *	Device support has two modes for writing binary values.  If the
 *	function code is a group one or two read or write, the desired value
 *	is written using a read/modify/write sequence.  The CAMAC branch
 *	is locked during this sequence so that it is atomic.  If the function
 *	code is a group one or two set or clear, the desired value is set
 *	using a single selective set or clear operation (BO only).
 *
 *	Note that multi-bit selective set/clear operations are implemented
 *	with two commands:
 *	 1) clear all the uncleared 0 bits in the new state
 *	 2) set all the unset 1 bits in the new state
 *	These two commands are executed with the CAMAC branch locked so
 *	that they are atomic to the software.  However, they will not be
 *	atomic to the hardware.  An intermediate state (after the clear
 *	command) is passed through before the final desired state is reached.
 *	If this is a problem for your hardware, you should use the
 *	read/modify/write sequence instead.
 *
 *   PARM = Bit number of desired bit to write (BO) or bit number of low-order
 *	bit to write (MBBO) plus optional readback function code.  As with
 *	BI and MBBI records, the bit numbering is from 0 to 23.  The readback
 *	function code has the format "Rn" where "n" is the function code.
 *
 *	Specifying a readback function code will enable the device-support
 *	software to read the current state of the hardware at record
 *	initialization time and store it in the RVAL field. This aids in
 *	performing "bumpless" reboots of the IOC.  The readback function is
 *	also issued after every write and the result is stored in the RBV
 *	field.
 *
 *	If the F parameter in a BO or MBBO record specifies a group 1 or group
 *	2 read or write, then the read (or the corresponding read) function
 *	will automatically be used for the readback, without having to specify
 *	it in the PARM field.
 *	
 *---------------------
 * Waveform (WF) --
 *
 *   F    = Function code to read or write raw value
 *	    Althought the waveform record is normally used to read arrays, the
 *	    CAMAC device support for the waveform record also allows writing
 *	    data if the F value is for a CAMAC write (F=16-23).
 *	    On output the values are written directly from the VAL array to the
 *	    device.  On input the values are masked for the appropriate range
 *	    and sign-extended if necessary.
 *
 *   PARM = Expected range of input values (for CAMAC read functions), and 
 *	Q transfer mode for the CAMAC block transfer operation.
 *	The range parameters are the same as for analog or longword
 *	input records.
 *	The Q transfer mode must be one of the following:
 *		QSTP 	Q-stop, i.e. continue transfers until the transfer
 *			count is reached or a Q=0 response is received.
 *		QRPT 	Q-repeat, i.e. repeat each transfer until Q=1 is
 *			response is received, or a hardware timeout occurs.
 *		QSCN 	Q-scan, i.e. increment A and N, starting from the
 *			values specifed in the B,C,N,A,F parameters, until the
 *			transfer count is reached or the last station address 
 *			is reached
 *	If no Q transfer mode is specified in the PARM field then QRPT is used.
 *
 *	Following are examples of valid PARM fields for WF records:
 *		0xffffff:QSTP		(24-bit, unipolar, Q-stop transfer mode)
 *		Bipolar:2047		(12-bit, bipolar, Q-repeat (default)
 *					 transfer mode)
 *
 * Note:  The waveform record permits any EPICS data type (DBF_CHAR,,
 *	  DBF_SHORT, DBF_DOUBLE, etc.).  However, the CAMAC device support only
 *	  allows short and long integers, both signed and unsigned. FTVL
 *	  values other than DBF_SHORT, DBF_USHORT, DBF_LONG, and DBF_ULONG will
 *	  generate an error during record initialization.
 *
 *****************************************************************************/

/************************************************************************/
/*  Header Files							*/
/************************************************************************/

#include	<stdio.h>	/* Standard I/O Routines		*/
#include	<string.h>	/* Standard String Handling Routines	*/
#include	<stdlib.h>	/* Standard String Handling Routines	*/
#include	<limits.h>	/* Architecture-dependant values	*/
#include	<errno.h>	/* Standard Error Codes			*/

#include	<dbDefs.h>	/* EPICS Standard Definitions		*/
#include        <dbAccess.h>
#include	<dbScan.h>	/* EPICS Data Base Scan Definitions	*/
#include        <recSup.h>	/* EPICS Record Support Definitions	*/
#include        <recGbl.h>
#include        <alarm.h>
#include	<devSup.h>	/* EPICS Device Support Definitions	*/
#include	<link.h>	/* EPICS db Link Field Definitions	*/

#include 	<camacLib.h>	/* EPICS/ESONE CAMAC Library Routines	*/
#include	<devCamac.h>	/* EPICS CAMAC Device Support Defs.	*/

/************************************************************************/
/*  Local Constants							*/
/************************************************************************/

#ifndef vxWorks
#define ERROR -1
#define OK    0
#endif
/*---------------------
 * Conversion codes returned to record support layer
 */
#define	CONVERT		0	/* Successfull return, request conversion    */
#define	DO_NOT_CONVERT	2	/* Successfull return, do not convert	     */

/*---------------------
 * CAMAC Function codes
 */
#define	RD1		0	/* Read group 1 register		     */
#define	RD2		1	/* Read group 2 register		     */
#define	WT1		16	/* Write group 1 register		     */
#define	WT2		17	/* Write group 2 register		     */
#define	SS1		18	/* Selective set group 1 register	     */
#define	SS2		19	/* Selective set group 2 register	     */
#define	SC1		21	/* Selective clear group 1 register	     */
#define	SC2		23	/* Selective clear group 2 register	     */
#define	DISABLE		24	/* Disable (clear) bit			     */
#define	ENABLE		26	/* Enable (set) bit			     */
#define	TST		27	/* Test status				     */

/*---------------------
 * Other CAMAC constants
 */
#define	MAX_BIT		24	/* Maximum number of bits in a CAMAC word    */
#define	MAX_MASK  0xffffff	/* Maximum value of a CAMAC word	     */
#define	CAMAC_PARAM_SZ 	26      /* This used to be defined in link.h 	     */

/************************************************************************/
/*  Local Structure Declarations					*/
/************************************************************************/

typedef struct /* DSET */ {
   long       number;		/* Number of entries in table		  */
   DEVSUPFUN  report;		/* Report routine			  */
   DEVSUPFUN  init;		/* Device support initialization routine  */
   DEVSUPFUN  init_record;	/* Record initialization routine	  */
   DEVSUPFUN  get_ioint_info;	/* Rtn to retrieve I/O event value	  */
   DEVSUPFUN  read_write;	/* Device I/O routine			  */
   DEVSUPFUN  special_linconv;	/* Rtn to adjust linear conversion parms  */
} DSET;


/************************************************************************/
/*  Macro Definitions							*/
/************************************************************************/

/*---------------------
 * Header for routines to be exported from this module
 */
#define	GLOBAL_RTN

/*---------------------
 * Header for routines which are local to this module only
 */
#define	LOCAL_RTN  LOCAL

/*---------------------
 * Check function code for read/write
 */
#define	RW_F(f) ((f & 0x8) == 0)

/************************************************************************/
/*  Device Support Entry Tables						*/
/************************************************************************/

/*---------------------
 * AI (Analog Input) Device Support Entry Table
 */
DSET devAiCamac={
   6,						/* 6 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev Support init	*/
   (DEVSUPFUN)  devCamac_aiInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_aiRead,		/* Read routine		*/
   (DEVSUPFUN)  devCamac_aiLinConv		/* Set conversion slope	*/
   };

/*---------------------
 * AO (Analog Output) Device Support Entry Table
 */
DSET devAoCamac={
   6,						/* 6 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev Support init	*/
   (DEVSUPFUN)  devCamac_aoInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_aoWrite,		/* Write routine	*/
   (DEVSUPFUN)  devCamac_aoLinConv		/* Set conversion slope	*/
   };

/*---------------------
 * BI (Binary Input) Device Support Entry Table
 */
DSET devBiCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_biInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_biRead,		/* Read routine		*/
   NULL
   };

/*---------------------
 * BO (Binary Output) Device Support Entry Table
 */
DSET devBoCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_boInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_boWrite,		/* Write routine	*/
   NULL
   };

/*---------------------
 * LI (Longword Input) Device Support Entry Table
 */
DSET devLiCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_liInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_liRead,		/* Read routine		*/
   NULL
   };

/*---------------------
 * LO (Longword Output) Device Support Entry Table
 */
DSET devLoCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_loInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_loWrite,		/* Write routine	*/
   NULL
   };

/*---------------------
 * MBBI (Multi-Bit Binary Input) Device Support Entry Table
 */
DSET devMbbiCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_mbbiInitRecord,	/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_mbbiRead,		/* Read routine		*/
   NULL
   };

/*---------------------
 * MBBO (Multi-Bit Binary Output) Device Support Entry Table
 */
DSET devMbboCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_mbboInitRecord,	/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_mbboWrite,		/* Write routine	*/
   NULL
   };
/*---------------------
 * WF (Waveform) Device Support Entry Table
 */
DSET devWfCamac={
   5,						/* 5 entries:		*/
   NULL,					/* No report routine	*/
   NULL,					/* No Dev support init	*/
   (DEVSUPFUN)  devCamac_wfInitRecord,		/* Record init routine	*/
   NULL,					/* No I/O event		*/
   (DEVSUPFUN)  devCamac_wfReadWrite,		/* Read/Write routine	*/
   NULL
   };

/************************************************************************/
/* 			 Utility Macro Routines				*/
/*									*/


/*----------------------------------------------------------------------*/
/* commonInitAbort () -- Abort common CAMAC record initialization	*/
/*									*/
/*	This macro is called from CAMAC record initialization routines	*/
/*	when an error is detected in one of the record's hardware	*/
/*	address fields.  It performs the following actions:		*/
/*	o Disables the record so that it will not be processed		*/
/*	o Reports a "badField" error to the local console		*/
/*	o Aborts the record initialization routine with an S_db_badField*/
/*	  status code.							*/
/*									*/
/*	Macro parameters are:						*/
/*	   recType = Type of record (ai, ao, bi, bo, etc..)		*/
/*	   message = Message to output along with standard error msg.	*/
/*									*/
/*	The following variables are assumed to be declared with the	*/
/*	calling procedure:						*/
/*	   prec    = Pointer to record being initialized		*/
/*									*/
/*----------------------------------------------------------------------*/


#define commonInitAbort(recType, message)				\
{									\
   devCamac_recDisable ((void *) prec);					\
   recGblRecordError (S_db_badField, (void *)prec,	 		\
   "devCamac_" #recType "InitRecord:\n-- " message);			\
   return (S_db_badField);						\
									\
}/*end commonInitAbort()*/

/*----------------------------------------------------------------------*/
/* commonInitRecord () -- Common CAMAC record initialization		*/
/*									*/
/*	This macro provides the following functions which are common	*/
/*	to all of the generic CAMAC record initialization procedures	*/
/*	contained in this module.					*/
/*	o Make sure the record type is CAMAC_IO				*/
/*	o Check for the presence of an external CAMAC parameter		*/
/*	  structure and if present, set the function and subaddress	*/
/*	  values from it.						*/
/*	o Allocate the device-dependant data structure for this type	*/
/*	  of record and link it in to the record's DPVT field		*/
/*	o Create the CAMAC channel variable (ext) for this record and	*/
/*	  store it in the record's device dependant structure.		*/
/*									*/
/*	Macro parameters are:						*/
/*	   recType = Type of record (i.e. ai, ao, bi, bo, etc..)	*/
/*	   link    = Name of link field (inp or out)			*/
/*	   LINK    = Upper case name of link field (for error reports)	*/
/*									*/
/*	The following variables are assumed to be declared with the	*/
/*	calling procedure:						*/
/*	   prec          = Pointer to record being initialized		*/
/*	   pcamacio      = Pointer to camacio structure in link field	*/
/*	   pdevCamacParm = Pointer to external parameter structure	*/
/*	   pdevInfo      = Pointer to device-specific info structure	*/
/*									*/
/*----------------------------------------------------------------------*/


#define commonInitRecord(recType, link, LINK)				\
{									\
   int   status;	/* Local status variable	*/		\
									\
  /*---------------------						\
   * Make sure the record type is valid					\
   */									\
   if (prec->link.type != CAMAC_IO)					\
      commonInitAbort (recType, "Device type not Camac");		\
									\
  /*---------------------						\
   * Get pointers to internal and external parameter structures		\
   */									\
   pcamacio = &(prec->link.value.camacio);				\
   pdevCamacParm = (devCamacParm *)prec->dpvt;				\
									\
  /*---------------------						\
   * If an external parameter structure is present, use it to set	\
   * the function code and subaddress.					\
   */									\
   if (pdevCamacParm) {							\
      if (pdevCamacParm->f >= 0) pcamacio->f = pdevCamacParm->f;	\
      if (pdevCamacParm->a >= 0) pcamacio->a = pdevCamacParm->a;	\
   }/*end if external parameter structure present*/			\
									\
  /*---------------------						\
   * Allocate the device-specific information structure			\
   */									\
   pdevInfo = calloc (1, sizeof(devInfo_##recType##Camac));		\
   prec->dpvt = (long *)pdevInfo;					\
   if (pdevInfo == NULL) {						\
      devCamac_recDisable ((void *) prec);				\
      return (ENOMEM);							\
   }/*end if not enough memory to allocate devInfo*/ 			\
									\
  /*---------------------						\
   * Construct the CAMAC channel variable from the values in camacio	\
   */									\
   cdreg (&(pdevInfo->ext), pcamacio->b, pcamacio->c,			\
                            pcamacio->n, pcamacio->a);			\
   if (!pdevInfo->ext) {						\
      ctstat (&status);							\
      devCamac_recDisable ((void *) prec);				\
      recGblRecordError (status, (void *)prec,				\
        "devCamac_" #recType "InitRecord:\n-- Illegal BCNA in "		\
        #LINK " field");						\
      return (status);							\
   }/*end if illegal BCNA*/						\
									\
}/*end commonInitRecord()*/

/************************************************************************/
/*			Common Utility Routines				*/
/*									*/



/************************************************************************/
/* devCamac_getMask () -- Return mask associated with a range of values	*/
/*	o Called by InitRecord routines					*/
/*	o Returns a mask with enough bits set to cover the full range	*/
/*	o Returns 0 if error detected					*/
/*									*/
/************************************************************************/

GLOBAL_RTN
unsigned long  devCamac_getMask (int range1, int range2)
{
   unsigned long   mask;		/* Mask value to return		*/
   int             range;		/* Full-scale range		*/

  /*---------------------
   * Compute the total range by subtracting the smaller number from
   * the larger number.  Return 0 if range too big or too small.
   */
   range = (range1 < range2) ? (range2 - range1) : (range1 - range2);
   if ((range <= 0) || (range > MAX_MASK)) return (0);

  /*---------------------
   * Compute the mask for this range.  Note that we start the loop at 2 so
   * that you can have a one-bit range (if that's what you really want).
   */
   for (mask=2; mask < range; mask<<=1);
   return (--mask);

}/*end devCamac_getMask()*/

/************************************************************************/
/* devCamac_parseParm () -- Parse CAMACIO parameter string		*/
/*	o Parameter string may contain up to 4  values:			*/
/*	  - Mask or first range value (specified as integer or hex mask)*/
/*	  - Second range value (specified as integer, hex mask, or the	*/
/*	    string "bipolar")						*/
/*	  - Readback function code (specified as Rn)			*/
/*	  - Q transfer mode (specified as QSTP, QRPT, or QSCN)		*/
/*	  (Note: the Q transfer mode is only support for Wf records,    */
/*	   which do not support the Readback function code)		*/
/*	o Parameter string values may be separated by whitespace, tab,	*/
/*	  comma, semicolon, or colon.					*/
/*	o Range values are returned as 0 if not defined in parm string	*/
/*	o Readback function code is returned as -1 if not defined	*/
/*	o Q transfer function code is returned as -1 if not defined	*/
/*	o Routine returns number of tokens parsed, ERROR if parse failed*/
/*									*/
/************************************************************************/

GLOBAL_RTN
int  devCamac_parseParm (int *range1, int *range2, int *read_f, 
	int *qmode, char *parm)
{
   int           base;				  /* Base for numeric conv.  */
   int           bipolar;			  /* Indicates bipolar range */
   char         *next;				  /* Temp ptr for strtok_r() */
   int           num_parsed;			  /* Num tokens parsed       */
   char          parse_string [CAMAC_PARAM_SZ+1]; /* Temp string for parsing */
   int          *prange;			  /* Current range variable  */
   char         *ptend;				  /* End of current number   */
   int           range_counter;			  /* Num ranges parsed       */
   static char  *separators = "\t ,;:";		  /* Separator list          */
   char         *token;				  /* Ptr to current token    */

  /*---------------------
   * Copy the string to parse into temporary storage (since the strtok_r()
   * function is going to write nulls all over it).
   */
   strncpy (parse_string, parm, CAMAC_PARAM_SZ);
   parse_string[CAMAC_PARAM_SZ] = '\0';

  /*---------------------
   * Initialize the return values to their default (not defined) values and
   * get the first token.
   */
   *range1 = *range2 = 0;
   *read_f = -1;

   bipolar = FALSE;	/* No bipolar flag yet			*/
   num_parsed = 0;	/* No tokens processed yet		*/
   prange = range1;	/* Address of 1st range variable	*/
   range_counter = 0;	/* Number of range values seen so far	*/

   token = strtok_r (parse_string, separators, &next);

  /*---------------------
   * Loop to parse individual tokens until an error is detected or there
   * are no more tokens.
   */
   while (token) {

     /*---------------------
      * Decide whether this is a range or a readback token
      */
      base = 10;
      switch (*token) {

        /*---------------------
         * Parse the readback function code
         */
         case 'R':
         case 'r':
            if (*read_f >= 0) return ERROR;	/* Sorry, already have one  */
            *read_f = strtol (++token, &ptend, 10);
            if (ptend == token) return ERROR;	/* Can't parse number	    */
            break;

        /*---------------------
         * Bipolar flag
         *
         * If we have already seen the first range parameter, set the second
         * range parameter to be its negative.  If we have not yet seen the
         * first range parameter, set the "bipolar" flag so that when we do
         * see it, we treat it accordingly.
         */
         case 'B':
         case 'b':
            token++;  /* Check the rest of the characters */
            if (memcmp(token,"ipolar",7) && memcmp(token,"IPOLAR",7)) return ERROR;

            if (++range_counter > 2) return ERROR;
            else if (range_counter == 2) *range2 = -(*range1);
            else bipolar = TRUE;
            break;

        /*---------------------
         * Q transfer mode
         *
         */
         case 'Q':
         case 'q':
            token++;  /* Check the rest of the characters */
            *qmode = TMOD_QRPT;
            if (!memcmp(token,"RPT",3) || !memcmp(token,"rpt",3)) 
            		*qmode=TMOD_QRPT;
            else if (!memcmp(token,"STP",3) || !memcmp(token,"stp",3)) 
            		*qmode=TMOD_QSTP;
            else if (!memcmp(token,"SCN",3) || !memcmp(token,"scn",3)) 
            		*qmode=TMOD_QSCN;
            else
            		return ERROR;
            break;

        /*---------------------
         * Check for hexadecimal range parameter
         */
         case '0':
            if ((token[1] == 'x') || (token[1] == 'X')) base = 16;
            goto parse_range;

         case '+':
         case '-':
            if (token[1] == '0')
               if ((token[2] == 'x') || (token[2] == 'X')) base = 16;

        /*---------------------
         * Parse a range parameter
         *
         * If the "bipolar" flag is set, then treat the first range parameter
         * as if both positive and negative ranges were present.
         */
         default:
         parse_range:
            if (++range_counter > 2) return ERROR; /* Too many range values  */

            *prange = strtol (token, &ptend, base);
            if (ptend == token) return ERROR;	   /* Can't parse number     */

            if (bipolar) *range2 = -(*range1);     /* Bipolar, set both parms*/
            else prange = range2;		   /* else use range2 next   */
      }/*end switch*/

     /*---------------------
      * Get the next token and repeat
      */
      num_parsed++;
      token = strtok_r (NULL, separators, &next);

   }/*end while more tokens*/

  /*---------------------
   * If we didn't get an error, return how many tokens we parsed
   */
   return num_parsed;

}/*end devCamac_parseParm()*/

/************************************************************************/
/* devCamac_recDisable () -- Disable record from being processed	*/
/*	o Called when an error is detected at record initialization	*/
/*	o Disables the record by setting the DISV and DISA fields	*/
/*	o Sets STAT and SEVR fields to indicate record disabled		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
void  devCamac_recDisable (struct dbCommon *prec)
{
   prec->disa = TRUE;
   prec->disv = TRUE;		/* Record disabled when DISA = DISV	*/
   prec->disp = TRUE;		/* Also disable putFields to this record*/

   prec->stat = DISABLE_ALARM;	/* Set the record status to DISABLE	*/
   prec->sevr = INVALID_ALARM;	/* Set severity to INVALID		*/
   prec->diss = INVALID_ALARM;	/* Also set DISS for completeness	*/

}/*end devCamac_recDisable()*/

/************************************************************************/
/*			Common Readback Routines			*/
/*									*/


/************************************************************************/
/* devCamac_aioReadback () -- Common readback routine for analog i/o	*/
/*	o Reads value from card and masks for precision & sign extension*/
/*	o Returns ERROR if read error, OK otherwise.			*/
/*	o Note assumption that ai & ao records have identical formats	*/
/*	  for their device-dependent information structures.		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_aioReadback (devInfo_aiCamac *pdevInfo, int *value)
{
   int   data;		/* Raw data value	*/
   int   q;		/* Q reply		*/

  /*---------------------
   * Get the raw value and check for errors
   */
   cfsa (pdevInfo->f_read, pdevInfo->ext, &data, &q);
   if (!q) return ERROR;

  /*---------------------
   * Mask raw value for correct precision and apply sign extension if required
   */
   data &= pdevInfo->mask;
   if (data & pdevInfo->sign_mask) data |= pdevInfo->sign_extend;

   *value = data;
   return OK;

}/*end devCamac_aioReadback()*/

/************************************************************************/
/* devCamac_bioReadback () -- Common readback routine for binary i/o	*/
/*	o Reads value from card and returns masked value		*/
/*	o Returns OK if read succeeded.					*/
/*	o Note assumption that bi & bo records have identical formats	*/
/*	  for their device-dependent information structures.		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_bioReadback (devInfo_biCamac *pdevInfo, int *value)
{
   int   data;		/* Raw data value		*/
   int   q;		/* Q reply			*/
   int   status;	/* Local status variable	*/

  /*---------------------
   * Get the raw value and check for errors
   */
   cfsa (pdevInfo->f_read, pdevInfo->ext, &data, &q);
   ctstat (&status);

  /*---------------------
   * If readback is actually a "test" function, return the Q status instead
   */
   if (pdevInfo->test) {
      if ((status == OK) || (status == S_camacLib_noQ)) {
         *value = q;
         return OK;
      }/*end if read succeeded*/
   }/*end if test function*/

  /*---------------------
   * If this is a regular dataway read, return the masked value
   */
   else if (q) *value = data & pdevInfo->mask;

  /*---------------------
   * Return the status code from the read
   */
   return status;

}/*end devCamac_bioReadback()*/

/************************************************************************/
/* devCamac_lioReadback () -- Common readback routine for longword i/o	*/
/*	o Reads value from card and masks for precision & sign extension*/
/*	o Returns ERROR if read error, OK otherwise.			*/
/*	o Note assumption that li & lo records have identical formats	*/
/*	  for their device-dependent information structures.		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_lioReadback (devInfo_liCamac *pdevInfo, int *value)
{
   int   data;		/* Raw data value	*/
   int   q;		/* Q reply		*/

  /*---------------------
   * Get the raw value and check for errors
   */
   cfsa (pdevInfo->f_read, pdevInfo->ext, &data, &q);
   if (!q) return ERROR;

  /*---------------------
   * Mask raw value for correct precision and apply sign extension if required
   */
   data &= pdevInfo->mask;
   if (data & pdevInfo->sign_mask) data |= pdevInfo->sign_extend;

   *value = data;
   return OK;

}/*end devCamac_lioReadback()*/

/************************************************************************/
/* devCamac_mbbioReadback () -- Common readback routine for multi-bit	*/
/*                              binary i/o				*/
/*	o Reads value from card and returns masked value		*/
/*	o Returns OK if read succeeded.					*/
/*	o Note assumption that mbbi & mbbo records have identical	*/
/*	  formats for their device-dependent information structures.	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_mbbioReadback (devInfo_mbbiCamac *pdevInfo, int *value)
{
   int   data;		/* Raw data value		*/
   int   q;		/* Q reply			*/

  /*---------------------
   * Get the raw value and check for errors
   */
   cfsa (pdevInfo->f_read, pdevInfo->ext, &data, &q);
   if (!q) return ERROR;

  /*---------------------
   * Mask data and return
   */
   *value = data & pdevInfo->mask;
   return OK;

}/*end devCamac_mbbioReadback()*/

/************************************************************************/
/*			Analog Input Routines				*/
/*									*/


/************************************************************************/
/* devCamac_aiInitRecord () -- Intialize a CAMAC ai Record		*/
/*									*/
/************************************************************************/
GLOBAL_RTN
STATUS devCamac_aiInitRecord (struct aiRecord *prec)
{
   int               bipolar;		/* True if device is bi-polar	    */
   int               f_readback;	/* Used for PARM parse		    */
   register int      mask;		/* Mask for device range	    */
   int               parm_count;	/* Number of parms in INP.parm	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of INP filed */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_aiCamac  *pdevInfo;		/* Ptr to device-dependant struct   */
   int               range1;		/* 1st range parameter		    */
   int               range2;		/* 2nd range parameter		    */
   int               invalid;		/* Used to parse PARM field	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and the
   * external parameter structure (if present).  Initialize the CAMAC channel
   * variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (ai, inp, INP);

  /*---------------------
   * If we were called from an external device-support routine, get the
   * mask and bipolar flag from the external parameter structure.
   */
   if (pdevCamacParm) {
      mask = pdevCamacParm->range_mask;
      bipolar = pdevCamacParm->bipolar;
   }/*end if external parameter structure is present*/

  /*---------------------
   * If there is no external parameter structure, then compute the mask and
   * bipolar flag from the range information in the PARM field.
   *
   * Note: A device is defined as bipolar when its two range values have
   * opposite signs.
   */
   else {
      parm_count = devCamac_parseParm (&range1, &range2, &f_readback,
      			&invalid, (char *)pcamacio->parm);
      if ((parm_count == ERROR) || (f_readback >= 0))
         commonInitAbort (ai, "Illegal PARM in INP field");

      if (parm_count == 0) mask = MAX_MASK;
      else {
         mask = devCamac_getMask (range1, range2);
         if (!mask) commonInitAbort (ai, "Illegal range parameter(s)");
      }/*end if range parameters specified*/

      bipolar = (range1^range2) & LONG_MIN;

   }/*end if no external parameter structure*/

  /*---------------------
   * If this is a bipolar device, we need to set values for the sign mask,
   * the sign extension mask, and the record's ROFF field.  Also adjust the
   * range to accomodate the asymetry of two's-compliment binary notation.
   */
   if (bipolar) {
      prec->roff = mask >> 1;
      pdevInfo->sign_mask = (mask + 1) >> 1;
      pdevInfo->sign_extend = ~mask;
      pdevInfo->range = mask - 1;
   }/*end if bipolar device*/

  /*---------------------
   * If device is not bipolar, set ROFF and sign masks to 0
   */
   else {
      prec->roff = 0;
      pdevInfo->sign_mask = 0;
      pdevInfo->sign_extend = 0;
      pdevInfo->range = mask;
   }/*end if not bipolar device*/

  /*---------------------
   * Finish intializing the device-dependent information structure
   */
   pdevInfo->f_read = pcamacio->f;
   pdevInfo->mask = mask;

  /*---------------------
   * Calculate engineering slope for linear conversion and return.
   */
   devCamac_aiLinConv (prec, -1);
   return (OK);

}/*end devCamac_aiInitRecord()*/

/************************************************************************/
/* devCamac_aiRead () -- Analog input read routine			*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_aiRead (struct aiRecord *prec)
{
   register devInfo_aiCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   register int               status;	/* Local status variable	   */
   int                        value;	/* Value read from card		   */

  /*---------------------
   * Issue the read and check for errors
   */
   pdevInfo = (devInfo_aiCamac *)prec->dpvt;
   status = devCamac_aioReadback (pdevInfo, &value);
   if (status == OK) {
      prec->rval = value;
      return (OK);
   }/*end if read succeeded*/

  /*---------------------
   * Raise an alarm condition if the read failed
   */   
   recGblSetSevr (prec, READ_ALARM, INVALID_ALARM);
   return (READ_ALARM);

}/*end devCamac_aiRead()*/

/************************************************************************/
/* devCamac_aiLinConv () -- Special Linear Conversion Routine for AI	*/
/*	o Computes new slope (ESLO) whenever upper and/or lower		*/
/*	  engineering unit fields (EGUF & EGUL) are changed		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_aiLinConv (struct aiRecord *prec, int after)
{
   devInfo_aiCamac  *pdevInfo;		/* Pointer to devInfo structure	*/

   if (!after) return (OK);
   pdevInfo = (devInfo_aiCamac *)prec->dpvt;
   prec->eslo = (prec->eguf - prec->egul) / pdevInfo->range;
   return (OK);

}/*end devCamac_aiLinConv()*/

/************************************************************************/
/*			Analog Output Routines				*/
/*									*/


/************************************************************************/
/* devCamac_aoInitRecord () -- Intialize a CAMAC ao Record		*/
/*									*/
/************************************************************************/
GLOBAL_RTN
STATUS devCamac_aoInitRecord (struct aoRecord *prec)
{
   int               bipolar;		/* True if device is bi-polar	    */
   int               f_read;		/* Function code for readback	    */
   register int      mask;		/* Mask for device range	    */
   int               parm_count;	/* Number of parms in OUT.parm	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of OUT filed */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_aoCamac  *pdevInfo;		/* Ptr to device-dependant struct   */
   int               range1;		/* 1st range parameter		    */
   int               range2;		/* 2nd range parameter		    */
   int               invalid;		/* Used to parse PARM field	    */
   int               readback;		/* Current value of hardware	    */
   int               status;		/* Local status variable	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and the
   * external parameter structure (if present).  Initialize the CAMAC channel
   * variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (ao, out, OUT);

  /*---------------------
   * If we were called from an external device-support routine, get the
   * mask, readback function code, and bipolar flag from the external
   * parameter structure.
   */
   if (pdevCamacParm) {
      f_read = pdevCamacParm->readback;
      mask = pdevCamacParm->range_mask;
      bipolar = pdevCamacParm->bipolar;
   }/*end if external parameter structure is present*/

  /*---------------------
   * If there is no external parameter structure, then compute the mask, 
   * bipolar flag, and the readback function code from information in the
   * PARM field.
   *
   * Note: A device is defined as bipolar when its two range values have
   * opposite signs.
   */
   else {
      parm_count = devCamac_parseParm (&range1, &range2, &f_read, 
      				&invalid, (char *)pcamacio->parm);
      if (parm_count == ERROR) commonInitAbort (ao, "Illegal PARM in OUT field");

      if (parm_count == 0) mask = MAX_MASK;
      else {
         mask = devCamac_getMask (range1, range2);
         if (!mask) commonInitAbort (ao, "Illegal range parameter(s)");
      }/*end if range parameters specified*/

      bipolar = (range1^range2) & LONG_MIN;

   }/*end if no external parameter structure*/

  /*---------------------
   * If this is a bipolar device, we need to set values for the sign mask,
   * the sign extension mask, and the record's ROFF field.  Also adjust the
   * range to accomodate the asymetry of two's-compliment binary notation.
   */
   if (bipolar) {
      prec->roff = mask >> 1;
      pdevInfo->sign_extend = ~mask;
      pdevInfo->sign_mask = (mask + 1) >> 1;
      pdevInfo->range = mask - 1;
   }/*end if bipolar device*/

  /*---------------------
   * Set range for unipolar device
   */
   else
      pdevInfo->range = mask;

  /*---------------------
   * Finish intializing the device-dependent information structure
   */
   pdevInfo->f_write = pcamacio->f;
   pdevInfo->f_read = f_read;
   pdevInfo->mask = mask;

  /*---------------------
   * Calculate engineering slope for linear conversion
   */
   devCamac_aoLinConv (prec, -1);  

  /*---------------------
   * If the card has a readback function, read the current value into
   * the RVAL and RBV fields.
   */
   if (f_read >= 0) {
      status = devCamac_aioReadback (pdevInfo, &readback);
      if (status == OK) {
         prec->rval = readback;
         prec->rbv  = readback;
         prec->stat = prec->sevr = NO_ALARM;
         return (CONVERT);
      }/*end if readback succeeded*/
   }/*end if card can be read*/

  /*---------------------
   * Record initialization complete, but initial value not read
   */
   return (DO_NOT_CONVERT);

}/*end devCamac_aoInitRecord()*/

/************************************************************************/
/* devCamac_aoWrite () -- Analog output write routine			*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_aoWrite (struct aoRecord *prec)
{
   register devInfo_aoCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   int                        q;	/* Q status of write		   */
   int                        val;	/* Value to write to card	   */

  /*---------------------
   * Mask the output word to the required precision
   */
   pdevInfo = (devInfo_aoCamac *)prec->dpvt;
   val = prec->rval & pdevInfo->mask;

  /*---------------------
   * Issue the write and check for errors
   */
   cfsa (pdevInfo->f_write, pdevInfo->ext, &val, &q);
   if (!q) {
      recGblSetSevr (prec, WRITE_ALARM, INVALID_ALARM);
      return (WRITE_ALARM);
   }/*end if error writing to hardware*/

  /*---------------------
   * If a readback function code is defined, do a hardware readback,
   * otherwise, just set the readback to the command value.
   */
   if (pdevInfo->f_read >= 0) devCamac_aioReadback (pdevInfo, (int *)&(prec->rbv));
   else prec->rbv = prec->rval;

   return (OK);

}/*end devCamac_aoWrite()*/

/************************************************************************/
/* devCamac_aoLinConv () -- Special Linear Conversion Routine for AO	*/
/*	o Computes new slope (ESLO) whenever upper and/or lower		*/
/*	  engineering unit fields (EGUF & EGUL) are changed		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_aoLinConv (struct aoRecord *prec, int after)
{
   devInfo_aoCamac  *pdevInfo;		/* Pointer to devInfo structure	*/

   if (!after) return (OK);
   pdevInfo = (devInfo_aoCamac *)prec->dpvt;
   prec->eslo = (prec->eguf - prec->egul) / pdevInfo->range;
   return (OK);

}/*end devCamac_aoLinConv()*/

/************************************************************************/
/*			Binary Input Routines				*/
/*									*/


/************************************************************************/
/* devCamac_biInitRecord () -- Binary input record initialization	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_biInitRecord (struct biRecord *prec)
{
   int               bit_num;		/* Bit no. (0-23) to control	    */
   int               f_readback;	/* Used to parse PARM field	    */
   int               invalid;		/* Used to parse PARM field	    */
   int               parm_count;	/* Number of parms in INP.PARM 	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of INP field */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_biCamac  *pdevInfo;		/* Ptr to device-dependant struct.  */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (bi, inp, INP);

  /*---------------------
   * Extract the bit offset from the camacio.parm field.  Abort initialization
   * if the parse failed.
   */
   parm_count = devCamac_parseParm (&bit_num, &invalid, &f_readback, 
   				&invalid, (char *)pcamacio->parm);
   if (invalid || (parm_count == ERROR) || (f_readback >=0) ||
      ((unsigned long)bit_num >= MAX_BIT)) {
      commonInitAbort (bi, "Illegal PARM in INP field");
   }/*end if parameter parse failed*/

  /*---------------------
   * Finish initializating the device-dependant structure
   */
   prec->mask = 1 << bit_num;		/* Set mask field in record	*/
   pdevInfo->mask = prec->mask;		/* Set mask field in devInfo	*/
   pdevInfo->f_read = pcamacio->f;	/* Set read function code	*/

  /*---------------------
   * If the read function code does not use the dataway read lines,  set
   * the "test" flag so that the read routine will use the Q response
   * instead.
   */
   pdevInfo->test = !RW_F(pdevInfo->f_read);

  /*---------------------
   * Record initialization complete.
   */
   return (OK);

}/*end devCamac_biInitRecord()*/

/************************************************************************/
/* devCamac_biRead () -- Binary input read routine			*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_biRead (struct biRecord *prec)
{
   register devInfo_biCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   register int               status;	/* Local status variable	   */
   int                        value;	/* Value read from card		   */

  /*---------------------
   * Issue the read and check for errors
   */
   pdevInfo = (devInfo_biCamac *)prec->dpvt;
   status = devCamac_bioReadback (pdevInfo, &value);
   if (status == OK) {
      prec->rval = value;
      return (OK);
   }/*end if read succeeded*/

  /*---------------------
   * Raise an alarm condition if the read failed
   */   
   recGblSetSevr (prec, READ_ALARM, INVALID_ALARM);
   return (READ_ALARM);

}/*end devCamac_biRead()*/

/************************************************************************/
/*			Binary Output Routines				*/
/*									*/


/************************************************************************/
/* devCamac_boInitRecord () -- Binary output record initialization	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_boInitRecord (struct boRecord *prec)
{
   int               bit_num;		/* Bit no. (0-23) to control	    */
   int               f_readback;	/* Function code for readback	    */
   int               invalid;		/* Used to parse PARM field	    */
   int               parm_count;	/* Number of parms in OUT.PARM 	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of OUT field */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_boCamac  *pdevInfo;		/* Ptr to device-dependant struct.  */
   int               readback;		/* Readback value		    */
   int               status;		/* Local status variable	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and the
   * external parameter structure (if present).  Initialize the CAMAC channel
   * variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (bo, out, OUT);

  /*---------------------
   * Extract the bit offset and readback function (if specified) from the
   * camacio.parm field.  Use the bit offset to set the mask fields in
   * the bo record and in the device-dependant structure.
   */
   parm_count = devCamac_parseParm (&bit_num, &invalid, &f_readback, 
   				&invalid, (char *)pcamacio->parm);
   if (invalid || (parm_count == ERROR) || ((unsigned long)bit_num >= MAX_BIT))
      commonInitAbort (bo, "Illegal PARM in OUT field");

   prec->mask = 1 << bit_num;		/* Set mask field in record	*/
   pdevInfo->mask = prec->mask;		/* Set mask field in devInfo	*/

  /*---------------------
   * If an external parameter structure exists, its value overrides the
   * readback function code value from the PARM field.
   */
   if (pdevCamacParm) f_readback = pdevCamacParm->readback;

  /*---------------------
   * Figure out what function codes to use based on the function code passed
   * to us in the OUT field.
   *
   * NOTE 1: Generic CAMAC device support assumes symetry, i.e. if the
   * specified function code is a group 1 read, then we assume that the
   * corresponding write is a group 1 write to the same CAMAC channel
   * variable.
   *
   * NOTE 2: Any fields not explicitly set in the devInfo structure will
   * be zero by default.
   */
   switch (pcamacio->f) {

      /* Function code is group 1 read/write */
      case RD1:
      case WT1:
         pdevInfo->f_read = RD1;
         pdevInfo->f_write = WT1;
         pdevInfo->merge_mask = ~(prec->mask);
         break;

      /* Function code is group 2 read/write */
      case RD2:
      case WT2:
         pdevInfo->f_read = RD2;
         pdevInfo->f_write = WT2;
         pdevInfo->merge_mask = ~(prec->mask);
         break;

      /* Function code is group 1 selective set/clear */
      case SS1:
      case SC1:
         pdevInfo->f_read  = f_readback;
         pdevInfo->f_write = SS1;
         pdevInfo->f_clear = SC1;
         break;

      /* Function code is group 2 selective set/clear */
      case SS2:
      case SC2:
         pdevInfo->f_read  = f_readback;
         pdevInfo->f_write = SS2;
         pdevInfo->f_clear = SC2;
         break;

      /* Function code is enable/disable */
      case DISABLE:
      case ENABLE:
         pdevInfo->f_read  = f_readback;
         pdevInfo->f_write = ENABLE;
         pdevInfo->f_clear = DISABLE;
         break;

      /* Other function codes not allowed */
      default:
         commonInitAbort (bo, "Illegal function code for BO device");

   }/*end switch*/

  /*---------------------
   * Set the "test" flag if there is a readback function and it doesn't use
   * the dataway lines. 
   */
   pdevInfo->test = (pdevInfo->f_read >= 0) && !RW_F(pdevInfo->f_read);

  /*---------------------
   * If the card has a readback function, read the current value into
   * the RVAL and RBV fields.
   */
   if (pdevInfo->f_read >= 0) {
      status = devCamac_bioReadback (pdevInfo, &readback);
      if (status == OK) {
         prec->rval = readback;
         prec->rbv  = readback;
         prec->stat = prec->sevr = NO_ALARM;
         return (CONVERT);
      }/*end if read succeeded*/
   }/*end if card has readback*/

  /*---------------------
   * Record initialization complete, but initial value not read.
   */
   return (DO_NOT_CONVERT);

}/*end devCamac_boInitRecord()*/

/************************************************************************/
/* devCamac_boWrite () -- Binary output write routine			*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_boWrite (struct boRecord *prec)
{
   int                        f;	/* Function code (for set/clear)*/
   register devInfo_boCamac  *pdevInfo;	/* Ptr to device info structure	*/
   int                        q;	/* Q response			*/
   int                        status;	/* Local status variable	*/
   unsigned int               value;	/* Value to write		*/

  /*---------------------
   * Get the device-dependant information structure
   */
   q = FALSE;	/* In case we die before issuing any CAMAC commands */
   pdevInfo = (devInfo_boCamac *)prec->dpvt;

  /*---------------------
   * If this record has a merge-mask, than it is a read/modify/write type
   * of device.  Lock the CAMAC branch while we are executing this sequence
   * to prevent interference from other devices in the same card.
   */
   if (pdevInfo->merge_mask) {
      status = camacLockBranch (pdevInfo->ext);
      if (status == OK) {
         cfsa (pdevInfo->f_read, pdevInfo->ext, (int *)&value, &q);
         if (q) {
            if (prec->rval) value |= pdevInfo->mask;
            else value &= pdevInfo->merge_mask;
            cfsa (pdevInfo->f_write, pdevInfo->ext, (int *)&value, &q);
         }/*end if read was successfull*/
         camacUnlockBranch (pdevInfo->ext);
      }/*end if lock was successfull*/
   }/*end if read/write device*/

  /*---------------------
   * Card supports selective set/clear (or this is an enable/disable bit).
   * Decide which function code to use based on the value in RVAL.
   */
   else {
      f = prec->rval ? pdevInfo->f_write : pdevInfo->f_clear;
      cfsa (f, pdevInfo->ext, (int *)&(prec->mask), &q);
   }/*end if selective set/clear device*/

  /*---------------------
   * Check for errors
   */
   if (!q) {
      recGblSetSevr (prec, WRITE_ALARM, INVALID_ALARM);
      return (WRITE_ALARM);
   }/*end if error writing to hardware*/

  /*---------------------
   * If a readback function code is defined, do a hardware readback,
   * otherwise, just set the readback to the command value.
   */
   if (pdevInfo->f_read >= 0) devCamac_bioReadback (pdevInfo, (int *)&(prec->rbv));
   else prec->rbv = prec->rval;

   return (OK);

}/*end devCamac_boWrite()*/

/************************************************************************/
/*			Longword Input Routines				*/
/*									*/


/************************************************************************/
/* devCamac_liInitRecord () -- Intialize a CAMAC li Record		*/
/*									*/
/************************************************************************/
GLOBAL_RTN
STATUS devCamac_liInitRecord (struct longinRecord *prec)
{
   int               bipolar;		/* True if device is bipolar	    */
   int               f_readback;	/* Used for PARM parse		    */
   register int      mask;		/* Mask for device range	    */
   int               parm_count;	/* Number of parms in INP.parm	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of INP filed */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_liCamac  *pdevInfo;		/* Ptr to device-dependant struct   */
   int               range1;		/* 1st range parameter		    */
   int               range2;		/* 2nd range parameter		    */
   int               invalid;		/* Used to parse PARM field	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (li, inp, INP);

  /*---------------------
   * If we were called from an external device-support routine, get the
   * mask and bipolar flag from the external parameter structure.
   */
   if (pdevCamacParm) {
      mask = pdevCamacParm->range_mask;
      bipolar = pdevCamacParm->bipolar;
    }/*end if external parameter structure is present*/

  /*---------------------
   * If there is no external parameter structure, then compute the mask and
   * bipolar flag from the range information in the PARM field.
   *
   * Note:  A device is defined a bipolar when its two renage values have
   * opposite signs.
   */
   else {
      parm_count = devCamac_parseParm (&range1, &range2, &f_readback, 
      				&invalid, (char *)pcamacio->parm);
      if ((parm_count == ERROR) || (f_readback >= 0))
         commonInitAbort (li, "Illegal PARM in INP field");

      if (parm_count == 0) mask = MAX_MASK;
      else {
         mask = devCamac_getMask (range1, range2);
         if (!mask) commonInitAbort (li, "Illegal range parameter(s)");
      }/*end if range parameters specified*/

      bipolar = (range1^range2) & LONG_MIN;
   }/*end if no external parameter structure*/

  /*---------------------
   * If this is a bipolar device, we need to set values for the sign mask,
   * and the sign extension mask.
   */
   if (bipolar) {
      pdevInfo->sign_mask = (mask + 1) >> 1;
      pdevInfo->sign_extend = ~mask;
   }/*end if bipolar device*/

  /*---------------------
   * Finish intializing the device-dependent information structure
   */
   pdevInfo->f_read = pcamacio->f;
   pdevInfo->mask = mask;

   return (OK);

}/*end devCamac_liInitRecord()*/

/************************************************************************/
/* devCamac_liRead () -- Longword input read routine			*/
/*	o Note:  This routine sets the UDF field to FALSE (since record	*/
/*	  support does not seem to do it))				*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_liRead (struct longinRecord *prec)
{
   register devInfo_liCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   register int               status;	/* Local status variable	   */
   int                        value;	/* Value read from card		   */

  /*---------------------
   * Issue the read and check for errors
   */
   pdevInfo = (devInfo_liCamac *)prec->dpvt;
   status = devCamac_lioReadback (pdevInfo, &value);
   prec->udf = FALSE;
   if (status == OK) {
      prec->val = value;
      return (OK);
   }/*end if read succeeded*/

  /*---------------------
   * Raise an alarm condition if the read failed
   */   
   recGblSetSevr (prec, READ_ALARM, INVALID_ALARM);
   return (READ_ALARM);

}/*end devCamac_liRead()*/

/************************************************************************/
/*			Longword Output Routines			*/
/*									*/


/************************************************************************/
/* devCamac_loInitRecord () -- Intialize a CAMAC lo Record		*/
/*									*/
/************************************************************************/
GLOBAL_RTN
STATUS devCamac_loInitRecord (struct longoutRecord *prec)
{
   int               bipolar;		/* True if device is bi-polar	    */
   int               f_read;		/* Function code for readback	    */
   register int      mask;		/* Mask for device range	    */
   int               parm_count;	/* Number of parms in OUT.parm	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of OUT filed */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_loCamac  *pdevInfo;		/* Ptr to device-dependant struct   */
   int               range1;		/* 1st range parameter		    */
   int               range2;		/* 2nd range parameter		    */
   int               invalid;		/* Used to parse PARM field	    */
   int               readback;		/* Current value of hardware	    */
   int               status;		/* Local status variable	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (lo, out, OUT);

  /*---------------------
   * If we were called from an external device-support routine, get the
   * mask, readback function code, and bipolar flag from the external
   * parameter structure.
   */
   if (pdevCamacParm) {
      f_read = pdevCamacParm->readback;
      mask = pdevCamacParm->range_mask;
      bipolar = pdevCamacParm->bipolar;
    }/*end if external parameter structure is present*/

  /*---------------------
   * if there is no external parameter structure, then compute the mask,
   * bipolar flag, and the readback function code from information in the
   * PARM field.
   *
   * Note:  A device is defined as bipolar when its two range values have
   * opposite signs.
   */

   else {
      parm_count = devCamac_parseParm (&range1, &range2, &f_read, 
      				&invalid, (char *)pcamacio->parm);
      if (parm_count == ERROR) commonInitAbort (lo, "Illegal PARM in OUT field");

      if (parm_count == 0) mask = MAX_MASK;
      else {
         mask = devCamac_getMask (range1, range2);
         if (!mask) commonInitAbort (lo, "Illegal range parameter(s)");
      }/*end if range parameters specified*/

      bipolar = (range1^range2) & LONG_MIN;

    }/*end if no external parameter structure*/

  /*---------------------
   * If this is a bipolar device, we need to set values for the sign mask
   * and the sign extension mask.
   */
   if (bipolar) {
      pdevInfo->sign_extend = ~mask;
      pdevInfo->sign_mask = (mask + 1) >> 1;
   }/*end if bipolar device*/

  /*---------------------
   * Finish intializing the device-dependent information structure
   */
   pdevInfo->f_write = pcamacio->f;
   pdevInfo->f_read = f_read;
   pdevInfo->mask = mask;

  /*---------------------
   * If the card has a readback function, read the current value into
   * the VAL field.
   */
   if (f_read >= 0) {
      status = devCamac_lioReadback (pdevInfo, &readback);
      if (status == OK) {
         prec->val = readback;
         prec->stat = prec->sevr = NO_ALARM;
      }/*end if readback succeeded*/
   }/*end if card can be read*/

  /*---------------------
   * Record initialization complete
   */
   return (OK);

}/*end devCamac_loInitRecord()*/

/************************************************************************/
/* devCamac_loWrite () -- Longword output write routine			*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_loWrite (struct longoutRecord *prec)
{
   register devInfo_loCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   int                        q;	/* Q status of write		   */
   int                        value;	/* Value to write to card	   */

  /*---------------------
   * Mask the output word to the required precision
   */
   pdevInfo = (devInfo_loCamac *)prec->dpvt;
   value = prec->val & pdevInfo->mask;

  /*---------------------
   * Issue the write and check for errors
   */
   cfsa (pdevInfo->f_write, pdevInfo->ext, &value, &q);
   if (!q) {
      recGblSetSevr (prec, WRITE_ALARM, INVALID_ALARM);
      return (WRITE_ALARM);
   }/*end if error writing to hardware*/

   return (OK);

}/*end devCamac_loWrite()*/

/************************************************************************/
/*		Multi-Bit Binary Input Routines				*/
/*									*/


/************************************************************************/
/* devCamac_mbbiInitRecord () -- Multi-Bit Binary Input Record Init	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_mbbiInitRecord (struct mbbiRecord *prec)
{
   int                 f_readback;	/* Used to parse PARM field	    */
   int                 invalid;		/* Used to parse PARM field	    */
   int                 offset;		/* Bit no. (0-23) to control	    */
   int                 parm_count;	/* Number of parms in INP.PARM 	    */
   struct camacio     *pcamacio;	/* Ptr to camacio part of INP field */
   devCamacParm       *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_mbbiCamac  *pdevInfo;	/* Ptr to device-dependant struct.  */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (mbbi, inp, INP);

  /*---------------------
   * Extract the bit offset from the camacio.parm field.  Abort initialization
   * if the parse failed.
   */
   parm_count = devCamac_parseParm (&offset, &invalid, &f_readback, 
   				&invalid, (char *)pcamacio->parm);
   if (invalid || (parm_count == ERROR) || (f_readback >=0) ||
      ((unsigned long)offset >= MAX_BIT) || ((offset + prec->nobt) > MAX_BIT)) {
      commonInitAbort (mbbi, "Illegal PARM in INP field");
   }/*end if parameter parse failed*/

  /*---------------------
   * Set the shift count and mask in the appropriate mbbi record fields
   * Finish initializating the device-dependant structure
   */
   prec->shft = offset;			/* Set shift field in record	*/
   prec->mask <<= offset;		/* Set mask field in record	*/

   pdevInfo->mask = prec->mask;		/* Set mask field in devInfo	*/
   pdevInfo->f_read = pcamacio->f;	/* Set read function code	*/

  /*---------------------
   * Record initialization complete.
   */
   return (OK);

}/*end devCamac_mbbiInitRecord()*/

/************************************************************************/
/* devCamac_mbbiRead () -- Multi-Bit Binary Input read routine		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_mbbiRead (struct mbbiRecord *prec)
{
   register devInfo_mbbiCamac  *pdevInfo;	/* Ptr to devInfo struct */
   register int                 status;		/* Local status variable */
   int                          value;		/* Value read from card	 */

  /*---------------------
   * Issue the read and check for errors
   */
   pdevInfo = (devInfo_mbbiCamac *)prec->dpvt;
   status = devCamac_mbbioReadback (pdevInfo, &value);
   if (status == OK) {
      prec->rval = value;
      return (OK);
   }/*end if read succeeded*/

  /*---------------------
   * Raise an alarm condition if the read failed
   */   
   recGblSetSevr (prec, READ_ALARM, INVALID_ALARM);
   return (READ_ALARM);

}/*end devCamac_mbbiRead()*/

/************************************************************************/
/*		Multi-Bit Binary Output Routines			*/
/*									*/


/************************************************************************/
/* devCamac_mbboInitRecord () -- Multi-Bit Binary Output Record Init	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_mbboInitRecord (struct mbboRecord *prec)
{
   int                 f_readback;	/* Function code for readback	    */
   int                 invalid;		/* Used to parse PARM field	    */
   int                 offset;		/* Bit offset (0-23) of low bit     */
   int                 parm_count;	/* Number of parms in OUT.parm	    */
   struct camacio     *pcamacio;	/* Ptr to camacio part of OUT field */
   devCamacParm       *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_mbboCamac  *pdevInfo;	/* Ptr to device-dependant struct.  */
   int                 readback;	/* Readback value		    */
   int                 status;		/* Local status variable	    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (mbbo, out, OUT);

  /*---------------------
   * Extract the bit offset and readback function (if specified) from the
   * camacio.parm field.
   */
   parm_count = devCamac_parseParm (&offset, &invalid, &f_readback, 
   				&invalid, (char *)pcamacio->parm);
   if (invalid || (parm_count == ERROR) || ((unsigned long)offset >= MAX_BIT) ||
      ((offset + prec->nobt) > MAX_BIT)) {
      commonInitAbort (mbbo, "Illegal PARM in OUT field");
   }/*end if parameter parse failed*/

  /*---------------------
   * Set the shift count and mask in the appropriate mbbo record fields
   * and in our device-dependant structure.
   */
   prec->shft = offset;
   prec->mask <<= offset;
   pdevInfo->mask = prec->mask;

  /*---------------------
   * If an external parameter structure exists, its value overrides the
   * readback function code value from the PARM field.
   */
   if (pdevCamacParm) f_readback = pdevCamacParm->readback;

  /*---------------------
   * Figure out what function codes to use based on the function code passed
   * to us in the OUT field.
   *
   * NOTE 1: Generic CAMAC device support assumes symetry, i.e. if the
   * specified function code is a group 1 read, then we assume that the
   * corresponding write is a group 1 write to the same CAMAC channel
   * variable.
   *
   * NOTE 2: Any fields not explicitly set in the devInfo structure will
   * be zero by default.
   */
   switch (pcamacio->f) {

      /* Function code is group 1 read/write */
      case RD1:
      case WT1:
         pdevInfo->f_read = RD1;
         pdevInfo->f_write = WT1;
         pdevInfo->merge_mask = ~(prec->mask);
         break;

      /* Function code is group 2 read/write */
      case RD2:
      case WT2:
         pdevInfo->f_read = RD2;
         pdevInfo->f_write = WT2;
         pdevInfo->merge_mask = ~(prec->mask);
         break;

      /* Function code is group 1 selective set/clear */
      case SS1:
      case SC1:
         pdevInfo->f_read  = f_readback;
         pdevInfo->f_write = SS1;
         pdevInfo->f_clear = SC1;
         break;

      /* Function code is group 2 selective set/clear */
      case SS2:
      case SC2:
         pdevInfo->f_read  = f_readback;
         pdevInfo->f_write = SS2;
         pdevInfo->f_clear = SC2;
         break;

     /*---------------------
      * Other function codes not allowed
      */
      default:
         commonInitAbort (mbbo, "Illegal function code for MBBO record");

   }/*end switch*/

  /*---------------------
   * If the card has a readback function, read the current value into
   * the RVAL and RBV fields.
   */
   if (pdevInfo->f_read >=0) {
      status = devCamac_mbbioReadback (pdevInfo, &readback);
      if (status == OK) {
         prec->rval = readback;
         prec->rbv  = readback;
         prec->stat = prec->sevr = NO_ALARM;
         return (CONVERT);
      }/*end if read succeeded*/
   }/*end if card has readback*/

  /*---------------------
   * Record initialization complete, but initial value not read
   */
   return (DO_NOT_CONVERT);

}/*end devCamac_mbboInitRecord()*/

/************************************************************************/
/* devCamac_mbboWrite () -- Multi-Bit Binary Output Write Routine	*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_mbboWrite (struct mbboRecord *prec)
{
   int                          cb[4]={2,0,0,0};/* Control block for cfga    */
   int                          data[2];	/* Data word array for cfga  */
   int                          exta[2];	/* CAMAC channel array (cfga)*/
   int                          fa[2];		/* Function code array (cfga)*/
   register devInfo_mbboCamac  *pdevInfo;	/* Ptr to devInfo struct     */
   int                          q;		/* Q response		     */
   int                          qa[2];		/* Q response array for cfga */
   int                          status;		/* Local status variable     */
   unsigned int                 value;		/* Value to write	     */

  /*---------------------
   * Get the device-dependant information structure
   */
   pdevInfo = (devInfo_mbboCamac *)prec->dpvt;

  /*---------------------
   * If this record has a merge mask, then we should perform the mbbo
   * operation by first reading the previous value, changing the desired bits,
   * and then writing the value back to the card.
   *
   * Note that these actions must be performed with the CAMAC branch locked
   * so that we don't get interference from other bo or mbbo devices on the
   * same card.
   */
   if (pdevInfo->merge_mask) {
      status = camacLockBranch (pdevInfo->ext);

      if (status == OK) {
         cfsa (pdevInfo->f_read, pdevInfo->ext, (int *)&value, &q);

         if (q) {
            value &= pdevInfo->merge_mask;
            value |= prec->rval;
            cfsa (pdevInfo->f_write, pdevInfo->ext, (int *)&value, &q);
         }/*end if read was successfull*/

         camacUnlockBranch (pdevInfo->ext);
         if (!q) status = ERROR;

      }/*end if lock was successfull*/
   }/*end if read/merge/write device*/

  /*---------------------
   * If this card only supports selective set/clear kinds of writes, we will
   * approximate the action of a "writeable" device by first clearing the
   * zero bits of the new state and then setting the one bits of the new
   * state.
   *
   * Note that this is not quite a true mbbo write, since the device is (very
   * briefly) commanded to an intermediate state before the desired state is
   * reached.  If this behavior is bothersome to your hardware (EPICS won't
   * see it) then you need a different kind of card.
   */
   else {
      exta[0] = exta[1] = pdevInfo->ext;	/* Set channel variables     */

      fa[0] = pdevInfo->f_clear;		/* Clear 0 bits by selective */
      data[0] = ~prec->rval & prec->mask;	/* clear of RVAL's inverse   */

      fa[1] = pdevInfo->f_write;		/* Set 1 bits by selective   */
      data[1] = prec->rval;			/* set of RVAL		     */

      cfga (fa, exta, data, qa, cb);		/* Issue the commands	     */
      ctstat (&status);				/* Check completion status   */
   }/*end if selective write device*/

  /*---------------------
   * Check for errors
   */
   if (status != OK) {
      recGblSetSevr (prec, WRITE_ALARM, INVALID_ALARM);
      return (WRITE_ALARM);
   }/*end if error writing to hardware*/

  /*---------------------
   * If a readback function code is defined, do a hardware readback,
   * otherwise, just set the readback to the command value.
   */
   if (pdevInfo->f_read >= 0) devCamac_mbbioReadback (pdevInfo, (int *)&(prec->rbv));
   else prec->rbv = prec->rval;

   return (OK);

}/*end devCamac_mbboWrite()*/

/************************************************************************/
/* devCamac_wfInitRecord () -- Intialize a CAMAC wf Record		*/
/*									*/
/************************************************************************/
GLOBAL_RTN
STATUS devCamac_wfInitRecord (struct waveformRecord *prec)
{
   int               bipolar;		/* True if device is bi-polar	    */
   register int      mask;		/* Mask for device range	    */
   int               parm_count;	/* Number of parms in OUT.parm	    */
   struct camacio   *pcamacio;		/* Ptr to camacio part of OUT filed */
   devCamacParm     *pdevCamacParm;	/* Ptr to external parameter struct */
   devInfo_wfCamac  *pdevInfo;		/* Ptr to device-dependant struct   */
   int               range1=0;		/* 1st range parameter		    */
   int               range2=0;		/* 2nd range parameter		    */
   int		     f_read;		/* Unused			    */
   int               qmode;		/* Q transfer mode		    */

  /*---------------------
   * Perform common initialization tasks:
   *
   * Check record type, allocate the device-dependant structure (pdevInfo),
   * initialize the pointer to the i/o link fields (pcamacio), and initialize
   * the CAMAC channel variable (pdevInfo->ext) for this record.
   *
   * Note that the macro aborts record processing if there is an error.
   */
   commonInitRecord (wf, inp, INP);

  /*---------------------
   * If we were called from an external device-support routine, get the
   * mask, readback function code, and bipolar flag from the external
   * parameter structure.
   */
   if (pdevCamacParm) {
      mask = pdevCamacParm->range_mask;
      bipolar = pdevCamacParm->bipolar;
      qmode = pdevCamacParm->qmode;
    }/*end if external parameter structure is present*/

  /*---------------------
   * if there is no external parameter structure, then compute the mask,
   * bipolar flag, and the readback function code from information in the
   * PARM field.
   *
   * Note:  A device is defined as bipolar when its two range values have
   * opposite signs.
   */

   else {
      parm_count = devCamac_parseParm (&range1, &range2, &f_read, 
      				&qmode, (char *)pcamacio->parm);
      if (parm_count == ERROR) commonInitAbort (wf, "Illegal PARM in OUT field");

      pdevInfo->qmode = qmode;			/* The Q transfer mode */
      if ((range1 == 0) && (range2 == 0)) mask = MAX_MASK;
      else {
         mask = devCamac_getMask (range1, range2);
         if (!mask) commonInitAbort (wf, "Illegal range parameter(s)");
      }/*end if range parameters specified*/

      bipolar = (range1^range2) & LONG_MIN;

    }/*end if no external parameter structure*/

  /*---------------------
   * If this is a bipolar device, we need to set values for the sign mask
   * and the sign extension mask.
   */
   if (bipolar) {
      pdevInfo->sign_extend = ~mask;
      pdevInfo->sign_mask = (mask + 1) >> 1;
   }/*end if bipolar device*/

  /*---------------------
   * Make sure the field type is USHORT, SHORT, ULONG or LONG
   */
   switch (prec->ftvl) {
   	case DBF_USHORT:
   	case DBF_SHORT:
   	case DBF_ULONG:
   	case DBF_LONG:
   		break;
   	default:
   		commonInitAbort (wf, "Invalid data type");
   }
   

   /*---------------------
   * Finish intializing the device-dependent information structure
   */
   pdevInfo->f = pcamacio->f;
   pdevInfo->mask = mask;

  /*---------------------
   * Record initialization complete
   */
   return (OK);

}/*end devCamac_wfInitRecord()*/

/************************************************************************/
/* devCamac_wfReadWrite () -- Waveform read/write routine		*/
/*									*/
/************************************************************************/

GLOBAL_RTN
STATUS  devCamac_wfReadWrite (struct waveformRecord *prec)
{
   register devInfo_wfCamac  *pdevInfo;	/* Ptr to device-dependant struct  */
   int cb[4] = {0, 0, 0, 0};		/* Control block		   */
   int extb[2];				/* Two element ext for cfmad	   */
   int size;				/* Transfer size		   */
   int i;				/* Loop counter			   */
   short int *short_ptr;		/* Pointer to 16 bit array	   */
   int *int_ptr;			/* Pointer to 24 bit array	   */
   
   pdevInfo = (devInfo_wfCamac *)prec->dpvt;
   cb[0] = prec->nelm;			/* Transfer count		   */
   if ((prec->ftvl == DBF_USHORT) || (prec->ftvl == DBF_SHORT))
   	size = 16;
   else
   	size = 24;
   
   switch (pdevInfo->qmode) {
      case TMOD_QSTP:
      	 if (size == 16)
            csubc(pdevInfo->f, pdevInfo->ext, prec->bptr, cb);
      	 else
            cfubc(pdevInfo->f, pdevInfo->ext, prec->bptr, cb);
         break;
      case TMOD_QRPT:
      	 if (size == 16)
            csubr(pdevInfo->f, pdevInfo->ext, prec->bptr, cb);
      	 else
            cfubr(pdevInfo->f, pdevInfo->ext, prec->bptr, cb);
         break;
      case TMOD_QSCN:
      	 /* Routine cfmad and csmad need a 2 element extb[] array. 	*/
	 /* extb[0] is the first bcna, extb[1] is the last bcna 	*/
	 /* Some hardware only uses the first value (e.g. KSC 3922) */
	 /* so we supply the same value for both 		*/
	 extb[0] = pdevInfo->ext;
	 extb[1] = pdevInfo->ext;
      	 if (size == 16)
            csmad(pdevInfo->f, extb, prec->bptr, cb);
      	 else
            cfmad(pdevInfo->f, extb, prec->bptr, cb);
         break;
   }
   prec->nord = cb[1];		/* Actual # of transfers completed 	 */
   if(cb[0] != cb[1]) {		/* If not same as # requested, error 	 */
     	recGblSetSevr (prec, WRITE_ALARM, INVALID_ALARM);
      	return (WRITE_ALARM);
   }/*end if error writing to hardware*/
   
  /*---------------------
   * If this is a CAMAC read then mask raw value for correct precision and 
   * apply sign extension if required
   */
   if (pdevInfo->f < 8) {
      if (size == 16) {
   	 short_ptr = (short *)prec->bptr;
         for (i=0; i<cb[1]; i++) {
   	    short_ptr[i] &= pdevInfo->mask;
   	    if (short_ptr[i] & pdevInfo->sign_mask) 
   	    		short_ptr[i] |= pdevInfo->sign_extend;
         } /* End for */
      } /* End size = 16 */
      else { /* Size = 24 */
   	 int_ptr = (int *)prec->bptr;
         for (i=0; i<cb[1]; i++) {
   	    int_ptr[i] &= pdevInfo->mask;
   	    if (int_ptr[i] & pdevInfo->sign_mask) 
   	    		int_ptr[i] |= pdevInfo->sign_extend;
         } /* End for */
      } /* End size = 24 */
   } /* End CAMAC read function (f < 8) */
   
   return (OK);
}/*end devCamac_wfReadWrite()*/
