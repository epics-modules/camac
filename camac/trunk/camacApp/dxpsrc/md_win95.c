/*<##Wed Apr  3 17:20:53 2002--COUGAR--Do not remove--XIA##>*/

/*****************************************************************************
 *
 *  md_win95.c
 *
 *  Created 06-Oct-1997 Ed Oltman
 *	Add EPP support	01/29/01 JW
 *
 *  Copywrite 1997 X-ray Instrumentaton Associates
 *  All rights reserved
 *
 *    This file contains the interface between the Jorway 73A CAMAC SCSI
 *    interface Dynamic Link Library and the DXP primitive calls.
 */
/* System include files */
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
/* XIA include files */
#include "xerxes_errors.h"
#include "xerxes_structures.h"
#include "md_windows.h"
#include "md_generic.h"
#include "xia_md.h"
#define MODE 4

/* variables to store the IO channel information */
static char *name[MAXMOD];
/* total number of modules in the system */
static unsigned int numDXP=0,numEPP=0,numMod=0;
/* error string used as a place holder for calls to dxp_md_error() */
static char error_string[132];
/* are we in debug mode? */
static int print_debug=0;
/* maximum number of words able to transfer in a single call to dxp_md_io() */
static unsigned int maxblk=0;


#if !(defined(EXCLUDE_DXP4C) && defined(EXCLUDE_DXP4C2X))
/* typedef the actual DLL function call to be a pointer to a function returning a 
 * long integer */
/*typedef long (*MYDLLFUNC)(short*,short,long,short,short*);*/
/* define the variable that will point at the DLL itself */
/*static struct HINSTANCE__* camLibrary=NULL;*/
/* declare the pointer to the function used to access the CAMAC crate */
/*static MYDLLFUNC camTransferFunc=NULL;*/
#endif

#ifndef EXCLUDE_DXPX10P
/* EPP definitions */
static unsigned short port,eport,dport,sport,aport,cport;
static unsigned short next_addr=0;
#endif

/******************************************************************************
 *
 * Routine to create pointers to the MD utility routines
 * 
 ******************************************************************************/
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_util(Xia_Util_Functions* funcs, char* type)
/* Xia_Util_Functions *funcs;	*/
/* char *type;					*/
{
  /* Legacy "type" argument should be used to keep the compiler happy */
  char *dummy = NULL;

  dummy = type;


	funcs->dxp_md_error_control = dxp_md_error_control;
	funcs->dxp_md_alloc         = dxp_md_alloc;
	funcs->dxp_md_free          = dxp_md_free;
	funcs->dxp_md_puts          = dxp_md_puts;
	funcs->dxp_md_wait          = dxp_md_wait;

	funcs->dxp_md_error         = dxp_md_error;
	funcs->dxp_md_warning       = dxp_md_warning;
	funcs->dxp_md_info          = dxp_md_info;
	funcs->dxp_md_debug         = dxp_md_debug;
	funcs->dxp_md_output        = dxp_md_output;
	funcs->dxp_md_suppress_log  = dxp_md_suppress_log;
	funcs->dxp_md_enable_log    = dxp_md_enable_log;
	funcs->dxp_md_set_log_level = dxp_md_set_log_level;
	funcs->dxp_md_log	        = dxp_md_log;

	if (out_stream == NULL)
	{
		out_stream = stdout;
	}

	return DXP_SUCCESS;
}

/******************************************************************************
 *
 * Routine to create pointers to the MD utility routines
 * 
 ******************************************************************************/
XIA_MD_EXPORT int XIA_MD_API dxp_md_init_io(Xia_Io_Functions* funcs, char* type)
/* Xia_Io_Functions *funcs;		*/
/* char *type;					*/
{
	unsigned int i;
	
	for(i=0;i<strlen(type);i++) type[i]=(char) tolower(type[i]);
#if !(defined(EXCLUDE_DXP4C) && defined(EXCLUDE_DXP4C2X))
	if (STREQ(type,"camac")) {
		funcs->dxp_md_io         = dxp_md_io;
		funcs->dxp_md_initialize = dxp_md_initialize;
		funcs->dxp_md_open       = dxp_md_open;
	} 
#endif
#ifndef EXCLUDE_DXPX10P
	if (STREQ(type,"epp")) {
		funcs->dxp_md_io         = dxp_md_epp_io;
		funcs->dxp_md_initialize = dxp_md_epp_initialize;
		funcs->dxp_md_open       = dxp_md_epp_open;
	}
#endif
	funcs->dxp_md_get_maxblk = dxp_md_get_maxblk;
	funcs->dxp_md_set_maxblk = dxp_md_set_maxblk;
	funcs->dxp_md_lock_resource = NULL;


	return DXP_SUCCESS;
}

#if !(defined(EXCLUDE_DXP4C) && defined(EXCLUDE_DXP4C2X))
/*****************************************************************************
 * 
 * Initialize the system.  Alloocate the space for the library arrays, define
 * the pointer to the CAMAC library and the IO routine.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_initialize(unsigned int* maxMod, char* dllname)
/* unsigned int *maxMod;					Input: maximum number of dxp modules allowed */
/* char *dllname;							Input: name of the DLL						*/
{
	int status = DXP_SUCCESS; 
	int lstatus = -1;
	int len;
	
	short buf[256];

	/* "dllname" argument should be used to keep the compiler happy.
	 * We may choose to expand the functionality of the library
	 * to use this in the future...
	 */
	len = strlen(dllname);


	/* Initialize the CAMAC interface */
		
/* check if all the memory was allocated */
    if (*maxMod>MAXMOD){
		status = DXP_NOMEM;
		sprintf(error_string,"Calling routine requests %d maximum modules: only %d available.", 
			*maxMod, MAXMOD);
        dxp_md_log_error("dxp_md_initialize", error_string, status);
        return status;
	}

/* Zero out the number of modules currently in the system */
	numDXP = 0;
	numMod = 0;

/* Initialize the camac-SCSI interface */
	lstatus = xia_caminit(buf);
    if (lstatus!=0){
        sprintf(error_string,"camxfr error: status = 0x%lX, while trying to initialize DLL",lstatus);
        dxp_md_log_error("dxp_md_initialize",error_string,status);
        status = DXP_MDINITIALIZE;
        return status;
    }
	
	return status;
}
/*****************************************************************************
 * 
 * Routine is passed the user defined configuration string *name.  This string
 * contains all the information needed to point to the proper IO channel by 
 * future calls to dxp_md_io().  In the case of a simple CAMAC crate, the string 
 * should contain a branch number (representing the SCSI bus number in this case),
 * a crate number (single SCSI can control multiple crates) and a station 
 * number (better known as a slot number).
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_open(char* ioname, int* camChan)
/* char *ioname;						Input:  string used to specify this IO 
												channel */
/* int *camChan;						Output: returns a reference number for
												this module */
{
    unsigned int i;
	int status=DXP_SUCCESS;

/* First loop over the existing names to make sure this module 
 * was not already configured?  Don't want/need to perform
 * this operation 2 times. */
    
	for(i=0;i<numDXP;i++){
        if(STREQ(name[i],ioname)) {
            status=DXP_SUCCESS;
			*camChan = i;
			return status;
        }
    }

/* Got a new one.  Increase the number existing and assign the global 
 * information */

	if (name[numMod]!=NULL) {
		dxp_md_free(name[numMod]);
	}
	name[numMod] = (char *) dxp_md_alloc((strlen(ioname)+1)*sizeof(char));
	strcpy(name[numMod],ioname);

    *camChan = numMod++;
	numDXP++;

	return status;
}

/*****************************************************************************
 * 
 * This routine performs the IO call to read or write data.  The pointer to 
 * the desired IO channel is passed as camChan.  The address to write to is
 * specified by function and address.  The data length is specified by 
 * length.  And the data itself is stored in data.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_io(int* camChan, unsigned int* function, 
					 unsigned int* address, unsigned short* data,
					 unsigned int* length)
/* int *camChan;						Input: pointer to IO channel to access		*/
/* unsigned int *function;				Input: function number to access (CAMAC F)	*/
/* unsigned int *address;				Input: address to access	(CAMAC A)		*/
/* unsigned short *data;				I/O:  data read or written					*/
/* unsigned int *length;				Input: how much data to read or write		*/
{

    short camadr[4];
    long lstatus=0;
    int status;
	int branch, crate, station;
/* mode defines Q-stop, etc... types of transfer */
    short mode=MODE;

    short func = (short) *function;
    unsigned long  nbytes;

	sscanf(name[*camChan],"%1d%1d%2d",&branch,&crate,&station);

/* prelimenary definitions used by the DLL routine */
    nbytes=2*(*length);
    camadr[0] = (short) branch;
    camadr[1] = (short) crate;
    camadr[2] = (short) station;
    camadr[3] = (short) *address;

/* Now perform the read or write operation using the routine */

	lstatus = xia_camxfr(camadr, func, nbytes, mode, (short*) data);
    if ((lstatus!=0)&&(lstatus!=4)){
        status = DXP_MDIO;
        sprintf(error_string,"camxfr error: status = 0x%lX",lstatus);
        dxp_md_log_error("dxp_md_io",error_string,status);
        return status;
    }

/* All done, free the library and get out */
	status=DXP_SUCCESS;

    return status;
}
#endif
#ifndef EXCLUDE_DXPX10P
/*****************************************************************************
 * 
 * Initialize the system.  Alloocate the space for the library arrays, define
 * the pointer to the CAMAC library and the IO routine.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_initialize(unsigned int* maxMod, char* dllname)
/* unsigned int *maxMod;					Input: maximum number of dxp modules allowed */
/* char *dllname;							Input: name of the DLL						*/
{
	int status = DXP_SUCCESS;
	int rstat = 0;

/* EPP initialization */

/* check if all the memory was allocated */
    if (*maxMod>MAXMOD){
		status = DXP_NOMEM;
		sprintf(error_string,"Calling routine requests %d maximum modules: only %d available.", 
			*maxMod, MAXMOD);
        dxp_md_log_error("dxp_md_epp_initialize",error_string,status);
        return status;
	}

/* Zero out the number of modules currently in the system */
	numEPP = 0;

/* Initialize the EPP port */
	rstat = sscanf(dllname,"%x",&port);
	if (rstat!=1) {
		status = DXP_NOMATCH;
        dxp_md_log_error("dxp_md_epp_initialize",
			"Unable to read the EPP port address",status);
        return status;
	}
													 
	sprintf(error_string, "EPP Port = %#x", port);
	dxp_md_log_debug("dxp_md_epp_initialize", error_string);

/* Call the EPPLIB.DLL routine */	
	rstat = DxpInitEPP((int)port);

/* Check for Success */
	if (rstat==0) {
		status = DXP_SUCCESS;
	} else {
		status = DXP_INITIALIZE;
		sprintf(error_string,
			"Unable to initialize the EPP port: rstat=%d",rstat);
        dxp_md_log_error("dxp_md_epp_initialize", error_string,status);
        return status;
	}

	return status;
}
/*****************************************************************************
 * 
 * Routine is passed the user defined configuration string *name.  This string
 * contains all the information needed to point to the proper IO channel by 
 * future calls to dxp_md_io().  In the case of a simple CAMAC crate, the string 
 * should contain a branch number (representing the SCSI bus number in this case),
 * a crate number (single SCSI can control multiple crates) and a station 
 * number (better known as a slot number).
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_open(char* ioname, int* camChan)
/* char *ioname;							Input:  string used to specify this IO 
												channel */
/* int *camChan;						Output: returns a reference number for
												this module */
{
    unsigned int i;
	int status=DXP_SUCCESS;

/* First loop over the existing names to make sure this module 
 * was not already configured?  Don't want/need to perform
 * this operation 2 times. */
    
	for(i=0;i<numDXP;i++){
        if(STREQ(name[numMod],ioname)) {
            status=DXP_SUCCESS;
			*camChan = i;
			return status;
        }
    }

/* Got a new one.  Increase the number existing and assign the global 
 * information */

	if (name[numMod]!=NULL) {
		dxp_md_free(name[numMod]);
	}
	name[numMod] = (char *) dxp_md_alloc((strlen(ioname)+1)*sizeof(char));
	strcpy(name[numMod],ioname);

    *camChan = numMod++;
	numEPP++;

	return status;
}

/*****************************************************************************
 * 
 * This routine performs the IO call to read or write data.  The pointer to 
 * the desired IO channel is passed as camChan.  The address to write to is
 * specified by function and address.  The data length is specified by 
 * length.  And the data itself is stored in data.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_epp_io(int* camChan, unsigned int* function, 
					 unsigned int* address, unsigned short* data,
					 unsigned int* length)
/* int *camChan;						Input: pointer to IO channel to access		*/
/* unsigned int *function;				Input: XIA EPP function definition			*/
/* unsigned int *address;				Input: XIA EPP address definition			*/
/* unsigned short *data;				I/O:  data read or written					*/
/* unsigned int *length;				Input: how much data to read or write		*/
{
	int rstat = 0; 
	int status;
	int garbage;

	unsigned int i;

	unsigned long *temp=NULL;

	garbage = *camChan;

/* Data*/
	if (*address==0) {
/* Perform short reads and writes if not in program address space */
		if (next_addr>=0x4000) {
			if (*length>1) {
				if (*function==0) {
					rstat = DxpReadBlock(next_addr, data, (int) *length);
				} else {
					rstat = DxpWriteBlock(next_addr, data, (int) *length);
				}
			} else {
				if (*function==0) {
					rstat = DxpReadWord(next_addr, data);
				} else {
					rstat = DxpWriteWord(next_addr, data[0]);
				}
			}
		} else {
/* Perform long reads and writes if in program address space (24-bit) */
/* Allocate memory */
			temp = (unsigned long *) dxp_md_alloc(sizeof(unsigned short)*(*length));
			if (*function==0) {
				rstat = DxpReadBlocklong(next_addr, temp, (int) *length/2);
/* reverse the byte order for the EPPLIB library */
				for (i=0;i<*length/2;i++) {
					data[2*i] = (unsigned short) (temp[i]&0xFFFF);
					data[2*i+1] = (unsigned short) ((temp[i]>>16)&0xFFFF);
				}
			} else {
/* reverse the byte order for the EPPLIB library */
				for (i=0;i<*length/2;i++) {
					temp[i] = ((data[2*i]<<16) + data[2*i+1]);
				}
				rstat = DxpWriteBlocklong(next_addr, temp, (int) *length/2);
			}
/* Free the memory */
			dxp_md_free(temp);
		}
/* Address port*/
	} else if (*address==1) {
		next_addr = (unsigned short) *data;
/* Control port*/
	} else if (*address==2) {
/*		dest = cport;
		*length = 1;
 */
/* Status port*/
	} else if (*address==3) {
/*		dest = sport;
		*length = 1;*/
	} else {
        sprintf(error_string,"Unknown EPP address=%d",*address);
		status = DXP_MDIO;
        dxp_md_log_error("dxp_md_epp_io",error_string,status);
        return status;
	}

	if (rstat!=0) {
		status = DXP_MDIO;
		sprintf(error_string,"Problem Performing I/O to Function: %d, address: %#hx",*function, *address);
		dxp_md_log_error("dxp_md_epp_io",error_string,status);
		sprintf(error_string,"Trying to write to internal address: %d, length %d",next_addr, *length);
		dxp_md_log_error("dxp_md_epp_io",error_string,status);
		return status;
	}
	
	status=DXP_SUCCESS;

    return status;
}
#endif
/*****************************************************************************
 * 
 * Routine to control the error reporting operation.  Currently the only 
 * keyword available is "print_debug".  Then whenever dxp_md_error() is called
 * with an error_code=DXP_DEBUG, the message is printed.  
 * 
 *****************************************************************************/
XIA_MD_STATIC void XIA_MD_API dxp_md_error_control(char* keyword, int* value)
/* char *keyword;						Input: keyword to set for future use by dxp_md_error()	*/
/* int *value;							Input: value to set the keyword to					*/
{

/* Enable debugging */

    if (strstr(keyword,"print_debug")!=NULL){
        print_debug=(*value);
        return;
    }

/* Else we have a problem */

    dxp_md_puts("dxp_md_error_control: keyword %s not recognized..\n");
}

/*****************************************************************************
 * 
 * Routine to get the maximum number of words that can be block transfered at 
 * once.  This can change from system to system and from controller to 
 * controller.  A maxblk size of 0 means to write all data at once, regardless 
 * of transfer size.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_get_maxblk(void)
{

    return maxblk;

}

/*****************************************************************************
 * 
 * Routine to set the maximum number of words that can be block transfered at 
 * once.  This can change from system to system and from controller to 
 * controller.  A maxblk size of 0 means to write all data at once, regardless 
 * of transfer size.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_set_maxblk(unsigned int* blksiz)
/* unsigned int *blksiz;			*/
{
	int status; 

	if (*blksiz > 0) {
		maxblk = *blksiz;
	} else {
        status = DXP_NEGBLOCKSIZE;
        dxp_md_log_error("dxp_md_set_maxblk","Block size must be positive or zero",status);
        return status;
	}
    return DXP_SUCCESS;

}

/*****************************************************************************
 * 
 * Routine to wait a specified time in seconds.  This allows the user to call
 * routines that are as precise as required for the purpose at hand.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_wait(float* time)
/* float *time;							Input: Time to wait in seconds	*/
{
	int status=DXP_SUCCESS;

	Sleep((DWORD) (1000.*(*time)));

	return status;

}

/*****************************************************************************
 * 
 * Routine to allocate memory.  The calling structure is the same as the 
 * ANSI C standard routine malloc().  
 * 
 *****************************************************************************/
XIA_MD_SHARED void* XIA_MD_API dxp_md_alloc(size_t length)
/* size_t length;							Input: length of the memory to allocate
													in units of size_t (defined to be a 
													byte on most systems) */
{

	return malloc(length);

}

/*****************************************************************************
 * 
 * Routine to free memory.  Same calling structure as the ANSI C routine 
 * free().
 * 
 *****************************************************************************/
XIA_MD_SHARED void XIA_MD_API dxp_md_free(void* array)
/* void *array;							Input: pointer to the memory to free */
{

	free(array);

}

/*****************************************************************************
 * 
 * Routine to print a string to the screen or log.  No direct calls to 
 * printf() are performed by XIA libraries.  All output is directed either
 * through the dxp_md_error() or dxp_md_puts() routines.
 * 
 *****************************************************************************/
XIA_MD_STATIC int XIA_MD_API dxp_md_puts(char* s)
/* char *s;								Input: string to print or log	*/
{

	return printf("%s", s);

}

