#include "TestProductionCode.h"
#include "unity.h"

// gcc cellModem.c atCmdDevice.c unity.c TestProductionCode.c TestProductionRunner.c -Wall -D ENABLED_WAIT_BLOCK -std=c99 -o  main
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
    RUN_TEST(test_modem_SmsSetMessageFormat);
    RUN_TEST(test_modem_SmsSetTextModeParam);
    RUN_TEST(test_modem_SmsSetTeCharacterGsm);
    RUN_TEST(test_modem_SmsSendNumber);
    RUN_TEST(test_modem_SmsSendTextMessage);
    RUN_TEST(test_modem_SocketOpen);
    RUN_TEST(test_modem_SocketGetServStatus);
    RUN_TEST(test_modem_SocketGetRecv);
    RUN_TEST(test_modem_QIURC_incoming);
    RUN_TEST(test_modem_QIURC_closedConnection);
    RUN_TEST(test_modem_QIURC_incomingConnection);
    RUN_TEST(test_modem_QIURC_RecvConnection);
    RUN_TEST(test_modem_SocketClose);
    RUN_TEST(test_modem_SocketSendData);
    printf(KRED "VERSION: [%s]\n" KNRM, CELLMODEM_CAPTION);

    return UNITY_END();
}
