#include "cellModem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *responses[16]={
    "OK\r\n",
    "ERROR",
    ">",
   // "URC", /*not move , keep in this position*/
    "\r\n", /*not move , keep last*/
};

static char* extractBetween(char *src , char init, char end, char* outPutMatch,int offset );
static char* intToStr(int value);


cmError_t cellModem_Init(cmData_t *obj,char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction, char *matchResponseFromIsr){
    if( obj != NULL){
         
        if(responses != NULL){
            obj->rxData.responsesExpected = responses;  /*OK ERROR etc*/
            for (int i=0; i< CM_MAX_NUM_RESPONSES_EXPECTED ; i++){
                obj->rxData.lenResponseExpected[i] =strlen(obj->rxData.responsesExpected[i])-1;  /*len OK len ERROR etc*/
            }
        }
        obj->rxData.buffer = (char*)inputBuffer;
        obj->rxData.len = len;
        obj->tickReadHandler = (cmTickReadHandler_t)GetTickCountMs;
        obj->txHandler = (cmTransmissionHandler_t)OutputFunction;
        obj->matchResponseFromIsr = matchResponseFromIsr;
    }
    return cmInit(obj);
}

cmError_t cellModem_GetSignalQuality(cmData_t *obj ,uint8_t *rssi ){
    cmError_t retVal = CM_OK;
    char cmd[15]="AT+CSQ\r";
    char bufferRecv[60];
    char RSSI[10] = "\0";
    char *ptr = NULL;

    retVal = cmSendCmd(obj, cmd, (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+CSQ:")) != NULL ){
            ptr = extractBetween(bufferRecv,' ',',',RSSI,0);
            *rssi = (uint8_t)atoi(RSSI);
        }else{
           // retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*usage: cellModem_SendRaw(&cmData, "AT\r\n") */
cmError_t cellModem_SendRaw(cmData_t *obj, char * cmd){
    cmError_t retVal = CM_FAIL;
    if( cmd != NULL)
        retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,300);
    return retVal;
}

cmError_t cellModem_SetApn(cmData_t *obj, cellModemConfig_t *config){
    cmError_t retVal;
    char apn[100]="AT+QICSGP=1,1,\""; //TODO:change contextid and connectId as input
    char *ptr = NULL;

      if(config->apn != NULL && config->apnUser != NULL  && config->apnPassw != NULL  ){
        ptr = strcat(apn +15, config->apn );
        ptr = strcat(ptr ,"\",\"" );
        ptr = strcat(ptr ,config->apnUser );
        ptr = strcat(ptr ,"\",\"" );
        ptr = strcat(ptr ,config->apnPassw );
        ptr = strcat(ptr ,"\",3\r" );
      }else{
          strcat(apn,CELLMODEM_CMD_APN_DEFAULT);
      }
    retVal = cmSendCmd(obj,apn , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,500);

    return retVal;
}

/* Activate context profile */
cmError_t cellModem_SetContextProfile(cmData_t *obj, cellModemConfig_t *config){
    cmError_t retVal;
    char cmd[15]="AT+QIACT=";
    char snum[5];
    char *ptr = NULL;
    sprintf(snum,"%d",config->contextId);
    ptr = strcat(cmd + 9 , snum);
    ptr = strcat(ptr, "\r");

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,150000);//150sg

    return retVal;
}

void cellModem_IsrRx(cmData_t *obj, const char rxChar){
    cmIsrRx(obj, rxChar); // comentar para nonblocking test wait
}


cmError_t cellModem_GetNetworkInformation(cmData_t *obj, netInfo_t *getNetinfo){
    cmError_t retVal;
    char cmd[15]="AT+QNWINFO\r";
    char bufferRecv[128];
    char *ptr;

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        //+QNWINFO: "FDD LTE","732101","LTE BAND 7",3075
        //api bufferready , apiclearready
        if( (ptr = strstr(bufferRecv,"+QNWINFO:")) != NULL ){
            ptr = extractBetween(bufferRecv,'"','"',getNetinfo->accessTechnSel,0);
            ptr = extractBetween(ptr,'"','"',getNetinfo->operator,2);
            ptr = extractBetween(ptr,'"','"',getNetinfo->band,2);
            ptr = extractBetween(ptr,',','\r',getNetinfo->channelId,1);
        }else{
            //retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*return manofactur id, model module and firmware versión*/
cmError_t cellModem_GetModuleInformation(cmData_t *obj, modemInfo_t *getModemInfo){
    cmError_t retVal;
    char cmd[15]="ATI\r";
    char bufferRecv[60];
    char *ptr;
    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
            ptr = extractBetween(bufferRecv,'\n','\r',getModemInfo->manufactorId,0);
            ptr = extractBetween(ptr,'\n','\r',getModemInfo->deviceModel,1);
            ptr = extractBetween(ptr,'\n','\r',getModemInfo->firmwareVersion,0);
 
    }
    return retVal;
}

cmError_t cellModem_GetImei(cmData_t *obj, char *getImei){
    cmError_t retVal;
    char cmd[15]="AT+GSN\r";
    char bufferRecv[60];

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if(getImei != NULL)
            extractBetween(bufferRecv,'\n','\r',getImei,0);
    }
    return retVal;
}

cmError_t cellModem_GetImsi(cmData_t *obj,  char *getImsi){
    cmError_t retVal;
    char cmd[15]="AT+CIMI\r";
    char bufferRecv[60];

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if(getImsi != NULL)
            extractBetween(bufferRecv,'\n','\r',getImsi,0);
    }
    return retVal;
}

cmError_t cellModem_GetIccid(cmData_t *obj,  char *getIccid){
    cmError_t retVal;
    char cmd[15]="AT+QCCID\r";
    char bufferRecv[60];
    char *ptr = NULL;

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+QCCID:")) != NULL ){
            if(getIccid != NULL){
                extractBetween(bufferRecv,' ','\r',getIccid,0);
            }
        
        }
    }
    return retVal;
}

/* Use ATE1 to enable echo mode*/
cmError_t cellModem_SetATE(cmData_t *obj, uint8_t echoEn){
    cmError_t retVal;
    char cmd[8]="ATE";
    cmd[3] = (char)( (0 == echoEn ) ? (char)echoEn +'0' : '1');
    cmd[4]  = '\r';
    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,300);
    return retVal;
}

/* Use ATV1 to set the response format 
    value =1 -> OK    <CR><LF><text><CR><LF>
    value = 0 -> 0    <text><CR><LF>  
    Note: should change matchResponseIsr in initModem according to value
    NO USE with 0, should be with 1, OK/r/n
*/
cmError_t cellModem_SetATV(cmData_t *obj, uint8_t value){
    cmError_t retVal = CM_FAIL;
    char cmd[8]="ATV";
    cmd[3] = (char)( (0 == value ) ? (char)value +'0' : '1');
    cmd[4]  = '\r';

    cellModem_SetatvFormat(obj,value);
    cellModem_SetMatchResponseFromIsr(obj, (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ); /*cambia la respuesta esperada de la Isr*/
    retVal = cmSendCmd(obj,cmd , (0 == value )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,NULL ,300);

    return retVal;
}

cmError_t cellModem_GetCREG(cmData_t *obj, netRegStatus_t *netRegStatus){
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[10]="AT+CREG?\r";
    char *ptr =NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,300); 
    
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+CREG:")) != NULL ){
            if(netRegStatus != NULL){
                extractBetween(bufferRecv,' ',',',buffer,0);
                netRegStatus->n = (int)atoi(buffer);
                memset(buffer,0,sizeof(buffer));
                extractBetween(bufferRecv,',','\r',buffer,1);
                netRegStatus->stat = (int)atoi(buffer);
            }
        }
    }
    
    return retVal;
}

cmError_t cellModem_GetCOPS(cmData_t *obj, netOperatorStatus_t *netOperatorStat){
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[10]="AT+COPS?\r";
    char *ptr =NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,180000); //180 sg
    
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+COPS:")) != NULL ){
            if(netOperatorStat != NULL){
                ptr = extractBetween(bufferRecv,' ',',',buffer,0);
                netOperatorStat->mode = (int)atoi(buffer);
                memset(buffer,0,sizeof(buffer));
                
                ptr = extractBetween(ptr,',',',',buffer,0);
                netOperatorStat->format = (int)atoi(buffer);
                
                ptr = extractBetween(ptr,'"','"',netOperatorStat->operator,1);

                memset(buffer,0,sizeof(buffer));
                ptr = extractBetween(ptr,',','\r',buffer,0);
                netOperatorStat->act = (int)atoi(buffer);
            }
        }
    }
    
    return retVal;
}

cmError_t cellModem_GetCPIN(cmData_t *obj, char *getCode){
    cmError_t retVal;
    char bufferRecv[60]={0};
    char cmd[10]="AT+CPIN?\r";
    char *ptr = NULL;

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); 
    
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+CPIN:")) != NULL ){
            if(getCode != NULL){
                ptr = extractBetween(bufferRecv,' ','\r',getCode,0);
            }
        }
    }
    
    return retVal;
}

/* get context profile , only check one context, not multiple*/
cmError_t cellModem_GetContextProfile(cmData_t *obj, cellModemConfig_t *config){  
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[16]="AT+QIACT?\r";
    char *ptr = NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); //150 sg
    
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+QIACT:")) != NULL ){
            if(config != NULL){
                ptr = extractBetween(bufferRecv,' ',',',buffer,0);
                config->contextId = (int)atoi(buffer);
                ptr = extractBetween(bufferRecv,',',',',buffer,0);
                config->contextState = (int)atoi(buffer);
                ptr = extractBetween(ptr,',',',',buffer,0);
                config->contextType = (int)atoi(buffer);
                ptr = extractBetween(ptr,'"','"',config->contextIpAddres,0);
            }
        }else{
           // retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*deactivate context profile*/
cmError_t cellModem_SetDeactContextProfile(cmData_t *obj, uint8_t contextID){
    cmError_t retVal;
    char cmd[16]="AT+QIDEACT=";
    cmd[11] = (char)( (0 == contextID ) ? (char)contextID +'0' : '1');
    cmd[12]  = '\r';

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,40000);/*40 sg*/

    return retVal;
}

/*CSCA?
CMEE=1  esto es default, necesario?
CNMI=2,1   esto es default, necesario?
CPMS="SM","SM","SM"   memoria en la simcard ( no es necesario)
CMGF=0  esto es default, necesario? PDU default
CMGL=4  all messages, PDU
CMGD=1,4  mem1 y borra todos los leidos  ( esto no es necesario por ahora)*/


// 1 text format , 0 PDU format
cmError_t cellModem_SmsSetMessageFormat(cmData_t *obj, uint8_t mode){
    cmError_t retVal;
    char cmd[16]="AT+CMGF=";
    cmd[8] = (char)( (0 == mode ) ? (char)mode +'0' : '1');
    cmd[9]  = '\r';

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,300);

    return retVal;
}

cmError_t cellModem_SmsSetTextModeParam(cmData_t *obj){
    // default
    return cmSendCmd(obj,"AT+CSMP=17,167,0,0\r" , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,300);
}

cmError_t cellModem_SmsSetTeCharacterGsm(cmData_t *obj){
    // GSM default alphabet 
    return cmSendCmd(obj,"AT+CSCS=\"GSM\"\r" , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,300);
}

cmError_t cellModem_SmsSendNumber(cmData_t *obj, const char *number){
    cmError_t retVal;
    char cmd[256]="AT+CMGS=\"";  // max 160 char, pending controller
    char *ptr = NULL;

    ptr = strcat(cmd+9, number);
    ptr = strcat(ptr,"\"\r");

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :">" ,NULL ,12000);/*120 sg*/

    return retVal;
}

/*For its correct operation, it must first be used cellModem_SmsSendNumber*/
cmError_t cellModem_SmsSendTextMessage(cmData_t *obj, const char *msg){
    cmError_t retVal;
    char cmd[256] ={0};  // max 160 char, pending controller
    memcpy((void*)cmd,(void*)msg, strlen(msg));

    //\x1A  = ctrl+z = 0x1A para que salga el mensaje
    strcat(cmd, "\r");
   
    retVal = cmSendCmd(obj,(char*)cmd , NULL ,NULL ,12000);/*120 sg*/
    retVal = cmSendCmd(obj,"\x1A\r" , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,NULL ,12000);/*120 sg*/
    //cmSendDirectUart(obj,"\x1A");
    return retVal;
}


/*AT+QIOPEN=1,3,"TCP LISTENER","127.0.0.1",0,2020,0
"TCP" "UDP" "TCP LISTENER" "TCP SERVICE"  "UDP SERVICE" "UDP LISTENER" 
AT+QIOPEN=<contextID>,<connectID>,<service_type>,<IP_address>/<domain_name>,<remote_port>[,<local_port>[,<access_mode>]]
only buffer acces mode or direct push*/

cmError_t cellModem_SocketOpen(cmData_t *obj, socketData_t *config){
    cmError_t retVal;
    char str[256]="AT+QIOPEN=";
    char *ptr = NULL;

      if((config->serviceType != NULL) &&
        (config->remip != NULL)        &&
        (config->remPort != NULL)      &&
        (config->locPort != NULL) ){

        ptr = strcat(str +10, intToStr(config->contextId) );
        ptr = strcat(ptr ,"," );
        ptr = strcat(ptr , intToStr(config->connectId) );
        ptr = strcat(ptr ,",\"" );
        ptr = strcat(ptr , config->serviceType );
        ptr = strcat(ptr ,"\",\"" );
        ptr = strcat(ptr , config->remip );
        ptr = strcat(ptr ,"\"," );
        ptr = strcat(ptr , config->remPort);
        ptr = strcat(ptr ,"," );
        ptr = strcat(ptr , config->locPort);
        ptr = strcat(ptr ,"," );
        ptr = strcat(ptr , intToStr(config->accessMode));
        ptr = strcat(ptr ,"\r" );
      }else{
          retVal = CM_FAIL;
      }
    retVal = cmSendCmd(obj,str , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE ,NULL ,500);

    return retVal;
}

/* get connection status QISTATE*/
cmError_t cellModem_SocketGetServStatus(cmData_t *obj, socketData_t *config){  
    cmError_t retVal;
    char bufferRecv[256];
    char cmd[32]="AT+QISTATE=1,";
    char *ptr = NULL;
    char  buffer[32] ={0};

    sprintf(buffer,"%d\r",config->connectId);
    ptr  = strcat(cmd,buffer);
    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); //150 sg
    
    if(CM_OK == retVal){
        ptr = NULL;
        memset(buffer,0,sizeof(buffer));
        if( (ptr = strstr(bufferRecv,"+QISTATE:")) != NULL ){
            if(config != NULL){
                ptr = extractBetween(bufferRecv,' ',',',buffer,0);
                config->connectId = (int)atoi(buffer);
                ptr = extractBetween(bufferRecv,'"','"',buffer,0); // service type
                ptr = extractBetween(ptr,'"','"',config->ipAddres,2);

                if((config->remPort != NULL) || (config->locPort != NULL)){
                    memset((void*)config->remPort,0,sizeof(config->remPort));
                    memset((void*)config->remPort,0,sizeof(config->locPort));
                    ptr = extractBetween(ptr,',',',',config->remPort,0);
                    ptr = extractBetween(ptr,',',',',config->locPort,0); //printf("remPort: %s\n",config->remPort);
                }else{
                    //move pointer
                    ptr = extractBetween(ptr,',',',',NULL,0);
                    ptr = extractBetween(ptr,',',',',NULL,0);
                }
                
                ptr = extractBetween(ptr,',',',',buffer,0);//
                config->state = (int)atoi(buffer); //0: initial 1: opening 2:connected 3:listening 4 : closing
                ptr = extractBetween(ptr,',',',',NULL,0);//
                memset(buffer,0,sizeof(buffer));
                ptr = extractBetween(ptr,',',',',buffer,0);//
                config->serverId = (int)atoi(buffer);
            }
        }else{
            memset((void*)config->remPort,0,sizeof(config->remPort));
            memset((void*)config->locPort,0,sizeof(config->locPort));
            memset((void*)config->ipAddres,0,sizeof(config->ipAddres));
            config->state = 0;
            config->serverId = 0;
        }
    }
    return retVal;
}

/*create  buffer for store msgRecv*/
cmError_t cellModem_SocketGetRecv(cmData_t *obj, socketData_t *config){  
    cmError_t retVal;
    char bufferRecv[1024]={0};
    char cmd[32]="AT+QIRD=";
    char *ptr = NULL;
    char  buffer[1024] ={0};
    config->msgLen = 0;

    sprintf(buffer,"%d,%d\r",config->connectId, (config->buffRcvLen <= 0 ? CM_BUFFER_LEN_DEF :config->buffRcvLen) );
    ptr  = strcat(cmd,buffer);
    
    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,bufferRecv ,2000); //2 sg
    
    if(CM_OK == retVal){
        ptr = NULL;
        memset(buffer,0,sizeof(buffer));
        if(config->msgRecv != NULL){
            memset((void*)config->msgRecv,0,CM_BUFFER_LEN_DEF); 
        }
        if( (ptr = strstr(bufferRecv,"+QIRD:")) != NULL ){
            if(config != NULL){
                ptr = extractBetween(bufferRecv,' ','\r',buffer,0);
                config->msgLen = (size_t)atoi(buffer);

                if( (config->msgLen > 0) && (config->msgRecv != NULL)){
                    memcpy((void*) config->msgRecv,ptr+2 , config->msgLen);
                }
            }
        }else{
           // retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

cmError_t cellModem_SocketConfigSendData(cmData_t *obj,socketData_t *config ){
    cmError_t retVal;
    char cmd[1024]="AT+QISEND=";  // max 160 char, pending controller
    char *ptr = NULL;
    char  buffer[32] ={0};

    if(config->dataToSend != NULL)
        sprintf(buffer,"%d,%d\r",config->connectId, strlen(config->dataToSend));

    ptr = strcat(cmd+9, buffer);
    //printf("ptr: [%s]",ptr);

    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :">" ,NULL ,300);/*120 sg*/
    return retVal;
}

/*For its correct operation, it must first be used cellModem_SmsSendNumber*/
cmError_t cellModem_SocketSendData(cmData_t *obj, socketData_t *config){
    cmError_t retVal;
    char cmd[1024] ={0}; 
    if(config->dataToSend != NULL)
        memcpy((void*)cmd,(void*)config->dataToSend, strlen(config->dataToSend));

    //\x1A  = ctrl+z = 0x1A para que salga el mensaje
    strcat(cmd, "\r");

   /*is wait block*/
    retVal = cmSendCmd(obj,(char*)cmd , NULL ,NULL ,0);/*4 sg*/
    retVal = cmSendCmd(obj,"\x1A\r" , (0 == cellModem_GetatvFormat(obj) )? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :"SEND OK\r\n"  ,NULL ,4000);/*4 sg*/
    //cmSendDirectUart(obj,"\x1A");
    return retVal;
}

static char* intToStr(int value){
    static char buffer[16]={0};
    memset(buffer,0,16);
    sprintf(buffer,"%d",value);
    return buffer;
}

char* cellModem_GetBufferResponse(cmData_t *obj ){
    return (char*)obj->rxData.buffer;
}

/*asegurarse que CELLMODEM_CMD_GENERAL_RESPONSE sea el correcto*/
void cellModem_SetMatchResponseFromIsr(cmData_t *obj,  char *matchResponseFromIsr ){
    if(matchResponseFromIsr != NULL)
        obj->matchResponseFromIsr = matchResponseFromIsr;
}

int cellModem_GetatvFormat(cmData_t *obj ){
    return  obj->DataPrivate.atvFormat ;
}
void cellModem_SetatvFormat(cmData_t *obj , int value){
    obj->DataPrivate.atvFormat = (0 == value ) ? value : 1;;
}


cmError_t cellModem_UrcGetDataIncomingConnection(cmData_t *obj, asynEventIncomingConn_t *getDataIncomingConn){
    cmError_t retVal= CM_FAIL;
    char bufferRecv[CM_BUFFER_LEN_DEF] = {0};
    char *ptr;
    char bufferAux[128] = {0};

    if(cmIsAsyncEvent(obj)){
        ptr = cmGetBufferAsyncEvent(obj);
        memcpy(bufferRecv, ptr, strlen(ptr));        
        
        if( (ptr = strstr(bufferRecv,": \"incoming\"")) != NULL ){
            ptr = extractBetween(bufferRecv,',',',', bufferAux, 0);
            getDataIncomingConn->info.connectId = atoi(bufferAux);

            memset((void*)bufferAux,0,sizeof(bufferAux));
            ptr = extractBetween(ptr,',',',', bufferAux, 0);
            getDataIncomingConn->serverId = atoi(bufferAux);


            ptr = extractBetween(ptr,'"','"', getDataIncomingConn->remoteIp, 0);
            
            memset((void*)bufferAux,0,sizeof(bufferAux));
            ptr = extractBetween(ptr,',','\r', bufferAux, 0);
            getDataIncomingConn->remotePort = (uint16_t)atoi(bufferAux);
            
            retVal = CM_OK;
        }else{
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

cmError_t cellModem_UrcGetClosedConnection(cmData_t *obj, asynEventURC_t *getDataClosedConn){
    cmError_t retVal= CM_FAIL;
    char bufferRecv[CM_BUFFER_LEN_DEF] = {0};
    char *ptr=NULL;
    char bufferAux[32] = {0};

    if(cmIsAsyncEvent(obj)){
        ptr = cmGetBufferAsyncEvent(obj);
        memcpy(bufferRecv, ptr, strlen(ptr));        
        if( (ptr = strstr(bufferRecv,": \"closed\"")) != NULL ){
            ptr = extractBetween(ptr,',','\r', bufferAux, 0);
            getDataClosedConn->connectId = atoi(bufferAux);
            retVal = CM_OK;
        }else{
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

cmError_t cellModem_UrcGetRecvConnection(cmData_t *obj, asynEventURC_t *getRecvConn){
    cmError_t retVal= CM_FAIL;
    char bufferRecv[CM_BUFFER_LEN_DEF] = {0};
    char *ptr=NULL;
    char bufferAux[32] = {0};

    if(cmIsAsyncEvent(obj)){
        ptr = cmGetBufferAsyncEvent(obj);
        memcpy(bufferRecv, ptr, strlen(ptr));        
        if( (ptr = strstr(bufferRecv,": \"recv\"")) != NULL ){
            ptr = extractBetween(ptr,',','\r', bufferAux, 0);
            getRecvConn->connectId = atoi(bufferAux);
            retVal = CM_OK;
        }else{
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

cmError_t cellModem_SocketClose(cmData_t *obj, int connectId){  
    cmError_t retVal = CM_FAIL;
    char cmd[32]="AT+QICLOSE=";
    char  buffer[8] ={0};
    
    sprintf(buffer,"%d\r",connectId );
    strcat(cmd,buffer);
    retVal = cmSendCmd(obj,cmd , (0 == cellModem_GetatvFormat(obj) ) ? CELLMODEM_CMD_GENERAL_RESPONSE_ATV0 :CELLMODEM_CMD_GENERAL_RESPONSE  ,NULL ,10000); //10 sg
    
    return retVal;
}

/*search string between iniit and end and store in match, return in end position*/
static char * extractBetween(char *src , char init, char end, char* outPutMatch,int offset ){
    char *pInit, *pEnd;
    static char * ret = NULL;
    
    if(src != NULL){
            if( (pInit = strchr(src+offset,init)) != NULL){
                if( (pEnd = strchr(pInit + 1,end)) != NULL){
                    if( outPutMatch != NULL){
                        strncpy(outPutMatch,pInit + 1 , (size_t)(pEnd -pInit -1) );
                    }
                    ret = pEnd;
                }
            }
    }
    return ret;
}