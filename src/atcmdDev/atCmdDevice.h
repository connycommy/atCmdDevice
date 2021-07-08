/**
 * @file atCmdDevice.h
 * @author Julian Bustamante Narvaez
 * @brief AT command interface library for any modem device
 * @version 0.0.0
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _AT_CMD_DEVICE_H_
#define _AT_CMD_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stddef.h"
#include "stdbool.h"

#define CM_CMD_GENERAL_RESPONSE_FROM_ISR                  "\r\nOK\r\n"  /*!< can be used to initialize obj->matchResponseFromIsr*/

#define CM_BUFFER_LEN_DEF   1024 /*!< buffer length by default*/
#define CM_MAX_NUM_RETRIES   4 /*!< RETRIES NUMBER MAXIMUN by default*/

/**Char no print*/
#define CHAR_PRINT_BELOW	( 10 )

/**Char no print*/
#define CHAR_PRINT_ABOVE	( 122 )

typedef enum {
    CM_FAIL =   -1,         /*!< Generic cm_error_t code indicating failure */
    CM_OK   =    0,         /*!< cm_error_t value indicating success (no error) */
    
    CM_ERR_TIMEOUT,         /*!< Operation timed out */
    CM_ERR_WAITING,         /*!< Operation waiting*/
    CM_ERR_INVALID_RESPONSE, /*!< Received response was invalid*/
    CM_ERR_MAX_RETRIES_REACHED,  /*!< retries reached send command*/
    CM_ASYNC_EVENT
   // CM_ERR_INVALID_ARG      /*!< Invalid argument */

}cmError_t;


typedef uint32_t (*cmTickReadHandler_t)(void);          /*!< Pointer to Function type TickReadFcn_t : function Get Tick in ms. */

typedef void (*cmTransmissionHandler_t)(void*, char);  /*!< pointer to function: Transmission by UART */

//uart structure
typedef struct {
    volatile char *buffer;
    volatile uint8_t ready;
    volatile uint8_t asyncEvent; /*!< for urc*/
    size_t len;
    volatile uint16_t index;
}cmDataRx_t;


//PRIVATE

typedef enum {
    WAIT_IDLE = 0,
    WAIT_RUNING
}cmWaitState_t;

typedef enum {
    API_IDLE = 0,
    API_RUNING
}cmApiState_t;

typedef struct _private{
    cmWaitState_t waitState;
    cmApiState_t apiState;
    uint8_t numberRetries;
    uint8_t atvFormat; /* ATV1 or ATV0*/
}cmDataPrivate_t;

typedef struct cmData{
    cmTickReadHandler_t tickReadHandler;
    cmTransmissionHandler_t txHandler;
    cmDataRx_t  rxData;
    cmDataPrivate_t DataPrivate; /*private data internal implementation*/
    /*by default in CM_CMD_GENERAL_RESPONSE_FROM_ISR*/
    char *matchResponseFromIsr; /*respuesta principal para que  driver funcione inicializado*/
}cmData_t;

/** @brief Macro that can be used to initialize cmData_t instance*/
#define CM_DATA_INITIALIZER {NULL, NULL,{NULL,0,0,0,0},{0,0,0,1},NULL}   



// prototype

cmError_t cmInit(cmData_t *obj);
cmError_t cmSendCmd(cmData_t *obj,char* payload,char* expectedResponse,char * buffStr, uint32_t mSec);
void cmIsrRx(cmData_t *obj, const char rxChar);
uint8_t cmIsAsyncEvent(cmData_t *obj);
char* cmGetBufferRecv(cmData_t *obj);
// getBufferReady

#ifdef __cplusplus
}
#endif
#endif /*end _AT_CMD_DEVICE_H_*/