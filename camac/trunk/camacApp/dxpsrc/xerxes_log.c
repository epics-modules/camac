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

/* Static variables */
char info_string[400];

/*****************************************************************************
 *
 * This routine enables the logging output
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_enable_log(void)
{
	if (dxp_md_enable_log==NULL) { 
		dxp_install_utils("NULL");
	}

	return dxp_md_enable_log();
}

/*****************************************************************************
 *
 * This routine disables the logging output
 *
 *****************************************************************************/
XERXES_EXPORT int XERXES_API dxp_suppress_log(void)
{
	if (dxp_md_suppress_log==NULL) { 
		dxp_install_utils("NULL");
	}

	return dxp_md_suppress_log();
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
	if (dxp_md_set_log_level==NULL) { 
		dxp_install_utils("NULL");
	}

	return dxp_md_set_log_level(*level);
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
	if (dxp_md_output==NULL) { 
		dxp_install_utils("NULL");
	}

	dxp_md_output(filename);

	return DXP_SUCCESS;
}

