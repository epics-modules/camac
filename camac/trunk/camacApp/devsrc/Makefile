TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC_vxWorks += devCamac

DBD += devCamacSupport.dbd

# The following are compiled and added to the Support library
devCamac_SRCS += devCamac.c
devCamac_SRCS += devScalerCamac.c
devCamac_SRCS += devMotorE500.cc
devCamac_SRCS += drvMotorE500.cc

devCamac_LIBS += camac
devCamac_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

