#include "Ec25.h"
#include <string.h>
#include <stdio.h>


cmError_t EC25_InitModem(cmData_t *obj,char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction ){

    obj->rxData.buffer = (char*)inputBuffer;
    obj->rxData.len = len;
    obj->tickReadHandler = (cmTickReadHandler_t)GetTickCountMs;
    obj->txHandler = (cmTransmissionHandler_t)OutputFunction;
    return cmInit(obj);
}

cmError_t EC25_GetSignalQuality(cmData_t *obj ,uint8_t *rssi ){
    cmError_t retVal;
    retVal = cmSendCmd(obj, CM_CMD_SIGNAL_QUALITY_REPORT, EC25_CMD_SIGNAL_QUALITY_REPORT ,NULL ,3000);
    if(retVal == CM_OK){
        // deco frame
        // rssi = anythings
    }
    return retVal;
}
void EC25_IsrRx(cmData_t *obj, const char rxChar){
    cmIsrRx(obj, rxChar); // comentar para nonblocking test wait
}
