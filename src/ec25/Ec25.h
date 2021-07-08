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

#include "atCmdDevice.h"
#include <stdint.h>
#include "stddef.h"
#include "stdbool.h"


#define EC25_CMD_GENERAL_RESPONSE                   "\r\nOK\r\n"   /*!< format ATV1 */
#define EC25_CMD_GENERAL_RESPONSE_ATV0              "0\r\n"
#define EC25_CMD_APN_DEFAULT                        "AT+QICSGP=1,1,\"VATIA.COMCEL.COM.CO\",\"CLARO\",\"COMCEL\",0\r"  // APN DEBE SER VARIABLE , 0 puede ser 3

#define EC25_SOCKET_SRV_TYPE_TCP                       "TCP"  //Start a TCP connection as a client
#define EC25_SOCKET_SRV_TYPE_UDP                       "UDP" //Start a UDP connection as a client
#define EC25_SOCKET_SRV_TYPE_TCP_SERVICE               "TCP SERVICE" //Start a TCP service.
#define EC25_SOCKET_SRV_TYPE_TCP_INCOMMING             "TCP INCOMMING" //Start a TCP connection accepted by a TCP server
#define EC25_SOCKET_SRV_TYPE_TCP_LISTENER              "TCP LISTENER" //Start a TCP server to listen to TCP connection
#define EC25_SOCKET_SRV_TYPE_UDP_SERVICE               "UDP SERVICE" //Start a UDP service
#define EC25_SOCKET_SRV_TYPE_UDP_LISTENER              "UDP LISTENER" //Start a UDP server to listen to UDP connection
#define EC25_SOCKET_SRV_TYPE_UDP_INCOMMING             "UDP INCOMMING" //Start a UDP connection accepted by a UDP server

typedef struct EC25Config{
    char *apn;
    char *apnUser;
    char *apnPassw;
    int  connectId;
    int  contextId;
    int  contextState;
    int  contextType;
    char contextIpAddres[16];
}EC25Config_t;

/** @brief Macro that can be used to initialize EC25Config_t instance*/
#define EC25_INITIALIZER_CONFIG  { NULL, NULL, NULL , 0, 0, 0, 0, "\0"}

typedef struct socketData{

    char *remip;
    char *remPort;
    char *locPort;
    char *serviceType;
    int accessMode;
    char* msgRecv;
    size_t msgLen;
    size_t buffRcvLen;
    int  connectId;
    int  contextId;
    char ipAddres[16];
}socketData_t;

/** @brief Macro that can be used to initialize socketData_t instance*/
#define EC25_INITIALIZER_SOCKET_DATA  { NULL, NULL, NULL, NULL, 0, 0, 0,0, 0,0, "\0"}

typedef struct netInfo {
    char accessTechnSel[20];
    char operator[8];//numeric
    char band[24];
    char channelId[8];//numeric
}netInfo_t;

/** @brief Macro that can be used to initialize netInfo_t instance*/
#define EC25_INITIALIZER_NETINFO { "\0", "\0", "\0", "\0"}   

typedef struct simCard {
    char manufactorId[10];
    char deviceModel[10];
    char firmwareVersion[30];
}modemInfo_t;

/** @brief Macro that can be used to initialize modemInfo_t instance*/
#define EC25_INITIALIZER_MODEMINFO { "\0", "\0", "\0"}   

typedef struct netRegStatus {
    int n;
    int  stat;
}netRegStatus_t;

/** @brief Macro that can be used to initialize netRegStatus_t instance*/
#define EC25_INITIALIZER_NETREGSTATUS { 0, 0}   


typedef struct netOperatorStatus_t {
    int mode;
    int  format;
    char operator[32];
    int act;   /*!< Access technology selected*/
}netOperatorStatus_t;

/** @brief Macro that can be used to initialize netRegStatus_t instance*/
#define EC25_INITIALIZER_NETOPERATOR_STATUS { 0, 0, "\0",0}   

typedef struct EC25Data{
    int DataPrivate; /*private data internal implementation*/

}EC25Data_t;

/** @brief Macro that can be used to initialize EC25Data_t instance*/





// prototype

cmError_t EC25_InitModem(cmData_t *obj,char *inputBuffer,size_t len ,void *GetTickCountMs,void *OutputFunction, char *matchResponseFromIsr);
cmError_t EC25_SendRaw(cmData_t *obj, char * cmd);
cmError_t EC25_SetApn(cmData_t *obj, EC25Config_t *config);
cmError_t EC25_SetContextProfile(cmData_t *obj, EC25Config_t *config);

cmError_t EC25_GetSignalQuality(cmData_t *obj ,uint8_t *rssi );
cmError_t EC25_GetNetworkInformation(cmData_t *obj, netInfo_t *getNetinfo);
cmError_t EC25_GetModuleInformation(cmData_t *obj, modemInfo_t *getModemInfo);
cmError_t EC25_GetImei(cmData_t *obj,  char *getImei);
cmError_t EC25_GetImsi(cmData_t *obj,  char *getImsi);
cmError_t EC25_GetIccid(cmData_t *obj,  char *getIccid);
cmError_t EC25_SetATE(cmData_t *obj, uint8_t echoEn);
cmError_t EC25_SetATV(cmData_t *obj, uint8_t value);
cmError_t EC25_GetCREG(cmData_t *obj, netRegStatus_t *netRegStatus);
cmError_t EC25_GetCOPS(cmData_t *obj, netOperatorStatus_t *netOperatorStat);
cmError_t EC25_GetCPIN(cmData_t *obj, char *getCode);

void EC25_SetMatchResponseFromIsr(cmData_t *obj,  char *matchResponseFromIsr );
cmError_t EC25_GetContextProfile(cmData_t *obj, EC25Config_t *config);
cmError_t EC25_SetDeactContextProfile(cmData_t *obj, uint8_t contextID);
cmError_t EC25_SMS_SetMessageFormat(cmData_t *obj, uint8_t mode);
cmError_t EC25_SMS_SetTextModeParam(cmData_t *obj);
cmError_t EC25_SMS_SetTeCharacterGsm(cmData_t *obj);
cmError_t EC25_SMS_SendTextMessage(cmData_t *obj, const char *msg);
cmError_t EC25_SetOpenSocket(cmData_t *obj, socketData_t *config);
cmError_t EC25_GetSocketServStatus(cmData_t *obj, socketData_t *config);
cmError_t EC25_GetSocketRecv(cmData_t *obj, socketData_t *config);

char* EC25_GetBufferResponse(cmData_t *obj );
int EC25_GetatvFormat(cmData_t *obj );
void EC25_SetatvFormat(cmData_t *obj , int value);

void EC25_IsrRx(cmData_t *obj, const char rxChar);


#ifdef __cplusplus
}
#endif
#endif /*end _EC25_H_*/