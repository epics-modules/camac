/*<##Wed Apr  3 17:20:53 2002--COUGAR--Do not remove--XIA##>*/

/*
 * xerxes_log.c
 *
 * Routines used for controlling the logging functionality
 * in XerXes.
 *
 * Created 12/3/01 -- JEW
 *
 * Copyright (c) 2001
 * X-ray Instrumentation Associates
 * All rights reserved
 *
 */

#include <stdio.h>

#include "xerxes_generic.h"
#include "xerxesdef.h"
#include "xia_xerxes.h"
#include "xerxes_errors.h"
#include "md_generic.h"

#define INFO_LEN	400

/* Static variables */

/*****************************************************************************
 *
 * This routine enables the logging output
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_enable_log(void)
{
	if (xerxes_md_enable_log==NULL) { 
		dxp_install_utils("NULL");
	}

	return xerxes_md_enable_log();
}

/*****************************************************************************
 *
 * This routine disables the logging output
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_suppress_log(void)
{
	if (xerxes_md_suppress_log==NULL) { 
		dxp_install_utils("NULL");
	}

	return xerxes_md_suppress_log();
}

/*****************************************************************************
 *
 * This routine sets the maximum level at which log messages will be 
 * displayed.
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_set_log_level(int *level)
/* int *level;							Input: Level to set the logging to   */
{
	if (xerxes_md_set_log_level==NULL) { 
		dxp_install_utils("NULL");
	}

	return xerxes_md_set_log_level(*level);
}

/*****************************************************************************
 *
 * This routine sets the output stream for the logging routines. By default,
 * the output is sent to stdout.
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_set_log_output(char *filename)
/* char *filename;					Input: name of file to redirect reporting */
{

	if (xerxes_md_output==NULL) {

		printf("xerxes_md_output == NULL");
		dxp_install_utils("NULL");
	}

	xerxes_md_output(filename);

	return DXP_SUCCESS;
}
