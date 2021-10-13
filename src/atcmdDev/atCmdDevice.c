#include "atCmdDevice.h"
#include <string.h>
#include <stdio.h>
#undef _USE_STRSTR_FROM_ISR
#define CM_CMD_ERROR_RESPONSE_FROM_ISR                      "ERROR"  
#define CM_CMD_URC_RESPONSE_FROM_ISR                      "+QIURC:"  

typedef enum{
	ISR_STATE_WAITING = 0,
	ISR_STATE_END_RESPONSE
}isrState_t;

static cmError_t cmWaitNonBlock(cmData_t *obj, uint32_t msec);
static cmError_t cmWaitBlock(cmData_t *obj, uint32_t msec);
static cmError_t cmWrapperWaitNonBlock(cmData_t *obj, uint32_t msec);  /*!< exit when buffer isReady*/

static cmError_t cmWaitForResponse(cmData_t *obj , char *expectedResponse, uint32_t mSec);
static cmError_t cmMatchResponse(cmData_t *obj, char *expectedResponse);
static void cmResetObject(cmData_t *obj);
static void cmStringTx(cmData_t *obj, char* String);
static int IsrStrResponseCmpManually(cmData_t *obj , char rxChar);
static uint8_t findStrCharByChar(char* matchPattern, char rxchar);
static void cmResetAsyncEventObject(cmData_t *obj);

//#define ENABLED_WAIT_NONBLOCK

#ifdef ENABLED_WAIT_NONBLOCK
#define cmWait(obj, msec)    cmWrapperWaitNonBlock(obj, msec)
#else 
#define cmWait(obj, msec)    cmWaitBlock(obj, msec)
#endif


cmError_t cmInit(cmData_t *obj){
    cmError_t retValue = CM_FAIL;
	static volatile char buffer_urc[CM_BUFFER_LEN_DEF] = {0};
    if(obj != NULL){
		obj->rxData.bufferAsyncEvent = buffer_urc;
		if( obj->matchResponseFromIsr == NULL){
			obj->matchResponseFromIsr = CM_CMD_GENERAL_RESPONSE_FROM_ISR;
			obj->DataPrivate.atvFormat = 1; 
		}
		if( (obj->tickReadHandler != NULL) && (obj->txHandler != NULL) && (obj->rxData.buffer != NULL))
			retValue = CM_OK;
		
		cmResetAsyncEventObject(obj);
		cmResetObject(obj);
		if(obj->rxData.len <= 0 )
			obj->rxData.len = CM_BUFFER_LEN_DEF;
	}
    return retValue;
}




static cmError_t cmWaitNonBlock(cmData_t *obj, uint32_t msec){
	static cmError_t retValue;
	static volatile uint32_t startTick = 0;
	if( WAIT_IDLE == obj->DataPrivate.waitState ){
		retValue = CM_ERR_WAITING ;
		startTick = 0;
		startTick = obj->tickReadHandler() ;//tickRead_ms();
		obj->DataPrivate.waitState = WAIT_RUNING ;
	}

	if( ( obj->tickReadHandler() - startTick) > msec ){ 
		obj->DataPrivate.waitState = WAIT_IDLE;
		obj->DataPrivate.apiState = API_IDLE; //Cuando vence el tiempo mando el comando de nuevo
		retValue = CM_ERR_TIMEOUT ;
	}
	else{
		retValue = CM_ERR_WAITING;
	}
	return retValue;

}

static cmError_t cmWaitBlock(cmData_t *obj, uint32_t msec){
	cmError_t retVal = CM_ERR_WAITING;
	while( CM_ERR_WAITING == retVal ){
		retVal = cmWrapperWaitNonBlock(obj, msec) ;
	}
	return retVal ;
}

/* this function is used for wrapper WaitNonblock and can exit from wait block when buffer is ready*/
static cmError_t cmWrapperWaitNonBlock(cmData_t *obj, uint32_t msec){
	cmError_t retVal = CM_ERR_WAITING;
	retVal = cmWaitNonBlock(obj, msec) ;
	if(1 == obj->rxData.ready){
		retVal = CM_ERR_TIMEOUT;
	}
	return retVal ;
}
cmError_t cmSendCmd(cmData_t *obj, char* payload, char* expectedResponse, char * buffStr, uint32_t mSec){

	cmError_t retValue = CM_FAIL;
	if( API_IDLE == obj->DataPrivate.apiState ){ /*Firstentry bloqueante o no bloqueante*/
		cmResetObject(obj);
		if( NULL != payload ) {
			cmStringTx(obj,payload);
		}
		obj->DataPrivate.apiState = API_RUNING;
		if(obj->DataPrivate.numberRetries++ > CM_MAX_NUM_RETRIES) {
			obj->DataPrivate.numberRetries = 0;
			retValue = CM_ERR_MAX_RETRIES_REACHED; /* misra c 15.5*/
		}

	}
	/*expected is confirmed in to cmWaitForResponse*/
	if( CM_ERR_MAX_RETRIES_REACHED == retValue ){
		/*compliant misra c 15.5*/
	}
	else{
		retValue = cmWaitForResponse(obj, expectedResponse,mSec);
	}
	
	/*para que funcione block or non block*/
    // noveo necesario que devuelva timeout, con invalid response es suficiente
	if( (CM_ERR_INVALID_RESPONSE == retValue) || (CM_OK == retValue) ){ /*misra c 12.1*/
		if( 1 == obj->rxData.ready && (CM_OK == retValue) ){ /*misra c 14.4*/
			if( NULL != buffStr ){  /*misra c 15.6*/
				(void)strcpy(buffStr, (char*)obj->rxData.buffer) ; /*misra c 17.7*/
			}

			obj->DataPrivate.numberRetries = 0;
		}
		obj->DataPrivate.apiState = API_IDLE;  //reset object y envia comando de nuevo
	}
	return retValue;
}


static cmError_t cmWaitForResponse(cmData_t *obj , char *expectedResponse, uint32_t mSec){
	cmError_t retWait;
    cmError_t retMatchResp ; 
    cmError_t retVal; ; 
    
	retWait =  cmWait(obj, mSec); /*Return CM_ERR_WAITING or CM_ERR_TIMEOUT*/
	retMatchResp = cmMatchResponse(obj, expectedResponse); /*Return Response ok or No match*/

    if( (retMatchResp == CM_OK  ) ){
        retVal = CM_OK;
    }else if( (retMatchResp == CM_ERR_INVALID_RESPONSE  ) ){
        retVal = CM_ERR_INVALID_RESPONSE;
    }else if (retWait == CM_ERR_TIMEOUT){
        retVal = CM_ERR_TIMEOUT;
    }else if( (retMatchResp == CM_ERR_WAITING) &&  (retWait == CM_ERR_WAITING)){
        retVal = CM_ERR_WAITING;
    }

    return retVal;
}

static cmError_t cmMatchResponse(cmData_t *obj, char *expectedResponse){
	cmError_t Rprocess = CM_ERR_WAITING;  /*si no ha llegado nada al buffer*/
	if((bool)obj->rxData.ready){
		if( NULL != expectedResponse ){
			if( NULL != strstr((const char *) obj->rxData.buffer,(char *)expectedResponse) ) {
				Rprocess = CM_OK;
			}
			else{
				Rprocess = CM_ERR_INVALID_RESPONSE;
			}
		}
		else{ 
			Rprocess = CM_OK;
		}
	}
	return Rprocess;
}

void cmIsrRx(cmData_t *obj, const char rxChar){

	/* misra c 15.5*/
	if( (rxChar < (char)CHAR_PRINT_BELOW)  || (rxChar > (char)CHAR_PRINT_ABOVE) ) {
		return ;  /*Char no print*/
	}

	if(1 == obj->rxData.readyAsyncEvent){
		obj->rxData.bufferAsyncEvent[obj->rxData.indexAsyncEvent++] = rxChar;
		if(obj->rxData.indexAsyncEvent > CM_BUFFER_LEN_DEF ){
			obj->rxData.indexAsyncEvent = 0;
		}
		obj->rxData.bufferAsyncEvent[obj->rxData.indexAsyncEvent] = 0;

		if(rxChar == '\n'){
			 obj->rxData.readyAsyncEvent = 0;
			 obj->rxData.asyncEvent=1;
			 obj->rxData.countAsyncEvent++;
		}
	}else{
		obj->rxData.readyAsyncEvent = findStrCharByChar("URC",rxChar);
	}

	/*misra c 15.6,14.4 , 15.6*/
	if( 1 == obj->rxData.ready) {
		return; /* B_uffer reveived*/
	}
	 obj->rxData.buffer[obj->rxData.index] = rxChar; /*misra c 13.3*/
	obj->rxData.index++;

	if (obj->rxData.index >= (obj->rxData.len - (uint8_t)1) ) { /* misra c 10.4*/
		obj->rxData.index = 0; 
	}
	 obj->rxData.buffer[obj->rxData.index] = (char)0;

	#ifdef _USE_STRSTR_FROM_ISR
     //TODO: remove strstr and compare character singly
     if( (strstr((const char*)obj->rxData.buffer, obj->matchResponseFromIsr) != NULL) ||
	 	(strstr((const char*)obj->rxData.buffer, CM_CMD_ERROR_RESPONSE_FROM_ISR ) != NULL) /*exit when error or ok*/
		 ){
        obj->rxData.index = 0;
		obj->rxData.ready = 1; // completed frame
     }
	 if(strstr((const char*)obj->rxData.buffer, CM_CMD_URC_RESPONSE_FROM_ISR ) != NULL){
		 if(rxChar == '\n'){
			obj->rxData.index = 0;
			obj->rxData.ready = 1; // completed frame
			obj->rxData.asyncEvent = 1; // async event frame
		 }
	 }
	 #else
	 IsrStrResponseCmpManually(obj, rxChar);
	 #endif
}

static int IsrStrResponseCmpManually(cmData_t *obj , char rxChar){

    int retValue = 0;
    static char *array;
    static size_t pos = 0, cont = 0;
    char rxchar = rxChar;
 	static isrState_t isrState = ISR_STATE_WAITING;

	switch (isrState){
		
		case ISR_STATE_WAITING:
			if( obj->rxData.responsesExpected != NULL){
				for(int i = 0 ;i < CM_MAX_NUM_RESPONSES_EXPECTED; ++i ){
					/*if urc point to str asyn event*/
					array = &obj->rxData.responsesExpected[pos][0];

					if( rxchar == array[cont] ){
						if(cont == obj->rxData.lenResponseExpected[pos]){

							isrState = ISR_STATE_WAITING;
							obj->rxData.ready = 1;
							retValue = 1;
							pos = 0;
							obj->rxData.index = 0;
							cont = 0;
							break;
						}else{
							cont++;  /*move char position */
							break;
						}
					}else{
						cont = 0;
						if( pos == CM_MAX_NUM_RESPONSES_EXPECTED -1){
							pos = 0;
							}
						else{
							pos++; /*move pointer to next string expected*/
							}
						isrState = ISR_STATE_WAITING;
					}
					if(isrState == ISR_STATE_END_RESPONSE){
						break;
					}
				}
			}
		break;
	 default:
		break;
	 }
    return retValue;
}

static uint8_t findStrCharByChar(char* matchPattern, char rxchar){
	char * array = matchPattern;
	uint8_t retValue = 0;
	static int cont = 0;

	if( array != NULL){
		if( rxchar == array[cont] ){
			if(cont == 3-1){
				retValue = 1;
				cont = 0;
			}else{
				if(cont++ > 3-1)
					cont = 0;  /*move char position */
			}
		}else{
			cont = 0;
		}
	}
	return retValue;
}

static void cmResetObject(cmData_t *obj){
	(void)memset( (void *) obj->rxData.buffer,0,obj->rxData.len);
	obj->rxData.index = 0;
	obj->rxData.ready = 0;
	obj->DataPrivate.apiState = API_IDLE; 
	obj->DataPrivate.waitState = WAIT_IDLE; 
}

static void cmResetAsyncEventObject(cmData_t *obj){
	if(obj->rxData.bufferAsyncEvent != NULL)
		(void)memset( (void *) obj->rxData.bufferAsyncEvent, 0, CM_BUFFER_LEN_DEF);

	obj->rxData.indexAsyncEvent = 0;
	obj->rxData.readyAsyncEvent = 0;
	obj->rxData.asyncEvent = 0;
	obj->rxData.countAsyncEvent = 0;
}

static void cmStringTx(cmData_t *obj, char* String){
  	int i =0;
  	while( '\0' != String[i]  ) {          
		obj->txHandler(NULL,String[i++]);
	}
}

void cmSendDirectUart(cmData_t *obj, char *str){
	cmStringTx(obj, str);
}

uint8_t cmIsAsyncEvent(cmData_t *obj){
	return (uint8_t)obj->rxData.asyncEvent;
}

char* cmGetBufferRecv(cmData_t *obj){
	return (char*)obj->rxData.buffer;
}

void cmClearAllAsyncEvent(cmData_t *obj){
	cmResetAsyncEventObject(obj);
}

char* cmGetBufferAsyncEvent(cmData_t *obj){
	return (char*)obj->rxData.bufferAsyncEvent;
}