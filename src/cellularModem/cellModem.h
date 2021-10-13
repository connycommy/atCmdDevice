/**
 * @file cellModem.h
 * @author Julian Bustamante Narvaez
 * @brief cellular modem (cm) library for Quectel
 * @version 2.0.8
 * @date 2021-10-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _CELLMODEM_H_
#define _CELLMODEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "atCmdDevice.h"
#include <stdint.h>
#include "stddef.h"
#include "stdbool.h"

#define CELLMODEM_VERSION    	"2.0.8"
#define CELLMODEM_CAPTION     	"cellModem " CELLMODEM_VERSION

#define CELLMODEM_CMD_GENERAL_RESPONSE                   "\r\nOK\r\n"   /*!< format ATV1 */
#define CELLMODEM_CMD_GENERAL_RESPONSE_ATV0              "0\r\n"
#define CELLMODEM_CMD_APN_DEFAULT                        "AT+QICSGP=1,1,\"VATIA.COMCEL.COM.CO\",\"CLARO\",\"COMCEL\",0\r" 

#define CELLMODEM_SOCKET_SRV_TYPE_TCP                       "TCP"  //Start a TCP connection as a client
#define CELLMODEM_SOCKET_SRV_TYPE_UDP                       "UDP" //Start a UDP connection as a client
#define CELLMODEM_SOCKET_SRV_TYPE_TCP_SERVICE               "TCP SERVICE" //Start a TCP service.
#define CELLMODEM_SOCKET_SRV_TYPE_TCP_INCOMING             "TCP INCOMING" //Start a TCP connection accepted by a TCP server
#define CELLMODEM_SOCKET_SRV_TYPE_TCP_LISTENER              "TCP LISTENER" //Start a TCP server to listen to TCP connection
#define CELLMODEM_SOCKET_SRV_TYPE_UDP_SERVICE               "UDP SERVICE" //Start a UDP service
#define CELLMODEM_SOCKET_SRV_TYPE_UDP_LISTENER              "UDP LISTENER" //Start a UDP server to listen to UDP connection
#define CELLMODEM_SOCKET_SRV_TYPE_UDP_INCOMING             "UDP INCOMING" //Start a UDP connection accepted by a UDP server

typedef struct cellModemConfig{
    char *apn;
    char *apnUser;
    char *apnPassw;
    int  connectId;
    int  contextId;
    int  contextState;
    int  contextType;
    char contextIpAddres[16];
}cellModemConfig_t;

/** @brief Macro that can be used to initialize cellModemConfig_t instance*/
#define CELLMODEMCONFIG_INITIALIZER  { NULL, NULL, NULL , 0, 0, 0, 0, "\0"}

typedef struct socketData{

    char remip[16];
    char remPort[8];
    char locPort[8];
    char *serviceType;
    char *dataToSend;
    char* msgRecv;   /*Note: len minimun CM_BUFFER_LEN_DEF -> 1024*/
    size_t msgLen;
    size_t buffRcvLen;
    int accessMode;
    int  connectId;
    int  contextId;
    int state;
    int serverId;
    char ipAddres[16];
}socketData_t;

/** @brief Macro that can be used to initialize socketData_t instance*/
#define SOCKETDATA_INITIALIZER  { "\0","\0","\0", NULL, NULL,NULL, 0, 0, 0,0, 0,0,0,"\0"}

typedef struct netInfo {
    char accessTechnSel[32];
    char operator[8];//numeric
    char band[32];
    char channelId[8];//numeric
}netInfo_t;

/** @brief Macro that can be used to initialize netInfo_t instance*/
#define NETINFO_INITIALIZER { "\0", "\0", "\0", "\0"}   

typedef struct simCard {
    char manufactorId[16];
    char deviceModel[16];
    char firmwareVersion[32];
}modemInfo_t;

/** @brief Macro that can be used to initialize modemInfo_t instance*/
#define MODEMINFO_INITIALIZER { "\0", "\0", "\0"}   

typedef struct netRegStatus {
    int n;
    int  stat;
}netRegStatus_t;

/** @brief Macro that can be used to initialize netRegStatus_t instance*/
#define NETREGSTATUS_INITIALIZER { 0, 0}   


typedef struct netOperatorStatus_t {
    int mode;
    int  format;
    int act;   /*!< Access technology selected*/
    char operator[32];
}netOperatorStatus_t;

/** @brief Macro that can be used to initialize netOperatorStatus_t instance*/
#define NETOPERATORSTATUS_INITIALIZER { 0, 0, 0, "\0"}   

typedef struct cellModemData{
    int DataPrivate; /*private data internal implementation*/

}cellModemData_t;

/** @brief Macro that can be used to initialize cellModemData_t instance*/


typedef struct asynEventURC {
    int report;
    int  connectId;
}asynEventURC_t;

typedef struct asynEventIncomingData {
    asynEventURC_t info;
    size_t currentRecvLen;
    char* data;
}asynEventIncomingData_t;

typedef struct asynEventIncomingConn_t {
    asynEventURC_t info;
    int serverId;
    char remoteIp[16];
    uint16_t remotePort;
}asynEventIncomingConn_t;

/** @brief Macro that can be used to initialize netRegStatus_t instance*/
#define ASYNCEVENT_INCOMING_CONN_INITIALIZER { {0,0},0,"\0",0}   


// prototype

cmError_t cellModem_Init(cmData_t *obj,char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction, char *matchResponseFromIsr);
cmError_t cellModem_SendRaw(cmData_t *obj, char * cmd);
cmError_t cellModem_SetApn(cmData_t *obj, cellModemConfig_t *config);
cmError_t cellModem_SetContextProfile(cmData_t *obj, cellModemConfig_t *config);

cmError_t cellModem_GetSignalQuality(cmData_t *obj ,uint8_t *rssi );
cmError_t cellModem_GetNetworkInformation(cmData_t *obj, netInfo_t *getNetinfo);
cmError_t cellModem_GetModuleInformation(cmData_t *obj, modemInfo_t *getModemInfo);
cmError_t cellModem_GetImei(cmData_t *obj,  char *getImei);
cmError_t cellModem_GetImsi(cmData_t *obj,  char *getImsi);
cmError_t cellModem_GetIccid(cmData_t *obj,  char *getIccid);
cmError_t cellModem_SetATE(cmData_t *obj, uint8_t echoEn);
cmError_t cellModem_SetATV(cmData_t *obj, uint8_t value);
cmError_t cellModem_GetCREG(cmData_t *obj, netRegStatus_t *netRegStatus);
cmError_t cellModem_GetCOPS(cmData_t *obj, netOperatorStatus_t *netOperatorStat);
cmError_t cellModem_GetCPIN(cmData_t *obj, char *getCode);

void cellModem_SetMatchResponseFromIsr(cmData_t *obj,  char *matchResponseFromIsr );
cmError_t cellModem_GetContextProfile(cmData_t *obj, cellModemConfig_t *config);
cmError_t cellModem_SetDeactContextProfile(cmData_t *obj, uint8_t contextID);
cmError_t cellModem_SmsSetMessageFormat(cmData_t *obj, uint8_t mode);
cmError_t cellModem_SmsSetTextModeParam(cmData_t *obj);
cmError_t cellModem_SmsSetTeCharacterGsm(cmData_t *obj);
cmError_t cellModem_SmsSendNumber(cmData_t *obj, const char *number);
cmError_t cellModem_SmsSendTextMessage(cmData_t *obj, const char *msg);
cmError_t cellModem_SocketOpen(cmData_t *obj, socketData_t *config);
cmError_t cellModem_SocketGetServStatus(cmData_t *obj, socketData_t *config);
cmError_t cellModem_SocketGetRecv(cmData_t *obj, socketData_t *config);

char* cellModem_GetBufferResponse(cmData_t *obj );
int cellModem_GetatvFormat(cmData_t *obj );
void cellModem_SetatvFormat(cmData_t *obj , int value);

cmError_t cellModem_UrcGetDataIncomingConnection(cmData_t *obj, asynEventIncomingConn_t *getDataIncomingConn);
cmError_t cellModem_UrcGetClosedConnection(cmData_t *obj, asynEventURC_t *getDataClosedConn);
cmError_t cellModem_UrcGetRecvConnection(cmData_t *obj, asynEventURC_t *getRecvConn);
cmError_t cellModem_SocketClose(cmData_t *obj, int connectId);
cmError_t cellModem_SocketConfigSendData(cmData_t *obj,socketData_t *config );
cmError_t cellModem_SocketSendData(cmData_t *obj, socketData_t *config);
void cellModem_IsrRx(cmData_t *obj, const char rxChar);



#ifdef __cplusplus
}
#endif
#endif /*end _CELLMODEM_H_*/