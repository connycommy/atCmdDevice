
#include "TestProductionCode.h"
#include "unity.h"
#include "unity_internals.h"
#include "testMockCommands.h"
#include "string.h"   /*memset*/

/* sometimes you may want to get at local data in a module.
 * for example: If you plan to pass by reference, this could be useful
 * however, it should often be avoided */

char inputBuffer[1024];
cmData_t cmData = CM_DATA_INITIALIZER ;

char *MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;

void setUp(void)
{
  /* This is run before EACH TEST */
}

void tearDown(void)
{    
    //TEST_IGNORE_MESSAGE("ignorado");
}


void test_modem_initializer(){
    cmError_t retValue = CM_FAIL;
    /*lauch fail -1 because not initialize correctly*/
    retValue = cellModem_Init(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,NULL,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_FAIL,retValue);

    retValue = cellModem_Init(NULL,inputBuffer,CM_BUFFER_LEN_DEF,NULL,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_FAIL,retValue);

    retValue = cellModem_Init(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,GetTickCountMs,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetNetworkInformation(){
    cmError_t retValue = CM_FAIL;
    netInfo_t getnetInfo = NETINFO_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_QNWINFO_RESPONSE;
    retValue = cellModem_GetNetworkInformation(&cmData, &getnetInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("FDD LTE",getnetInfo.accessTechnSel);
    TEST_ASSERT_EQUAL_STRING("732101",getnetInfo.operator);
    TEST_ASSERT_EQUAL_STRING("LTE BAND 7",getnetInfo.band);
    TEST_ASSERT_EQUAL_STRING("3075",getnetInfo.channelId);

    memset(&getnetInfo,0,sizeof(getnetInfo));
    MockResponseExpected = CM_CMD_MOCK_QNWINFO_RESPONSE2_ECHO;
    retValue = cellModem_GetNetworkInformation(&cmData, &getnetInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("EDGE",getnetInfo.accessTechnSel);
    TEST_ASSERT_EQUAL_STRING("732101",getnetInfo.operator);
    TEST_ASSERT_EQUAL_STRING("GSM 1900",getnetInfo.band);
    TEST_ASSERT_EQUAL_STRING("583",getnetInfo.channelId);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetModuleInformation(){
    cmError_t retValue = CM_FAIL;
    modemInfo_t getModemInfo = MODEMINFO_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_ATI_RESPONSE2_ECHO;//CM_CMD_MOCK_ATI_RESPONSE;
    retValue = cellModem_GetModuleInformation(&cmData, &getModemInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("Quectel",getModemInfo.manufactorId);
    TEST_ASSERT_EQUAL_STRING("BG96",getModemInfo.deviceModel);
    TEST_ASSERT_EQUAL_STRING("Revision: BG96MAR02A07M1G",getModemInfo.firmwareVersion);

    MockResponseExpected = CM_CMD_MOCK_ATI_RESPONSE;
    retValue = cellModem_GetModuleInformation(&cmData, &getModemInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("Quectel",getModemInfo.manufactorId);
    TEST_ASSERT_EQUAL_STRING("EC25",getModemInfo.deviceModel);
    TEST_ASSERT_EQUAL_STRING("Revision: EC25AUXGAR08A03M1G",getModemInfo.firmwareVersion);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetImei(){
    cmError_t retValue = CM_FAIL;
    char getImei[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_GSN_RESPONSE;

    retValue = cellModem_GetImei(&cmData, getImei);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("862708040783073",getImei);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetImsi(){
    cmError_t retValue = CM_FAIL;
    char getImsi[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_IMSI_RESPONSE;

    retValue = cellModem_GetImsi(&cmData, getImsi);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("732101596840439",getImsi);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetIccid(){
    cmError_t retValue = CM_FAIL;
    char getIccid[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_CCID_RESPONSE;

    retValue = cellModem_GetIccid(&cmData, getIccid);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("89571015025068994296",getIccid);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetSignalQuality(){
    cmError_t retValue = CM_FAIL;
    uint8_t rssi = 0;

    MockResponseExpected = CM_CMD_MOCK_GSQ_RESPONSE;
    retValue = cellModem_GetSignalQuality(&cmData, &rssi);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_UINT8(28,rssi);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SetATE(){
    cmError_t retValue = CM_FAIL;

    MockResponseExpected = CM_CMD_MOCK_ATE1_RESPONSE;
    retValue = cellModem_SetATE(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    MockResponseExpected = CM_CMD_MOCK_ATE0_RESPONSE;
    retValue = cellModem_SetATE(&cmData,0);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SetATV(){
    cmError_t retValue = CM_FAIL;

    MockResponseExpected = CM_CMD_MOCK_ATV1_RESPONSE;
    retValue = cellModem_SetATV(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*se cambia respuesta de la Irs por que se cambio el formato de respuesta a  ATV0*/
    //MockResponseExpected = CM_CMD_MOCK_ATV0_RESPONSE;
    //retValue = cellModem_SetATV(&cmData,0);
    //TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*vuelve init a respuesta ATV1 o enviar literal ATV1*/
    MockResponseExpected = CM_CMD_MOCK_ATV1_RESPONSE;
    retValue = cellModem_SetATV(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  

  
}

void test_modem_GetCREG(){
    cmError_t retValue = CM_FAIL;
    netRegStatus_t netRegSta=NETREGSTATUS_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_CREG_RESPONSE;
    retValue = cellModem_GetCREG(&cmData,&netRegSta);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_INT(0,netRegSta.n);
    TEST_ASSERT_EQUAL_INT(1,netRegSta.stat);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetCOPS(){
    cmError_t retValue = CM_FAIL;
    netOperatorStatus_t netOperatorStat = NETOPERATORSTATUS_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_COPS_RESPONSE;
    retValue = cellModem_GetCOPS(&cmData,&netOperatorStat);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_INT(0,netOperatorStat.mode);
    TEST_ASSERT_EQUAL_INT(0,netOperatorStat.format);
    TEST_ASSERT_EQUAL_STRING("#CoberturaCLARO Claro",netOperatorStat.operator);
    TEST_ASSERT_EQUAL_INT(7,netOperatorStat.act);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetCPIN(){
    cmError_t retValue = CM_FAIL;
    char getCode[32]={0};

    MockResponseExpected = CM_CMD_MOCK_CPIN_RESPONSE;
    retValue = cellModem_GetCPIN(&cmData,getCode);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("READY",getCode);

    memset(getCode,0,sizeof(getCode));
    MockResponseExpected = CM_CMD_MOCK_CPIN_RESPONSE_BAD;
    retValue = cellModem_GetCPIN(&cmData,getCode);
    TEST_ASSERT_EQUAL_INT32(CM_ERR_INVALID_RESPONSE,retValue);
    //TEST_ASSERT_EQUAL_STRING("READY",getCode);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  

}

void test_modem_SetApn(){
    //TEST_IGNORE_MESSAGE("This Test Was Ignored, This Function Has Not Been Tested, Needs To Be Implemented");
    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config =CELLMODEMCONFIG_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    config.apn ="VATIA.COMCEL.COM.CO";
    config.apnPassw = "CLARO";
    config.apnUser ="COMCEL";
    retValue = cellModem_SetApn(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SetContextProfile(){
    //TEST_IGNORE_MESSAGE("This Test Was Ignored, This Function Has Not Been Tested, Needs To Be Implemented");
    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config =CELLMODEMCONFIG_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    config.contextId = 1;
    retValue = cellModem_SetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_GetContextProfile(){
    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config = CELLMODEMCONFIG_INITIALIZER;

    MockResponseExpected = CM_CMD_MOCK_QIACT_GET_RESPONSE;
    retValue = cellModem_GetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
 
    TEST_ASSERT_EQUAL_INT32(1, config.contextId);
    TEST_ASSERT_EQUAL_INT32(1, config.contextState);
    TEST_ASSERT_EQUAL_INT32(1, config.contextType);
    TEST_ASSERT_EQUAL_STRING("10.101.234.65",config.contextIpAddres);

/*respuesta mala,no se usan datos*/
    memset(&config, 0 ,sizeof(config));
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_GetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SetDeactContextProfile(){
    cmError_t retValue = CM_FAIL;
  
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SetDeactContextProfile(&cmData, 1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    MockResponseExpected = CM_CMD_MOCK_ERROR_GENERAL_RESPONSE;
    retValue = cellModem_SetDeactContextProfile(&cmData, 1);
    TEST_ASSERT_EQUAL_INT32(CM_ERR_INVALID_RESPONSE,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SmsSetMessageFormat(){
    cmError_t retValue = CM_FAIL;
    
    /*DPU format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSetMessageFormat(&cmData, 0);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*text format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSetMessageFormat(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SmsSetTextModeParam(){
    cmError_t retValue = CM_FAIL;
    
    /*text format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSetTextModeParam(&cmData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SmsSetTeCharacterGsm(){
    cmError_t retValue = CM_FAIL;
    
    /*GSM default alphabet */
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSetTeCharacterGsm(&cmData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SmsSendNumber(){
    cmError_t retValue = CM_FAIL;
    
    /*GSM default alphabet */
    MockResponseExpected = ">";//CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSendNumber(&cmData,"3146644565");
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}

void test_modem_SmsSendTextMessage(){
    cmError_t retValue = CM_FAIL;
    
    /*GSM default alphabet */
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SmsSendTextMessage(&cmData,"Hellow world!");
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
}


void test_modem_SocketOpen(){
    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config =CELLMODEMCONFIG_INITIALIZER;
    socketData_t configSocket =SOCKETDATA_INITIALIZER;
    MockResponseExpected = CM_CMD_MOCK_QIOPEN_RESPONSE;
    configSocket.accessMode = 0;
    configSocket.contextId = config.contextId = 1;
    configSocket.connectId = config.connectId = 3;
    configSocket.serviceType = CELLMODEM_SOCKET_SRV_TYPE_UDP;
    strcpy(configSocket.remip , "127.0.0.1");
    strcpy(configSocket.remPort , "0");
    strcpy(configSocket.locPort , "2020" );
    retValue = cellModem_SocketOpen(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  
    
}

void test_modem_SocketGetServStatus(){
    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config =CELLMODEMCONFIG_INITIALIZER;
    socketData_t configSocket =SOCKETDATA_INITIALIZER;


    MockResponseExpected = CM_CMD_MOCK_QISTATE_RESPONSE;

    configSocket.connectId = config.connectId = 3;
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(3, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("10.106.6.155",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("0",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("5500",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(3,configSocket.state);

    configSocket.connectId = config.connectId = 11;
    MockResponseExpected = CM_CMD_MOCK_QISTATE_RESPONSE_INCOMING_CLIENT;
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("192.168.32.68",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("54842",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("5500",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(2,configSocket.state);
    
    configSocket.connectId = config.connectId = 11;
    MockResponseExpected = CM_CMD_MOCK_QISTATE_RESPONSE2_ECHO;
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(3, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("10.101.234.65",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("0",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("2020",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(4,configSocket.state);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  

    memset(configSocket.locPort,0,8);
    configSocket.connectId = config.connectId = 3;
    MockResponseExpected = CM_CMD_MOCK_QISTATE_SERIAL_RESPONSE;
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(3, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("192.168.32.48",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("6000",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("0",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(1,configSocket.state);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));   

        configSocket.connectId = config.connectId = 11;
    MockResponseExpected = "AT+QISTATE=1,11\r\r\nOK\r\n";
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("\0",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("\0",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(0,configSocket.state);
    TEST_ASSERT_EQUAL_INT(0,configSocket.serverId);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));  

    configSocket.connectId = config.connectId = 11;
    MockResponseExpected = "AT+QISTATE=1,11\r\r\n+QISTATE: 11,\"TCP INCOMING\",\"192.168.32.48\",54707,2020,4,1,3,0,\"uart1\"\r\n\r\nOK\r\n";
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);

    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("192.168.32.48",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("54707",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("2020",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(4,configSocket.state);
    TEST_ASSERT_EQUAL_INT(3,configSocket.serverId);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));

        configSocket.connectId = config.connectId = 11;
    MockResponseExpected = "AT+QISTATE=1,11\r\r\n+QISTATE: 11,\"TCP INCOMING\",\"192.168.32.48\",54707,2020,4,1,5,0,\"uart1\"\r\n\r\nOK\r\n";
    retValue = cellModem_SocketGetServStatus(&cmData,&configSocket);

    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("192.168.32.48",configSocket.ipAddres);
    TEST_ASSERT_EQUAL_STRING("54707",configSocket.remPort);
    TEST_ASSERT_EQUAL_STRING("2020",configSocket.locPort);
    TEST_ASSERT_EQUAL_INT(4,configSocket.state);
    TEST_ASSERT_EQUAL_INT(5,configSocket.serverId);
    TEST_ASSERT_EQUAL_INT(0, cmIsAsyncEvent(&cmData));

}


void test_modem_SocketGetRecv(){
    char bufferRecv[CM_BUFFER_LEN_DEF] ;


    cmError_t retValue = CM_FAIL;
    cellModemConfig_t config =CELLMODEMCONFIG_INITIALIZER;
    socketData_t configSocket =SOCKETDATA_INITIALIZER;
    configSocket.msgRecv = bufferRecv;
    configSocket.buffRcvLen = CM_BUFFER_LEN_DEF;
   MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE;

    configSocket.connectId = config.connectId = 3;
    retValue = cellModem_SocketGetRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(6, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("julian",configSocket.msgRecv);

  
   // memset(bufferRecv,0,sizeof(bufferRecv));
    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_LONG;

    configSocket.connectId = config.connectId = 11;
    
    retValue = cellModem_SocketGetRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(112, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("7EA036610330A166E6E7006128A109060760857405080102A203020101A305A10302010DBE0F040D0800065F04001802201388FA0074EA7E",configSocket.msgRecv);

    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_EMPTY_BUFF;

    configSocket.connectId = config.connectId = 3;
    retValue = cellModem_SocketGetRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(0, configSocket.msgLen);
    //TEST_ASSERT_EQUAL_STRING(" ",configSocket.msgRecv);

    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_ECHO;
    configSocket.connectId = config.connectId = 3;
    retValue = cellModem_SocketGetRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(6, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("julian",configSocket.msgRecv);

    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_STR;
    configSocket.connectId = config.connectId = 3;
    retValue = cellModem_SocketGetRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(246, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("Buenas a todos! Estaba chusmeando el proyecto. Los navigation devices esten teniendo un conjunto de posiciones, que se espera hacer con eso? Ponemos todas las posiciones en el mapa, hacemos un calculo en el front, que representan esas posiciones?",configSocket.msgRecv);
    
  
   }


void test_modem_QIURC_incoming(){
    cmError_t retValue;
    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_INCOMING_RESPONSE;

    retValue = cellModem_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_INCOMING_RESPONSE,cmGetBufferRecv(&cmData));


    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_FULL_RESPONSE;
    retValue = cellModem_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_FULL_RESPONSE,cmGetBufferRecv(&cmData));

    
    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_CLOSED_RESPONSE;

    retValue = cellModem_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_CLOSED_RESPONSE,cmGetBufferRecv(&cmData));

    
    MockResponseExpected = CM_CMD_MOCK_QIURC_RECV_RESPONSE;

    retValue = cellModem_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_RECV_RESPONSE,cmGetBufferRecv(&cmData));
    (void)retValue;

}
/*The data must be previously loaded
   // mempcy( cmData.rxData.bufferAsyncEvent , ": \"incoming\",7,3,\"192.168.32.68\",41047\r\n",47);
   // cmData.rxData.asyncEvent = 1;
   */
void test_modem_QIURC_incomingConnection(){
    cmError_t retValue = CM_FAIL; 
    asynEventIncomingConn_t getDataIncomingConn = ASYNCEVENT_INCOMING_CONN_INITIALIZER;

    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    retValue = cellModem_UrcGetDataIncomingConnection(&cmData, &getDataIncomingConn);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, getDataIncomingConn.info.connectId);
    TEST_ASSERT_EQUAL_INT32(1, getDataIncomingConn.serverId);
    TEST_ASSERT_EQUAL_INT32(63713, getDataIncomingConn.remotePort);
    TEST_ASSERT_EQUAL_STRING("192.168.32.68", getDataIncomingConn.remoteIp);
}

void test_modem_QIURC_closedConnection(){
    cmError_t retValue = CM_FAIL; 
    asynEventURC_t getDataClosedConn ;

    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    retValue = cellModem_UrcGetClosedConnection(&cmData, &getDataClosedConn);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(10, getDataClosedConn.connectId);
}

void test_modem_QIURC_RecvConnection(){
    cmError_t retValue = CM_FAIL; 
    asynEventURC_t getRecvConn ;

    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    retValue = cellModem_UrcGetRecvConnection(&cmData, &getRecvConn);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(10, getRecvConn.connectId);
}

void test_modem_SocketClose(){
    cmError_t retValue = CM_FAIL;
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = cellModem_SocketClose(&cmData,10);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_SocketSendData(){
    cmError_t retValue = CM_FAIL;
    socketData_t socketData = SOCKETDATA_INITIALIZER;
    char data[1024] = "hola lalala";
    MockResponseExpected = ">";//CM_CMD_MOCK_GENERAL_RESPONSE;
    socketData.connectId = 1;
    //socketData.serviceType = cellModem_SOCKET_SRV_TYPE_TCP;
    socketData.dataToSend = data;
    retValue = cellModem_SocketConfigSendData(&cmData,&socketData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    retValue = CM_FAIL;
    MockResponseExpected = CM_CMD_MOCK_QISEND_OK_RESPONSE;
    retValue = cellModem_SocketSendData(&cmData,&socketData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

/*=========================================================
                    Wrappers
 ==========================================================
*/

//---systick generator-----------------------------------------------------
uint32_t GetTickCountMs(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_nsec / 1000000) + ((uint32_t)ts.tv_sec * 1000);
}

//-----------uart tx mock -------------------------------------------------
void OutputFunction( void *sp, char c){
	int i = 0; 
    (void)sp;
   // char ResponseExpected[100] = {CM_CMD_MOCK_ATI_RESPONSE};//{CM_CMD_MOCK_GENERAL_RESPONSE};
	if(c != '\r')
    	printf(KYEL "%c",c); /*no print \r in console */
	
	if(c == '\r') {	 /*if send command to module*/
		printf(KWHT); /*black color */
       
        while(MockResponseExpected[i] != '\0') { /*then the module response is pass to isr char by char */
		 	// para nonblocking test colocar un wait pequeño
            cellModem_IsrRx(&cmData, MockResponseExpected[i]); 
			i++;
		}
        /*print command and response*/
        printf(" -->");
		printf(KGRN "%s",MockResponseExpected);  
        printf(KNRM); /*white color */
	}
}