
#include "unity.h"
#include "unity_internals.h"
#include "cellModem.h"

#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


//===============Wrappers ==========================================
extern char inputBuffer[1024];   /*!< buffer rx*/
extern cmData_t cmData ;         /*!< global for isr*/
uint32_t GetTickCountMs(void);   /*!< systick generator*/
void OutputFunction(void *sp, char c);  /*!< tx uart, this call isr after send command*/
//===============Wrappers ==========================================

 void setUp(void);
 void tearDown(void);


/*Testing*/

 void test_modem_initializer();

 void test_modem_GetNetworkInformation();

 void test_modem_GetModuleInformation();

 void test_modem_GetImei();

void test_modem_GetImsi();

void test_modem_GetIccid();

void test_modem_GetSignalQuality();
void test_modem_SetATE();
void test_modem_SetATV();
void test_modem_GetCREG();
void test_modem_GetCOPS();
void test_modem_GetCPIN();

void test_modem_SetApn();
void test_modem_SetContextProfile();
void test_modem_GetContextProfile();
void test_modem_SetDeactContextProfile();
void test_modem_SmsSetMessageFormat();
void test_modem_SmsSetTextModeParam();
void test_modem_SmsSetTeCharacterGsm();
void test_modem_SmsSendNumber();
void test_modem_SmsSendTextMessage();
void test_modem_SocketOpen();
void test_modem_SocketGetServStatus();
void test_modem_SocketGetRecv();
void test_modem_QIURC_incoming();
void test_modem_QIURC_incomingConnection();
void test_modem_QIURC_closedConnection();
void test_modem_QIURC_RecvConnection();
void test_modem_SocketClose();
void test_modem_SocketSendData();