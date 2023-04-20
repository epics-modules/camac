#include <epicsExport.h>
#include <iocsh.h>
#include "camacLib.h"
#include "ksc2917.h"

static const iocshArg kscsetupArg0 = {"num cards", iocshArgInt};
static const iocshArg kscsetupArg1 = {"addrs", iocshArgString};
static const iocshArg kscsetupArg2 = {"ivec base", iocshArgInt};
static const iocshArg kscsetupArg3 = {"irq level", iocshArgInt};

static const iocshArg * const kscsetupArgs[4] = {&kscsetupArg0, &kscsetupArg1, &kscsetupArg2, &kscsetupArg3};
static const iocshFuncDef kscsetupFuncDef = {"ksc2917_setup", 4, kscsetupArgs};

static void kscsetupCallFunc(const iocshArgBuf *arg)
{
    ksc2917_setup(arg[0].ival, arg[1].sval, arg[2].ival, arg[3].ival);
}

static const iocshArg * const camacinitArgs[1] = {};
static const iocshFuncDef camacinitFuncDef = {"camacLibInit", 0, camacinitArgs};

static void camacinitCallFunc(const iocshArgBuf *arg)
{
    camacLibInit();
}


static void camacRegistrar(void) {
    iocshRegister(&kscsetupFuncDef, kscsetupCallFunc);
    iocshRegister(&camacinitFuncDef, camacinitCallFunc);
}

epicsExportRegistrar(camacRegistrar);
