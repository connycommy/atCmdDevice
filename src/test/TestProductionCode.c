
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
    retValue = EC25_InitModem(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,NULL,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_FAIL,retValue);

    retValue = EC25_InitModem(NULL,inputBuffer,CM_BUFFER_LEN_DEF,NULL,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_FAIL,retValue);

    retValue = EC25_InitModem(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,GetTickCountMs,OutputFunction,NULL);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_GetNetworkInformation(){
    cmError_t retValue = CM_FAIL;
    netInfo_t getnetInfo = EC25_INITIALIZER_NETINFO;

    MockResponseExpected = CM_CMD_MOCK_QNWINFO_RESPONSE;
    retValue = EC25_GetNetworkInformation(&cmData, &getnetInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("FDD LTE",getnetInfo.accessTechnSel);
    TEST_ASSERT_EQUAL_STRING("732101",getnetInfo.operator);
    TEST_ASSERT_EQUAL_STRING("LTE BAND 7",getnetInfo.band);
    TEST_ASSERT_EQUAL_STRING("3075",getnetInfo.channelId);

}

void test_modem_GetModuleInformation(){
    cmError_t retValue = CM_FAIL;
    modemInfo_t getModemInfo = EC25_INITIALIZER_MODEMINFO;

    MockResponseExpected = CM_CMD_MOCK_ATI_RESPONSE;
    retValue = EC25_GetModuleInformation(&cmData, &getModemInfo);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_STRING("Quectel",getModemInfo.manufactorId);
    TEST_ASSERT_EQUAL_STRING("EC25",getModemInfo.deviceModel);
    TEST_ASSERT_EQUAL_STRING("Revision: EC25AUXGAR08A03M1G",getModemInfo.firmwareVersion);
}

void test_modem_GetImei(){
    cmError_t retValue = CM_FAIL;
    char getImei[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_GSN_RESPONSE;

    retValue = EC25_GetImei(&cmData, getImei);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("862708040783073",getImei);
}

void test_modem_GetImsi(){
    cmError_t retValue = CM_FAIL;
    char getImsi[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_IMSI_RESPONSE;

    retValue = EC25_GetImsi(&cmData, getImsi);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("732101596840439",getImsi);
}

void test_modem_GetIccid(){
    cmError_t retValue = CM_FAIL;
    char getIccid[32]="\0";

    MockResponseExpected = CM_CMD_MOCK_CCID_RESPONSE;

    retValue = EC25_GetIccid(&cmData, getIccid);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("89571015025068994296",getIccid);
}

void test_modem_GetSignalQuality(){
    cmError_t retValue = CM_FAIL;
    uint8_t rssi = 0;

    MockResponseExpected = CM_CMD_MOCK_GSQ_RESPONSE;
    retValue = EC25_GetSignalQuality(&cmData, &rssi);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_UINT8(28,rssi);
}

void test_modem_SetATE(){
    cmError_t retValue = CM_FAIL;

    MockResponseExpected = CM_CMD_MOCK_ATE1_RESPONSE;
    retValue = EC25_SetATE(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    MockResponseExpected = CM_CMD_MOCK_ATE0_RESPONSE;
    retValue = EC25_SetATE(&cmData,0);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_SetATV(){
    cmError_t retValue = CM_FAIL;

    MockResponseExpected = CM_CMD_MOCK_ATV1_RESPONSE;
    retValue = EC25_SetATV(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*se cambia respuesta de la Irs por que se cambio el formato de respuesta a  ATV0*/
    MockResponseExpected = CM_CMD_MOCK_ATV0_RESPONSE;
    retValue = EC25_SetATV(&cmData,0);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*vuelve init a respuesta ATV1 o enviar literal ATV1*/
    MockResponseExpected = CM_CMD_MOCK_ATV1_RESPONSE;
    retValue = EC25_SetATV(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

  
}

void test_modem_GetCREG(){
    cmError_t retValue = CM_FAIL;
    netRegStatus_t netRegSta=EC25_INITIALIZER_NETREGSTATUS;

    MockResponseExpected = CM_CMD_MOCK_CREG_RESPONSE;
    retValue = EC25_GetCREG(&cmData,&netRegSta);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_INT(0,netRegSta.n);
    TEST_ASSERT_EQUAL_INT(1,netRegSta.stat);
}

void test_modem_GetCOPS(){
    cmError_t retValue = CM_FAIL;
    netOperatorStatus_t netOperatorStat = EC25_INITIALIZER_NETOPERATOR_STATUS;

    MockResponseExpected = CM_CMD_MOCK_COPS_RESPONSE;
    retValue = EC25_GetCOPS(&cmData,&netOperatorStat);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    TEST_ASSERT_EQUAL_INT(0,netOperatorStat.mode);
    TEST_ASSERT_EQUAL_INT(0,netOperatorStat.format);
    TEST_ASSERT_EQUAL_STRING("#CoberturaCLARO Claro",netOperatorStat.operator);
    TEST_ASSERT_EQUAL_INT(7,netOperatorStat.act);
}

void test_modem_GetCPIN(){
    cmError_t retValue = CM_FAIL;
    char getCode[32]={0};

    MockResponseExpected = CM_CMD_MOCK_CPIN_RESPONSE;
    retValue = EC25_GetCPIN(&cmData,getCode);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_STRING("READY",getCode);

    memset(getCode,0,sizeof(getCode));
    MockResponseExpected = CM_CMD_MOCK_CPIN_RESPONSE_BAD;
    retValue = EC25_GetCPIN(&cmData,getCode);
    TEST_ASSERT_EQUAL_INT32(CM_ERR_INVALID_RESPONSE,retValue);
    //TEST_ASSERT_EQUAL_STRING("READY",getCode);

}

void test_modem_SetApn(){
    //TEST_IGNORE_MESSAGE("This Test Was Ignored, This Function Has Not Been Tested, Needs To Be Implemented");
    cmError_t retValue = CM_FAIL;
    EC25Config_t config =EC25_INITIALIZER_CONFIG;

    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    config.apn ="VATIA.COMCEL.COM.CO";
    config.apnPassw = "CLARO";
    config.apnUser ="COMCEL";
    retValue = EC25_SetApn(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    
}

void test_modem_SetContextProfile(){
    //TEST_IGNORE_MESSAGE("This Test Was Ignored, This Function Has Not Been Tested, Needs To Be Implemented");
    cmError_t retValue = CM_FAIL;
    EC25Config_t config =EC25_INITIALIZER_CONFIG;

    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    config.contextId = 1;
    retValue = EC25_SetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_GetContextProfile(){
    cmError_t retValue = CM_FAIL;
    EC25Config_t config = EC25_INITIALIZER_CONFIG;

    MockResponseExpected = CM_CMD_MOCK_QIACT_GET_RESPONSE;
    retValue = EC25_GetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
 
    TEST_ASSERT_EQUAL_INT32(1, config.contextId);
    TEST_ASSERT_EQUAL_INT32(1, config.contextState);
    TEST_ASSERT_EQUAL_INT32(1, config.contextType);
    TEST_ASSERT_EQUAL_STRING("10.101.234.65",config.contextIpAddres);

/*respuesta mala,no se usan datos*/
    memset(&config, 0 ,sizeof(config));
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_GetContextProfile(&cmData,&config);
    TEST_ASSERT_EQUAL_INT32(CM_ERR_INVALID_RESPONSE,retValue);
}

void test_modem_SetDeactContextProfile(){
    cmError_t retValue = CM_FAIL;
  
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SetDeactContextProfile(&cmData, 1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    MockResponseExpected = CM_CMD_MOCK_ERROR_GENERAL_RESPONSE;
    retValue = EC25_SetDeactContextProfile(&cmData, 1);
    TEST_ASSERT_EQUAL_INT32(CM_ERR_INVALID_RESPONSE,retValue);
}

void test_modem_SMS_SetMessageFormat(){
    cmError_t retValue = CM_FAIL;
    
    /*DPU format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SMS_SetMessageFormat(&cmData, 0);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);

    /*text format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SMS_SetMessageFormat(&cmData,1);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_SMS_SetTextModeParam(){
    cmError_t retValue = CM_FAIL;
    
    /*text format*/
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SMS_SetTextModeParam(&cmData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_SMS_SetTeCharacterGsm(){
    cmError_t retValue = CM_FAIL;
    
    /*GSM default alphabet */
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SMS_SetTeCharacterGsm(&cmData);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}

void test_modem_SMS_SendTextMessage(){
    cmError_t retValue = CM_FAIL;
    
    /*GSM default alphabet */
    MockResponseExpected = CM_CMD_MOCK_GENERAL_RESPONSE;
    retValue = EC25_SMS_SendTextMessage(&cmData,"This is a short message ");
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
}


void test_modem_SetOpenSocket(){
    cmError_t retValue = CM_FAIL;
    EC25Config_t config =EC25_INITIALIZER_CONFIG;
    socketData_t configSocket =EC25_INITIALIZER_SOCKET_DATA;
    MockResponseExpected = CM_CMD_MOCK_QIOPEN_RESPONSE;
    configSocket.accessMode = 0;
    configSocket.contextId = config.contextId = 1;
    configSocket.connectId = config.connectId = 3;
    configSocket.serviceType = EC25_SOCKET_SRV_TYPE_UDP;
    configSocket.remip ="127.0.0.1";
    configSocket.remPort = "0";
    configSocket.locPort = "2020" ;
    retValue = EC25_SetOpenSocket(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    
}

void test_modem_GetSocketServStatus(){
    cmError_t retValue = CM_FAIL;
    EC25Config_t config =EC25_INITIALIZER_CONFIG;
    socketData_t configSocket =EC25_INITIALIZER_SOCKET_DATA;

    MockResponseExpected = CM_CMD_MOCK_QISTATE_RESPONSE;

    configSocket.connectId = config.connectId = 3;
    retValue = EC25_GetSocketServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(3, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("10.106.6.155",configSocket.ipAddres);

    configSocket.connectId = config.connectId = 11;
    MockResponseExpected = CM_CMD_MOCK_QISTATE_RESPONSE_INCOMMING_CLIENT;
    retValue = EC25_GetSocketServStatus(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(11, configSocket.connectId);
    TEST_ASSERT_EQUAL_STRING("192.168.32.68",configSocket.ipAddres);
}


void test_modem_GetSocketRecv(){
    char bufferRecv[1060];


    cmError_t retValue = CM_FAIL;
    EC25Config_t config =EC25_INITIALIZER_CONFIG;
    socketData_t configSocket =EC25_INITIALIZER_SOCKET_DATA;
    configSocket.msgRecv = bufferRecv;
    configSocket.buffRcvLen = CM_BUFFER_LEN_DEF;
   MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE;

    configSocket.connectId = config.connectId = 3;
    retValue = EC25_GetSocketRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(7, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("julian",configSocket.msgRecv);

  
   // memset(bufferRecv,0,sizeof(bufferRecv));
    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_LONG;

    configSocket.connectId = config.connectId = 11;
    
    retValue = EC25_GetSocketRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(112, configSocket.msgLen);
    TEST_ASSERT_EQUAL_STRING("7EA036610330A166E6E7006128A109060760857405080102A203020101A305A10302010DBE0F040D0800065F04001802201388FA0074EA7E",configSocket.msgRecv);

    MockResponseExpected = CM_CMD_MOCK_QIRD_RESPONSE_EMPTY_BUFF;

    configSocket.connectId = config.connectId = 3;
    retValue = EC25_GetSocketRecv(&cmData,&configSocket);
    TEST_ASSERT_EQUAL_INT32(CM_OK,retValue);
    TEST_ASSERT_EQUAL_INT32(0, configSocket.msgLen);
    //TEST_ASSERT_EQUAL_STRING(" ",configSocket.msgRecv);
   }


void test_modem_QIURC_incomming(){
    cmError_t retValue = CM_FAIL; 
    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_INCOMMING_RESPONSE;

    retValue = EC25_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(CM_ASYNC_EVENT,retValue);
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_INCOMMING_RESPONSE,cmGetBufferRecv(&cmData));


    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_FULL_RESPONSE;
    retValue = EC25_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(CM_ASYNC_EVENT,retValue);
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_FULL_RESPONSE,cmGetBufferRecv(&cmData));

    
    MockResponseExpected = CM_CMD_MOCK_QIURC_CONN_CLOSED_RESPONSE;

    retValue = EC25_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(CM_ASYNC_EVENT,retValue);
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_CONN_CLOSED_RESPONSE,cmGetBufferRecv(&cmData));

    
    MockResponseExpected = CM_CMD_MOCK_QIURC_RECV_RESPONSE;

    retValue = EC25_SendRaw(&cmData, "AT\r");   // solo para que funcione el test debe enviar algo
    TEST_ASSERT_EQUAL_INT32(CM_ASYNC_EVENT,retValue);
    TEST_ASSERT_EQUAL_INT32(1 ,cmIsAsyncEvent(&cmData));
    TEST_ASSERT_EQUAL_STRING(CM_CMD_MOCK_QIURC_RECV_RESPONSE,cmGetBufferRecv(&cmData));

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
            EC25_IsrRx(&cmData, MockResponseExpected[i]); 
			i++;
		}
        /*print command and response*/
        printf(" -->");
		printf(KGRN "%s",MockResponseExpected);  
        printf(KNRM); /*white color */
	}
}