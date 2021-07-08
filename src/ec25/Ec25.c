#include "Ec25.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char* extractBetween(char *src , char init, char end, char* outPutMatch,int offset );
static char* intToStr(int value);


cmError_t EC25_InitModem(cmData_t *obj,char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction, char *matchResponseFromIsr){
    if( obj != NULL){
        obj->rxData.buffer = (char*)inputBuffer;
        obj->rxData.len = len;
        obj->tickReadHandler = (cmTickReadHandler_t)GetTickCountMs;
        obj->txHandler = (cmTransmissionHandler_t)OutputFunction;
        obj->matchResponseFromIsr = matchResponseFromIsr;
    }
    return cmInit(obj);
}

cmError_t EC25_GetSignalQuality(cmData_t *obj ,uint8_t *rssi ){
    cmError_t retVal = CM_OK;
    char cmd[15]="AT+CSQ\r";
    char bufferRecv[60];
    char RSSI[10] = "\0";
    char *ptr = NULL;

    retVal = cmSendCmd(obj, cmd, (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if( (ptr = strstr(bufferRecv,"+CSQ:")) != NULL ){
            ptr = extractBetween(bufferRecv,' ',',',RSSI,0);
            *rssi = (uint8_t)atoi(RSSI);
        }else{
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*usage: EC25_SendRaw(&cmData, "AT\r\n") */
cmError_t EC25_SendRaw(cmData_t *obj, char * cmd){
    cmError_t retVal = CM_FAIL;
    if( cmd != NULL)
        retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,300);
    return retVal;
}

cmError_t EC25_SetApn(cmData_t *obj, EC25Config_t *config){
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
          strcat(apn,EC25_CMD_APN_DEFAULT);
      }
    retVal = cmSendCmd(obj,apn , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,500);

    return retVal;
}

/* Activate context profile */
cmError_t EC25_SetContextProfile(cmData_t *obj, EC25Config_t *config){
    cmError_t retVal;
    char cmd[15]="AT+QIACT=";
    char snum[5];
    char *ptr = NULL;
    sprintf(snum,"%d",config->contextId);
    ptr = strcat(cmd + 9 , snum);
    ptr = strcat(ptr, "\r");

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,150000);//150sg

    return retVal;
}

void EC25_IsrRx(cmData_t *obj, const char rxChar){
    cmIsrRx(obj, rxChar); // comentar para nonblocking test wait
}


cmError_t EC25_GetNetworkInformation(cmData_t *obj, netInfo_t *getNetinfo){
    cmError_t retVal;
    char cmd[15]="AT+QNWINFO\r";
    char bufferRecv[60];
    char *ptr;

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        //+QNWINFO: "FDD LTE","732101","LTE BAND 7",3075
        //api bufferready , apiclearready
        if( (ptr = strstr(bufferRecv,"+QNWINFO:")) != NULL ){
            ptr = extractBetween(bufferRecv,'"','"',getNetinfo->accessTechnSel,0);
            ptr = extractBetween(ptr,'"','"',getNetinfo->operator,2);
            ptr = extractBetween(ptr,'"','"',getNetinfo->band,2);
            ptr = extractBetween(ptr,',','\r',getNetinfo->channelId,1);
        }else{
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*return manofactur id, model module and firmware versión*/
cmError_t EC25_GetModuleInformation(cmData_t *obj, modemInfo_t *getModemInfo){
    cmError_t retVal;
    char cmd[15]="ATI\r";
    char bufferRecv[60];
    char *ptr;
    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
            ptr = extractBetween(bufferRecv,'\n','\r',getModemInfo->manufactorId,0);
            ptr = extractBetween(ptr,'\n','\r',getModemInfo->deviceModel,2);
            ptr = extractBetween(ptr,'\n','\r',getModemInfo->firmwareVersion,2);
 
    }
    return retVal;
}

cmError_t EC25_GetImei(cmData_t *obj, char *getImei){
    cmError_t retVal;
    char cmd[15]="AT+GSN\r";
    char bufferRecv[60];

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if(getImei != NULL)
            extractBetween(bufferRecv,'\n','\r',getImei,0);
    }
    return retVal;
}

cmError_t EC25_GetImsi(cmData_t *obj,  char *getImsi){
    cmError_t retVal;
    char cmd[15]="AT+CIMI\r";
    char bufferRecv[60];

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if(getImsi != NULL)
            extractBetween(bufferRecv,'\n','\r',getImsi,0);
    }
    return retVal;
}

cmError_t EC25_GetIccid(cmData_t *obj,  char *getIccid){
    cmError_t retVal;
    char cmd[15]="AT+QCCID\r";
    char bufferRecv[60];

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,bufferRecv ,300);
    if(CM_OK == retVal){
        if(getIccid != NULL)
            extractBetween(bufferRecv,'\n','\r',getIccid,0);
    }
    return retVal;
}

/* Use ATE1 to enable echo mode*/
cmError_t EC25_SetATE(cmData_t *obj, uint8_t echoEn){
    cmError_t retVal;
    char cmd[8]="ATE";
    cmd[3] = (char)( (0 == echoEn ) ? (char)echoEn +'0' : '1');
    cmd[4]  = '\r';
    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,300);
    return retVal;
}

/* Use ATV1 to set the response format 
    value =1 -> OK    <CR><LF><text><CR><LF>
    value = 0 -> 0    <text><CR><LF>  
    Note: should change matchResponseIsr in initModem according to value
*/
cmError_t EC25_SetATV(cmData_t *obj, uint8_t value){
    cmError_t retVal;
    char cmd[8]="ATV";
    cmd[3] = (char)( (0 == value ) ? (char)value +'0' : '1');
    cmd[4]  = '\r';

    EC25_SetatvFormat(obj,value);
    EC25_SetMatchResponseFromIsr(obj, (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ); /*cambia la respuesta esperada de la Isr*/
    retVal = cmSendCmd(obj,cmd , (0 == value )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,NULL ,300);

    return retVal;
}

cmError_t EC25_GetCREG(cmData_t *obj, netRegStatus_t *netRegStatus){
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[10]="AT+CREG?\r";
    char *ptr =NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,300); 
    
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

cmError_t EC25_GetCOPS(cmData_t *obj, netOperatorStatus_t *netOperatorStat){
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[10]="AT+COPS?\r";
    char *ptr =NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,180000); //180 sg
    
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

cmError_t EC25_GetCPIN(cmData_t *obj, char *getCode){
    cmError_t retVal;
    char bufferRecv[60]={0};
    char cmd[10]="AT+CPIN?\r";
    char *ptr = NULL;

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); 
    
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
cmError_t EC25_GetContextProfile(cmData_t *obj, EC25Config_t *config){  
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[16]="AT+QIACT?\r";
    char *ptr = NULL;
    char  buffer[4] ={0};

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); //150 sg
    
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
            retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

/*deactivate context profile*/
cmError_t EC25_SetDeactContextProfile(cmData_t *obj, uint8_t contextID){
    cmError_t retVal;
    char cmd[16]="AT+QIDEACT=";
    cmd[11] = (char)( (0 == contextID ) ? (char)contextID +'0' : '1');
    cmd[12]  = '\r';

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,40000);/*40 sg*/

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
cmError_t EC25_SMS_SetMessageFormat(cmData_t *obj, uint8_t mode){
    cmError_t retVal;
    char cmd[16]="AT+CMGF=";
    cmd[8] = (char)( (0 == mode ) ? (char)mode +'0' : '1');
    cmd[9]  = '\r';

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,300);

    return retVal;
}

cmError_t EC25_SMS_SetTextModeParam(cmData_t *obj){
    // default
    return cmSendCmd(obj,"AT+CSMP=17,167,0,0\r" , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,300);
}

cmError_t EC25_SMS_SetTeCharacterGsm(cmData_t *obj){
    // GSM default alphabet 
    return cmSendCmd(obj,"AT+CSCS=\"GSM\"\r" , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,300);
}

cmError_t EC25_SMS_SendTextMessage(cmData_t *obj, const char *msg){
    cmError_t retVal;
    char cmd[256]="AT+CMGS=\"";  // max 160 char, pending controller
    char *ptr = NULL;

    ptr = strcat(cmd+9, msg);
    ptr = strcat(ptr,"\"\r");

    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) )? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,120000);/*120 sg*/

    return retVal;
}

/*AT+QIOPEN=1,3,"TCP LISTENER","127.0.0.1",0,2020,0
"TCP" "UDP" "TCP LISTENER" "TCP SERVICE"  "UDP SERVICE" "UDP LISTENER" 
AT+QIOPEN=<contextID>,<connectID>,<service_type>,<IP_address>/<domain_name>,<remote_port>[,<local_port>[,<access_mode>]]
only buffer acces mode or direct push*/

cmError_t EC25_SetOpenSocket(cmData_t *obj, socketData_t *config){
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
    retVal = cmSendCmd(obj,str , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE ,NULL ,500);

    return retVal;
}

/* get connection status QISTATE*/
cmError_t EC25_GetSocketServStatus(cmData_t *obj, socketData_t *config){  
    cmError_t retVal;
    char bufferRecv[60];
    char cmd[32]="AT+QISTATE=1,";
    char *ptr = NULL;
    char  buffer[32] ={0};
    
    sprintf(buffer,"%d\r",config->connectId);
    ptr  = strcat(cmd,buffer);
    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,150000); //150 sg
    
    if(CM_OK == retVal){
        ptr = NULL;
        memset(buffer,0,sizeof(buffer));
        if( (ptr = strstr(bufferRecv,"+QISTATE:")) != NULL ){
            if(config != NULL){
                ptr = extractBetween(bufferRecv,' ',',',buffer,0);
                config->connectId = (int)atoi(buffer);
                ptr = extractBetween(bufferRecv,',',',',buffer,0); // service type
                ptr = extractBetween(ptr,'"','"',config->ipAddres,0);
            }
        }else{
           // retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}


cmError_t EC25_GetSocketRecv(cmData_t *obj, socketData_t *config){  
    cmError_t retVal;
    char bufferRecv[1024];
    char cmd[32]="AT+QIRD=";
    char *ptr = NULL;
    char  buffer[1024] ={0};
    
    sprintf(buffer,"%d,%ld\r",config->connectId, (config->buffRcvLen <= 0 ? 1024 :config->buffRcvLen) );
    ptr  = strcat(cmd,buffer);
    
    retVal = cmSendCmd(obj,cmd , (0 == EC25_GetatvFormat(obj) ) ? EC25_CMD_GENERAL_RESPONSE_ATV0 :EC25_CMD_GENERAL_RESPONSE  ,bufferRecv ,2000); //2 sg
    
    if(CM_OK == retVal){
        ptr = NULL;
        memset(buffer,0,sizeof(buffer));
        if( (ptr = strstr(bufferRecv,"+QIRD:")) != NULL ){
            if(config != NULL){
                ptr = extractBetween(bufferRecv,' ','\r',buffer,0);
                config->msgLen = (size_t)atoi(buffer);
                ptr = extractBetween(ptr,'\n','\r',buffer,2);
                if(config->msgRecv != NULL)
                    config->msgRecv = buffer;
            }
        }else{
           // retVal = CM_ERR_INVALID_RESPONSE;
        }
    }
    return retVal;
}

static char* intToStr(int value){
    static char buffer[16]={0};
    memset(buffer,0,16);
    sprintf(buffer,"%d",value);
    return buffer;
}

char* EC25_GetBufferResponse(cmData_t *obj ){
    return (char*)obj->rxData.buffer;
}

/*asegurarse que EC25_CMD_GENERAL_RESPONSE sea el correcto*/
void EC25_SetMatchResponseFromIsr(cmData_t *obj,  char *matchResponseFromIsr ){
    if(matchResponseFromIsr != NULL)
        obj->matchResponseFromIsr = matchResponseFromIsr;
}

int EC25_GetatvFormat(cmData_t *obj ){
    return  obj->DataPrivate.atvFormat ;
}
void EC25_SetatvFormat(cmData_t *obj , int value){
    obj->DataPrivate.atvFormat = (0 == value ) ? value : 1;;
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