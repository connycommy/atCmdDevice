/**
 * @file EC25.h
 * @author Julian Bustamante Narvaez
 * @brief cellular modem (cm) library for Quectel EC25
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _EC25_H_
#define _EC25_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Ec25Command.h"
#include "atCmdDevice.h"
#include <stdint.h>
#include "stddef.h"
#include "stdbool.h"


typedef struct EC25Data{
    int DataPrivate; /*private data internal implementation*/
}EC25Data_t;

/** @brief Macro that can be used to initialize cmData_t instance*/
#define EC25_DATA_INITIALIZER {0}   



// prototype

cmError_t EC25_InitModem(cmData_t *obj, char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction );

cmError_t EC25_GetSignalQuality(cmData_t *obj ,uint8_t *rssi );
void EC25_IsrRx(cmData_t *obj, const char rxChar);


#ifdef __cplusplus
}
#endif
#endif /*end _EC25_H_*/