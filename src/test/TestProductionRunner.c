#include "TestProductionCode.h"
#include "unity.h"

// gcc Ec25.c atCmdDevice.c unity.c TestProductionCode.c TestProductionRunner.c -Wall -D ENABLED_WAIT_BLOCK -std=c99 -o  main
/*=======MAIN=====*/
int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_modem_initializer);
    RUN_TEST(test_modem_SetATE);
    RUN_TEST(test_modem_SetATV);
    RUN_TEST(test_modem_GetNetworkInformation);
    RUN_TEST(test_modem_GetModuleInformation);
    RUN_TEST(test_modem_GetImei);
    RUN_TEST(test_modem_GetImsi);
    RUN_TEST(test_modem_GetIccid);
    RUN_TEST(test_modem_GetSignalQuality);
    RUN_TEST(test_modem_GetCREG);
    RUN_TEST(test_modem_GetCOPS);
    RUN_TEST(test_modem_GetCPIN);

    RUN_TEST(test_modem_SetApn);
    RUN_TEST(test_modem_SetContextProfile);
    RUN_TEST(test_modem_GetContextProfile);
    RUN_TEST(test_modem_SetDeactContextProfile);
    RUN_TEST(test_modem_SMS_SetMessageFormat);
    RUN_TEST(test_modem_SMS_SetTextModeParam);
    RUN_TEST(test_modem_SMS_SetTeCharacterGsm);
    RUN_TEST(test_modem_SMS_SendTextMessage);
    RUN_TEST(test_modem_SetOpenSocket);
    RUN_TEST(test_modem_GetSocketServStatus);
    RUN_TEST(test_modem_GetSocketRecv);
    RUN_TEST(test_modem_QIURC_incomming);
    return UNITY_END();
}








/*

    uint8_t rssi;
    cmError_t retValue;

    retValue = EC25_InitModem(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,GetTickCountMs,OutputFunction);
    printf("returned error : [%d] \n",retValue);

    retValue=CM_ERR_WAITING;
    while (retValue == CM_ERR_WAITING || retValue == CM_ERR_INVALID_RESPONSE ){ //for non blocking
        //retValue = EC25_GetSignalQuality(&cmData,&rssi);
        
        EC25Config_t EC25Config = EC25_CONFIG_INITIALIZER; //{.apn="VATIA.COMCEL.COM.CO", .apnUser="COMCEL" , .apnPassw="COMCEL"};
       
       
           EC25Config.apn = apn;
       
       
       // retValue = EC25_SetApn(&cmData, &EC25Config);

        //EC25Config.contextId = 1;
        //retValue = EC25_SetContextProfile(&cmData, &EC25Config);
        EC25Data_t EC25Data =EC25_DATA_INITIALIZER;

       // retValue = EC25_GetNetworkInformation(&cmData,&EC25Data);
        if(retValue == CM_OK){
            printf("EC25Data [%s]\n", EC25Data.net.accessTechnSel);
            printf("EC25Data [%s]\n", EC25Data.net.operator);
            printf("EC25Data [%s]\n", EC25Data.net.band);
            printf("EC25Data [%s]\n", EC25Data.net.channelId);
            }

       // retValue = EC25_GetImei(&cmData,&EC25Data);
        //retValue = EC25_GetImsi(&cmData,&EC25Data);
        //retValue = EC25_GetIccid(&cmData,&EC25Data);
        //if(retValue == CM_OK){
        //    printf("EC25Data [%s]\n", EC25Data.modemInfo.imei);
        //    printf("EC25Data [%s]\n", EC25Data.modemInfo.imsi);
        //    printf("EC25Data [%s]\n", EC25Data.modemInfo.iccid);
        // }
        retValue = EC25_GetModuleInformation(&cmData,&EC25Data);
        if(retValue == CM_OK){
            printf("EC25Data [%s]\n", EC25Data.modemInfo.deviceModel);
            printf("EC25Data [%s]\n", EC25Data.modemInfo.manufactorId);
            printf("EC25Data [%s]\n", EC25Data.modemInfo.firmwareVersion);
         }
        if( retValue != CM_ERR_WAITING)
            printf("\nreturned cmGetSignalQuality : [%d] %d\n",retValue,cmData.DataPrivate.numberRetries);
    }*/