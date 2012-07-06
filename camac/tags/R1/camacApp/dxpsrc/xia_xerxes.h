/*
 *  xia_xerxes.h
 *
 *  Modified 2-Feb-97 EO: add prototype for dxp_primitive routines
 *      dxp_read_long and dxp_write_long; added various parameters
 *  Major Mods 3-17-00 JW: Complete revamping of libraries
 *
 *  Copyright 1996 X-ray Instrumentation Associates
 *  All rights reserved
 *
 *
 *    Following are prototypes for dxp driver routines
 */
#ifndef XIA_XERXES_H
#define XIA_XERXES_H

#ifndef XERXESDEF_H
#include <xerxesdef.h>
#endif

#define CODE_VERSION					   0
#define CODE_REVISION		  			   6
/*
 *    
 */
#define MAXDET				500
#define MAXSYM              500
#define MAXSYMBOL_LEN        13
#define MAXFILENAME_LEN     200
#define MAXDXP				100		/* Maximum number of DXP modules in system */
#define MAXBOARDNAME_LEN	 20
/*
 *    CAMAC status Register control codes
 */
#define ALLCHAN              -1

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _XERXES_PROTO_
#include <stdio.h>

/*
 * following are internal prototypes for xerxes.c routines
 */
XERXES_EXPORT int XERXES_API dxp_initialize(void);
XERXES_EXPORT int XERXES_API dxp_init_ds(void);
XERXES_EXPORT int XERXES_API dxp_init_boards_ds(void);
XERXES_EXPORT int XERXES_API dxp_init_library(void);
XERXES_EXPORT int XERXES_API dxp_install_utils(const char *utilname);
XERXES_EXPORT int XERXES_API dxp_read_config(char *cname);
XERXES_EXPORT int XERXES_API dxp_add_system_item(char *token, char **values);
XERXES_EXPORT int XERXES_API dxp_add_board_item(char *token, char **values);
XERXES_EXPORT int XERXES_API dxp_assign_channel(void);
XERXES_EXPORT int XERXES_API dxp_user_setup(void);
XERXES_EXPORT int XERXES_API dxp_reset_channel(int *detChan);
XERXES_EXPORT int XERXES_API dxp_reset_dspconfig(int *detChan);
XERXES_EXPORT int XERXES_API dxp_reset_fipconfig(int *detChan);
XERXES_EXPORT int XERXES_API dxp_del_board(char *type, char *iolib, char *ifacelib, 
								   char *iostring);
XERXES_EXPORT int XERXES_API dxp_del_btype(char *name);
XERXES_EXPORT int XERXES_API dxp_del_dsp(char *filename);
XERXES_EXPORT int XERXES_API dxp_del_fippi(char *filename);
XERXES_EXPORT int XERXES_API dxp_del_defaults(char *filename);
XERXES_EXPORT int XERXES_API dxp_add_board(char *type, char *iolib, char *ifacelib,
								   char *iostring, unsigned short *nchan, int *detChan);
XERXES_EXPORT int XERXES_API dxp_add_btype(char *name, char *pointer, char *dllname);
XERXES_EXPORT int XERXES_API dxp_get_board_type(int *detChan, char *name);
XERXES_EXPORT int XERXES_API dxp_enable_LAM(int *detChan);
XERXES_EXPORT int XERXES_API dxp_disable_LAM(int *detChan);
XERXES_EXPORT int XERXES_API dxp_reset_LAM(int *detChan);
XERXES_EXPORT int XERXES_API dxp_start_run(unsigned short *gate, unsigned short *resume);
XERXES_EXPORT int XERXES_API dxp_resume_run();
XERXES_EXPORT int XERXES_API dxp_stop_run(void);
XERXES_EXPORT int XERXES_API dxp_start_one_run(int *detChan, unsigned short *gate, unsigned short *resume);
XERXES_EXPORT int XERXES_API dxp_resume_one_run(int *detChan);
XERXES_EXPORT int XERXES_API dxp_stop_one_run(int *detChan);
XERXES_EXPORT int XERXES_API dxp_isrunning(int *detChan, int *active);
XERXES_EXPORT int XERXES_API dxp_isrunning_any(int *detChan, int *active);
XERXES_EXPORT int XERXES_API dxp_readout_detector_run(int *, unsigned short [], unsigned short [], unsigned long []);
XERXES_EXPORT int XERXES_API dxp_write_spectra(int *,int *);
XERXES_EXPORT int XERXES_API dxp_dspdefaults(int *);
XERXES_EXPORT int XERXES_API dxp_dspconfig(void);
XERXES_EXPORT int XERXES_API dxp_fipconfig(void);
XERXES_EXPORT int XERXES_API dxp_replace_fipconfig(int *, char *);
XERXES_EXPORT int XERXES_API dxp_replace_dspconfig(int *, char *);
XERXES_EXPORT int XERXES_API dxp_put_dspparams(void);
XERXES_EXPORT int XERXES_API dxp_replace_dspparams(int *);
XERXES_EXPORT int XERXES_API dxp_dspparams(char *);
XERXES_EXPORT int XERXES_API dxp_upload_dspparams(int *);
XERXES_EXPORT int XERXES_API dxp_get_symbol_index(int* detChan, char* name, unsigned short* index);
XERXES_EXPORT int XERXES_API dxp_set_dspsymbol(char *, unsigned short *);
XERXES_EXPORT int XERXES_API dxp_set_one_dspsymbol(int *,char *, unsigned short *);
XERXES_EXPORT int XERXES_API dxp_get_one_dspsymbol(int *,char *, unsigned short *);
XERXES_EXPORT int XERXES_API dxp_get_dspsymbol(int *,char *, unsigned long *);
XERXES_EXPORT int XERXES_API dxp_download_params(int *, unsigned short [], unsigned short []);
XERXES_EXPORT int XERXES_API dxp_download_one_params(int *, int *,unsigned short [], unsigned short []);
XERXES_EXPORT int XERXES_API dxp_upload_channel(int *, unsigned short *,unsigned short *, unsigned long *);
XERXES_EXPORT int XERXES_API dxp_nspec(int *, unsigned int *);
XERXES_EXPORT int XERXES_API dxp_nbase(int *, unsigned int *);
XERXES_EXPORT int XERXES_API dxp_nevent(int *, unsigned int *);
XERXES_EXPORT int XERXES_API dxp_ndxp(int *);
XERXES_EXPORT int XERXES_API dxp_ndxpchan(int *);
XERXES_EXPORT int XERXES_API dxp_max_symbols(int *, unsigned short *);
XERXES_EXPORT int XERXES_API dxp_symbolname_list(int *, char **);
XERXES_EXPORT int XERXES_API dxp_symbolname_by_index(int *, unsigned short *, char *);
XERXES_EXPORT int XERXES_API dxp_symbolname_limits(int *, unsigned short *, unsigned short *, unsigned short *);
XERXES_EXPORT int XERXES_API dxp_check_decimation(unsigned short *,unsigned short *);
XERXES_EXPORT int XERXES_API dxp_calibrate(int *calibtask);
XERXES_EXPORT int XERXES_API dxp_calibrate_one_channel(int *detChan, int *calibtask);
XERXES_EXPORT int XERXES_API dxp_findpeak(long [],int *,float *,int *,int *);
XERXES_EXPORT int XERXES_API dxp_fitgauss0(long [],int *,int *,float *,float *);
XERXES_EXPORT int XERXES_API dxp_save_config(int *);
XERXES_EXPORT int XERXES_API dxp_restore_config(int *);
XERXES_EXPORT int XERXES_API dxp_save_dspparams(int *, int *);
XERXES_EXPORT int XERXES_API dxp_open_file(int *,char *,int *);
XERXES_EXPORT int XERXES_API dxp_close_file(int *);
XERXES_EXPORT int XERXES_API dxp_get_electronics(int *,int [],int []);
XERXES_EXPORT int XERXES_API dxp_get_detectors(int *,int []);
XERXES_EXPORT int XERXES_API dxp_modify_gains(float *);
XERXES_EXPORT int XERXES_API dxp_modify_one_gains(int *, float *);
XERXES_EXPORT int XERXES_API dxp_initialize_ASC(float *,float *);
XERXES_EXPORT int XERXES_API dxp_det_to_elec(int *,Board **,int *);
XERXES_EXPORT int XERXES_API dxp_elec_to_det(int *,int *,int *);
XERXES_EXPORT int XERXES_API dxp_mem_dump(int *);
XERXES_EXPORT int XERXES_API dxp_get_statistics(int *, double *, double *, double *, unsigned long *);
XERXES_EXPORT int XERXES_API dxp_replace_filesymbol(char *, char *);
XERXES_EXPORT int XERXES_API dxp_read_filesymbol(char *, char *);
XERXES_EXPORT void XERXES_API dxp_version(void);
XERXES_EXPORT int XERXES_API dxp_locate_system_files(unsigned int *, char **);
XERXES_EXPORT int XERXES_API dxp_locate_channel_files(int *, unsigned int *, char **);
XERXES_EXPORT int XERXES_API dxp_lock_resource(int *detChan, short *lock);
XERXES_EXPORT int XERXES_API dxp_read_adc(int *detChan, unsigned short *data);

XERXES_STATIC int XERXES_API dxp_get_btype(char *name, Board_Info **current);
XERXES_STATIC int XERXES_API dxp_add_btype_library(Board_Info *current);
XERXES_STATIC int XERXES_API dxp_init_iface_ds(void);
XERXES_STATIC int XERXES_API dxp_init_dsp_ds(void);
XERXES_STATIC int XERXES_API dxp_init_fippi_ds(void);
XERXES_STATIC int XERXES_API dxp_init_defaults_ds(void);
XERXES_STATIC int XERXES_API dxp_read_modules(FILE *);
XERXES_STATIC int XERXES_API dxp_free_board(Board *board);
XERXES_STATIC int XERXES_API dxp_free_iface(Interface *iface);
XERXES_STATIC int XERXES_API dxp_free_dsp(Dsp_Info *dsp);
XERXES_STATIC int XERXES_API dxp_free_fippi(Fippi_Info *fippi);
XERXES_STATIC int XERXES_API dxp_free_params(Dsp_Params *params);
XERXES_STATIC int XERXES_API dxp_free_defaults(Dsp_Defaults *defaults);
XERXES_STATIC int XERXES_API dxp_free_binfo(Board_Info *binfo);
XERXES_STATIC int XERXES_API dxp_add_fippi(char *, Board_Info *, Fippi_Info **);
XERXES_STATIC int XERXES_API dxp_add_dsp(char *, Board_Info *, Dsp_Info **);
XERXES_STATIC int XERXES_API dxp_add_defaults(char *, Board_Info *, Dsp_Defaults **);
XERXES_STATIC int XERXES_API dxp_add_iface(char *dllname, char *iolib, Interface **iface);
XERXES_STATIC int XERXES_API dxp_pick_filename(unsigned int, char *, char *);
XERXES_STATIC int XERXES_API dxp_strnstrm(char *, char *, unsigned int *, unsigned int *);
XERXES_STATIC int XERXES_API dxp_readout_run(Board *, int *, unsigned short [], unsigned short [], unsigned long []);
XERXES_STATIC int XERXES_API dxp_do_readout(Board *, int *, unsigned short [], unsigned short [], unsigned long []);
XERXES_STATIC FILE* XERXES_API dxp_find_file(const char *, const char *, char [MAXFILENAME_LEN]);

XERXES_IMPORT int DXP_API dxp_init_dxp4c2x(Functions *funcs);
XERXES_IMPORT int DXP_API dxp_init_dxp4c(Functions *funcs);
XERXES_IMPORT int DXP_API dxp_init_dxpx10p(Functions *funcs);
XERXES_IMPORT int MD_API dxp_md_init_util(Xia_Util_Functions *funcs, char *type);
XERXES_IMPORT int MD_API dxp_md_init_io(Xia_Io_Functions *funcs, char *type);

#else									/* Begin old style C prototypes */
/*
 * following are internal prototypes for xerxes.c routines
 */
XERXES_EXPORT int XERXES_API dxp_initialize();
XERXES_EXPORT int XERXES_API dxp_init_ds();
XERXES_EXPORT int XERXES_API dxp_init_boards_ds();
XERXES_EXPORT int XERXES_API dxp_init_library();
XERXES_EXPORT int XERXES_API dxp_install_utils();
XERXES_EXPORT int XERXES_API dxp_read_config();
XERXES_EXPORT int XERXES_API dxp_add_system_item();
XERXES_EXPORT int XERXES_API dxp_add_boards_item();
XERXES_EXPORT int XERXES_API dxp_install_utils();
XERXES_EXPORT int XERXES_API dxp_init_ds();
XERXES_EXPORT int XERXES_API dxp_assign_channel();
XERXES_EXPORT int XERXES_API dxp_user_setup();
XERXES_EXPORT int XERXES_API dxp_reset_channel();
XERXES_EXPORT int XERXES_API dxp_reset_dspconfig();
XERXES_EXPORT int XERXES_API dxp_reset_fipconfig();
XERXES_EXPORT int XERXES_API dxp_add_board();
XERXES_EXPORT int XERXES_API dxp_add_btype();
XERXES_EXPORT int XERXES_API dxp_get_board_type();
XERXES_EXPORT int XERXES_API dxp_enable_LAM();
XERXES_EXPORT int XERXES_API dxp_disable_LAM();
XERXES_EXPORT int XERXES_API dxp_reset_LAM();
XERXES_EXPORT int XERXES_API dxp_start_run();
XERXES_EXPORT int XERXES_API dxp_resume_run();
XERXES_EXPORT int XERXES_API dxp_stop_run();
XERXES_EXPORT int XERXES_API dxp_start_one_run();
XERXES_EXPORT int XERXES_API dxp_resume_one_run();
XERXES_EXPORT int XERXES_API dxp_stop_one_run();
XERXES_EXPORT int XERXES_API dxp_isrunning();
XERXES_EXPORT int XERXES_API dxp_isrunning_any();
XERXES_EXPORT int XERXES_API dxp_readout_detector_run();
XERXES_EXPORT int XERXES_API dxp_write_spectra();
XERXES_EXPORT int XERXES_API dxp_dspconfig();
XERXES_EXPORT int XERXES_API dxp_dspdefaults();
XERXES_EXPORT int XERXES_API dxp_fipconfig();
XERXES_EXPORT int XERXES_API dxp_replace_fipconfig();
XERXES_EXPORT int XERXES_API dxp_replace_dspconfig();
XERXES_EXPORT int XERXES_API dxp_put_dspparams();
XERXES_EXPORT int XERXES_API dxp_replace_dspparams();
XERXES_EXPORT int XERXES_API dxp_dspparams();
XERXES_EXPORT int XERXES_API dxp_upload_dspparams();
XERXES_EXPORT int XERXES_API dxp_get_symbol_index();
XERXES_EXPORT int XERXES_API dxp_set_dspsymbol();
XERXES_EXPORT int XERXES_API dxp_set_one_dspsymbol();
XERXES_EXPORT int XERXES_API dxp_get_one_dspsymbol();
XERXES_EXPORT int XERXES_API dxp_get_dspsymbol();
XERXES_EXPORT int XERXES_API dxp_download_params();
XERXES_EXPORT int XERXES_API dxp_download_one_params();
XERXES_EXPORT int XERXES_API dxp_upload_channel();
XERXES_EXPORT int XERXES_API dxp_nspec();
XERXES_EXPORT int XERXES_API dxp_nbase();
XERXES_EXPORT int XERXES_API dxp_nevent();
XERXES_EXPORT int XERXES_API dxp_ndxp();
XERXES_EXPORT int XERXES_API dxp_ndxpchan();
XERXES_EXPORT int XERXES_API dxp_max_symbols();
XERXES_EXPORT int XERXES_API dxp_symbolname_list();
XERXES_EXPORT int XERXES_API dxp_symbolname_by_index();
XERXES_EXPORT int XERXES_API dxp_symbolname_limits();
XERXES_EXPORT int XERXES_API dxp_check_decimation();
XERXES_EXPORT int XERXES_API dxp_calibrate();
XERXES_EXPORT int XERXES_API dxp_calibrate_one_channel();
XERXES_EXPORT int XERXES_API dxp_findpeak();
XERXES_EXPORT int XERXES_API dxp_fitgauss0();
XERXES_EXPORT int XERXES_API dxp_save_config();
XERXES_EXPORT int XERXES_API dxp_restore_config();
XERXES_EXPORT int XERXES_API dxp_save_dspparams();
XERXES_EXPORT int XERXES_API dxp_open_file();
XERXES_EXPORT int XERXES_API dxp_close_file();
XERXES_EXPORT int XERXES_API dxp_get_electronics();
XERXES_EXPORT int XERXES_API dxp_get_detectors();
XERXES_EXPORT int XERXES_API dxp_modify_gains();
XERXES_EXPORT int XERXES_API dxp_modify_one_gains();
XERXES_EXPORT int XERXES_API dxp_initialize_ASC();
XERXES_EXPORT int XERXES_API dxp_det_to_elec();
XERXES_EXPORT int XERXES_API dxp_elec_to_det();
XERXES_EXPORT int XERXES_API dxp_mem_dump();
XERXES_EXPORT int XERXES_API dxp_get_statistics();
XERXES_EXPORT int XERXES_API dxp_replace_filesymbol();
XERXES_EXPORT int XERXES_API dxp_read_filesymbol();
XERXES_EXPORT void XERXES_API dxp_version();
XERXES_EXPORT int XERXES_API dxp_locate_system_files();
XERXES_EXPORT int XERXES_API dxp_locate_channel_files();
XERXES_EXPORT int XERXES_API dxp_lock_resource();
XERXES_EXPORT int XERXES_API dxp_read_adc();

XERXES_STATIC int XERXES_API dxp_get_btype();
XERXES_STATIC int XERXES_API dxp_add_btype_library();
XERXES_STATIC int XERXES_API dxp_read_modules();
XERXES_STATIC int XERXES_API dxp_init_iface_ds();
XERXES_STATIC int XERXES_API dxp_init_dsp_ds();
XERXES_STATIC int XERXES_API dxp_init_fippi_ds();
XERXES_STATIC int XERXES_API dxp_init_defaults_ds();
XERXES_STATIC int XERXES_API dxp_free_board();
XERXES_STATIC int XERXES_API dxp_free_iface();
XERXES_STATIC int XERXES_API dxp_free_dsp();
XERXES_STATIC int XERXES_API dxp_free_fippi();
XERXES_STATIC int XERXES_API dxp_free_params();
XERXES_STATIC int XERXES_API dxp_free_defaults();
XERXES_STATIC int XERXES_API dxp_free_binfo();
XERXES_STATIC int XERXES_API dxp_add_fippi();
XERXES_STATIC int XERXES_API dxp_add_dsp();
XERXES_STATIC int XERXES_API dxp_add_defaults();
XERXES_STATIC int XERXES_API dxp_pick_filename();
XERXES_STATIC int XERXES_API dxp_strnstrm();
XERXES_STATIC int XERXES_API dxp_readout_run();
XERXES_STATIC FILE* XERXES_API dxp_find_file();

XERXES_IMPORT int DXP_API dxp_init_dxp4c2x();
XERXES_IMPORT int DXP_API dxp_init_dxp4c();
XERXES_IMPORT int DXP_API dxp_init_dxpx10p();
XERXES_IMPORT int MD_API dxp_md_init_util();
XERXES_IMPORT int MD_API dxp_md_init_io();

#endif                                  /*   end if _XERXES_PROTO_ */

/* If this is compiled by a C++ compiler, make it clear that these are C routines */
#ifdef __cplusplus
}
#endif

#endif						/* Endif for XIA_XERXES_H */
