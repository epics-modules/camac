/*
 *  xia_dxp4c2x.h
 *
 *  Created 11/30/99 JEW: internal include file.  define here, what we
 *						don't want the user to see.
 *
 *  Copyright 1999 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp4c2x.c and xerxes.c routines
 */
#ifndef XIA_X10P_H
#define XIA_X10P_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _XERXES_PROTO_							/* Begin ANSI C prototypes */
XERXES_EXPORT int XERXES_API dxp_init_dxpx10p(Functions *funcs);
XERXES_STATIC int XERXES_API dxp_init_driver(Interface *);
XERXES_STATIC int XERXES_API dxp_init_utils(Utils *);
XERXES_STATIC int XERXES_API dxp_write_tsar(int *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_write_csr(int *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_read_csr(int *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_write_data(int *, unsigned short *, unsigned int);
XERXES_STATIC int XERXES_API dxp_read_data(int *, unsigned short *, unsigned int);
XERXES_STATIC int XERXES_API dxp_write_fippi(int *, unsigned short *, unsigned int);
XERXES_STATIC int XERXES_API dxp_read_word(int *,int *,unsigned short *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_write_word(int *,int *,unsigned short *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_read_block(int *,int *,unsigned short *,unsigned int *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_write_block(int *,int *, unsigned short *,unsigned int *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_read_long(int *,int *,unsigned short *,unsigned long *);
XERXES_STATIC int XERXES_API dxp_write_long(int *,int *,unsigned short *,unsigned long *);
XERXES_STATIC int XERXES_API dxp_look_at_me(int *ioChan, int *modChan);
XERXES_STATIC int XERXES_API dxp_ignore_me(int *ioChan, int *modChan);
XERXES_STATIC int XERXES_API dxp_clear_LAM(int *ioChan, int *modChan);
XERXES_STATIC int XERXES_API dxp_read_CSR(int *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_prep_for_readout(int *, int *);
XERXES_STATIC int XERXES_API dxp_done_with_readout(int *, int *, short state[2]);
XERXES_STATIC int XERXES_API dxp_begin_run(int *, int *,unsigned short *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_end_run(int *, int *);
XERXES_STATIC int XERXES_API dxp_run_active(int *, int *, int*);
XERXES_STATIC int XERXES_API dxp_loc(char *, Dsp_Info *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_dspparam_dump(int *,int *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_begin_calibrate(int *, int *, int *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_test_mem(int *,int *,int *,unsigned int *,unsigned short *);
XERXES_STATIC int XERXES_API dxp_test_spectrum_memory(int *,int *,int *, Dsp_Info *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_test_baseline_memory(int *,int *,int *, Dsp_Info *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_test_event_memory(int *,int *,int *, Dsp_Info *, unsigned short *);

XERXES_STATIC int XERXES_API dxp_get_dspinfo(Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_get_fipinfo(Fippi_Info *);
XERXES_STATIC int XERXES_API dxp_get_defaultsinfo(Dsp_Defaults *);
XERXES_STATIC int XERXES_API dxp_get_fipconfig(Fippi_Info *);
XERXES_STATIC int XERXES_API dxp_download_fippi_done(int *, int *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_download_fipconfig(int *,int *,Fippi_Info *);

XERXES_STATIC int XERXES_API dxp_download_dspconfig(int *,int *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_download_dsp_done(int *, int *, int*, Dsp_Info *, unsigned short *, float *);
XERXES_STATIC int XERXES_API dxp_get_dspconfig(Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_get_dspdefaults(Dsp_Defaults *);
XERXES_STATIC int XERXES_API dxp_load_dspfile(FILE *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_load_dspsymbol_table(FILE *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_load_dspconfig(FILE *, Dsp_Info *);

XERXES_STATIC int XERXES_API dxp_decode_error(unsigned short [], Dsp_Info *, unsigned short *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_clear_error(int *, int *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_check_calibration(int *, unsigned short *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_get_runstats(unsigned short [],Dsp_Info *, unsigned int *,unsigned int *,
									  unsigned int *,unsigned int *,unsigned int *,double *);
XERXES_STATIC void XERXES_API dxp_swaplong(unsigned int *, unsigned long *);
XERXES_STATIC int XERXES_API dxp_symbolname(unsigned short *, Dsp_Info *, char *);

XERXES_STATIC int XERXES_API dxp_modify_dspsymbol(int *, int *, char *, unsigned short *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_write_dsp_param_addr(int *, int *, unsigned short *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_read_dspsymbol(int *, int *, char *, Dsp_Info *, unsigned long *);
XERXES_STATIC int XERXES_API dxp_read_dspparams(int *, int *, Dsp_Info *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_write_dspparams(int *, int *, Dsp_Info *, unsigned short *);

XERXES_STATIC unsigned int XERXES_API dxp_get_spectrum_length(Dsp_Info *, unsigned short *);
XERXES_STATIC unsigned int XERXES_API dxp_get_baseline_length(Dsp_Info *, unsigned short *);
XERXES_STATIC unsigned int XERXES_API dxp_get_event_length(Dsp_Info *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_read_spectrum(int *, int *, Dsp_Info *, unsigned short *, unsigned long *);
XERXES_STATIC int XERXES_API dxp_read_baseline(int *, int *, Dsp_Info *, unsigned short *, unsigned short *);
XERXES_STATIC int XERXES_API dxp_perform_gaincalc(float *,unsigned short *,short *);
XERXES_STATIC int XERXES_API dxp_change_gains(int *, int *, int *, float *,Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_setup_asc(int *, int *, int *, float *, float *, unsigned short *, 
								   float *, float *, float *, Dsp_Info *);
XERXES_STATIC int XERXES_API dxp_calibrate_asc(int *, int *, unsigned short *, Dsp_Info **);
XERXES_STATIC int XERXES_API dxp_calibrate_channel(int *, int *, unsigned short *, int *, Dsp_Info **);
XERXES_STATIC int XERXES_API dxp_little_endian(void);
XERXES_STATIC FILE* XERXES_API dxp_find_file(const char *, const char *);

XERXES_STATIC int XERXES_API dxp_get_adc(int *ioChan, int *modChan, Dsp_Info *dsp, unsigned short *data);

#else									/* Begin old style C prototypes */
XERXES_EXPORT int XERXES_API dxp_init_dxpx10p();
XERXES_STATIC int XERXES_API dxp_init_driver();
XERXES_STATIC int XERXES_API dxp_init_utils();
XERXES_STATIC int XERXES_API dxp_write_tsar();
XERXES_STATIC int XERXES_API dxp_write_csr();
XERXES_STATIC int XERXES_API dxp_read_csr();
XERXES_STATIC int XERXES_API dxp_write_data();
XERXES_STATIC int XERXES_API dxp_read_data();
XERXES_STATIC int XERXES_API dxp_write_fippi();
XERXES_STATIC int XERXES_API dxp_read_word();
XERXES_STATIC int XERXES_API dxp_write_word();
XERXES_STATIC int XERXES_API dxp_read_block();
XERXES_STATIC int XERXES_API dxp_write_block();
XERXES_STATIC int XERXES_API dxp_disable_LAM();
XERXES_STATIC int XERXES_API dxp_enable_LAM();
XERXES_STATIC int XERXES_API dxp_clear_LAM();
XERXES_STATIC int XERXES_API dxp_read_CSR();
XERXES_STATIC int XERXES_API dxp_download_fipconfig();
XERXES_STATIC int XERXES_API dxp_download_dspconfig();
XERXES_STATIC int XERXES_API dxp_download_dsp_done();
XERXES_STATIC int XERXES_API dxp_get_spectrum_length();
XERXES_STATIC int XERXES_API dxp_get_baseline_length();
XERXES_STATIC int XERXES_API dxp_get_event_length();
XERXES_STATIC int XERXES_API dxp_prep_for_readout();
XERXES_STATIC int XERXES_API dxp_done_with_readout();
XERXES_STATIC int XERXES_API dxp_begin_run();
XERXES_STATIC int XERXES_API dxp_end_run();
XERXES_STATIC int XERXES_API dxp_run_active();
XERXES_STATIC int XERXES_API dxp_loc();
XERXES_STATIC int XERXES_API dxp_dspparam_dump();
XERXES_STATIC int XERXES_API dxp_begin_calibrate();
XERXES_STATIC int XERXES_API dxp_test_mem();
XERXES_STATIC int XERXES_API dxp_test_spectrum_memory();
XERXES_STATIC int XERXES_API dxp_test_baseline_memory();
XERXES_STATIC int XERXES_API dxp_test_event_memory();

XERXES_STATIC int XERXES_API dxp_get_fipconfig();
XERXES_STATIC int XERXES_API dxp_download_fippi_done();

XERXES_STATIC int XERXES_API dxp_get_dspconfig();
XERXES_STATIC int XERXES_API dxp_get_dspdefaults();
XERXES_STATIC int XERXES_API dxp_load_dspfile();
XERXES_STATIC int XERXES_API dxp_load_dspsymbol_table();
XERXES_STATIC int XERXES_API dxp_load_dspconfig();

XERXES_STATIC int XERXES_API dxp_decode_error();
XERXES_STATIC int XERXES_API dxp_clear_error();
XERXES_STATIC int XERXES_API dxp_check_calibration();
XERXES_STATIC int XERXES_API dxp_get_runstats();
XERXES_STATIC void XERXES_API dxp_swaplong();
XERXES_STATIC int XERXES_API dxp_symbolname();
XERXES_STATIC int XERXES_API dxp_modify_dspsymbol();
XERXES_STATIC int XERXES_API dxp_write_dsp_param_addr();
XERXES_STATIC int XERXES_API dxp_read_dspsymbol();
XERXES_STATIC int XERXES_API dxp_read_dspparams();
XERXES_STATIC int XERXES_API dxp_write_dspparams();
XERXES_STATIC int XERXES_API dxp_read_spectrum();
XERXES_STATIC int XERXES_API dxp_read_baseline();
XERXES_STATIC int XERXES_API dxp_perform_gaincalc();
XERXES_STATIC int XERXES_API dxp_change_gains();
XERXES_STATIC int XERXES_API dxp_setup_asc();
XERXES_STATIC int XERXES_API dxp_calibrate_asc();
XERXES_STATIC int XERXES_API dxp_calibrate_channel();
XERXES_STATIC int XERXES_API dxp_read_long();
XERXES_STATIC int XERXES_API dxp_write_long();
XERXES_STATIC int XERXES_API dxp_little_endian();
XERXES_STATIC FILE* XERXES_API dxp_find_file();

XERXES_STATIC int XERXES_API dxp_get_adc();

#endif                                  /*   end if _XERXES_PROTO_ */

#ifdef __cplusplus
}
#endif

#endif						/* Endif for XIA_X10P_H */
