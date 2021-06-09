#include "atCmdDevice.h"
#include <string.h>
#include <stdio.h>

#define CM_CMD_GENERAL_RESPONSE                  "\r\nOK\r\n"

static cmError_t cmWaitNonBlock(cmData_t *obj, uint32_t msec);
static cmError_t cmWaitBlock(cmData_t *obj, uint32_t msec);

static cmError_t cmWaitForResponse(cmData_t *obj , char *expectedResponse, uint32_t mSec);
static cmError_t cmMatchResponse(cmData_t *obj, char *expectedResponse);
static void cmResetObject(cmData_t *obj);
static void cmStringTx(cmData_t *obj, char* String);

//#define ENABLED_WAIT_NONBLOCK

#ifdef ENABLED_WAIT_NONBLOCK
#define cmWait(obj, msec)    cmWaitNonBlock(obj, msec)
#else 
#define cmWait(obj, msec)    cmWaitBlock(obj, msec)
#endif


cmError_t cmInit(cmData_t *obj){
    cmError_t retValue = CM_FAIL;
    
    if( (obj->tickReadHandler != NULL) && (obj->txHandler != NULL) && (obj->rxData.buffer != NULL))
        retValue = CM_OK;
    
    cmResetObject(obj);
    if(obj->rxData.len <= 0 )
        obj->rxData.len = CM_BUFFER_LEN_DEF;
    
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
	while( CM_ERR_WAITING == cmWaitNonBlock(obj, msec) ){}
	return CM_ERR_TIMEOUT ;
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
        //printf("\nbuff [%s]",obj->rxData.buffer);
	}
	
	/*para que funcione block or non block*/
    // noveo necesario que devuelva timeout, con invalid response es suficiente
	if( (CM_ERR_INVALID_RESPONSE == retValue) || (CM_OK == retValue) ){ /*misra c 12.1*/
		if( 1 == obj->rxData.ready ){ /*misra c 14.4*/
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
    }else if( (retMatchResp == CM_ERR_INVALID_RESPONSE) &&  (retWait == CM_ERR_WAITING)){
        retVal = CM_ERR_WAITING;
    }else{
        retVal = CM_ERR_INVALID_RESPONSE;  //invalid response & timeout
    }

    return retVal;
}

static cmError_t cmMatchResponse(cmData_t *obj, char *expectedResponse){
	cmError_t Rprocess = CM_ERR_INVALID_RESPONSE;
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

     
     if( (strstr((const char*)obj->rxData.buffer, CM_CMD_GENERAL_RESPONSE) != NULL) ){
        obj->rxData.index = 0;
		obj->rxData.ready = 1; // completed frame
     }
}

static void cmResetObject(cmData_t *obj){
	(void)memset( (void *) obj->rxData.buffer,0,obj->rxData.len);
	//obj->StatusFlag=(uint8_t)_DEFAULT;
	obj->rxData.index = 0;
	obj->rxData.ready = 0;
	obj->DataPrivate.apiState = API_IDLE; 
	obj->DataPrivate.apiState = WAIT_IDLE; 
}

static void cmStringTx(cmData_t *obj, char* String){
  	int i =0;
   // printf("cmd : [%s]\n",String);
  	while( '\0' != String[i]  ) {          
		obj->txHandler(NULL,String[i++]);
	}
}
