
#ifndef _TESTMOCKCOMMANDS_H_
#define _TESTMOCKCOMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


#define CM_CMD_MOCK_GENERAL_RESPONSE                            "\r\nOK\r\n"
#define CM_CMD_MOCK_GENERAL_RESPONSE_0                          "0\r\n"
#define CM_CMD_MOCK_ERROR_GENERAL_RESPONSE                      "\r\nERROR\r\n"
#define CM_CMD_MOCK_ATE1_RESPONSE                               "\r\nATE1\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_ATE0_RESPONSE                               "\r\nATE0\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_ATV1_RESPONSE                               "\r\nOK\r\n"
#define CM_CMD_MOCK_ATV0_RESPONSE                               "0\r\n"
#define CM_CMD_MOCK_GSQ_RESPONSE                                "\r\n+CSQ: 28,99\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QNWINFO_RESPONSE                            "\r\n+QNWINFO: \"FDD LTE\",\"732101\",\"LTE BAND 7\",3075\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QNWINFO_RESPONSE2_ECHO                      "AT+QNWINFO\r\r\n+QNWINFO: \"EDGE\",\"732101\",\"GSM 1900\",583\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_GSN_RESPONSE                                "\r\n862708040783073\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_IMSI_RESPONSE                               "\r\n732101596840439\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_CCID_RESPONSE                               "\r\n+QCCID: 89571015025068994296\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_ATI_RESPONSE                                "\r\nQuectel\r\nEC25\r\nRevision: EC25AUXGAR08A03M1G\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_ATI_RESPONSE2_ECHO                          "ATI\r\r\nQuectel\r\nBG96\r\nRevision: BG96MAR02A07M1G\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_CREG_RESPONSE                               "\r\n+CREG: 0,1\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_COPS_RESPONSE                               "\r\n+COPS: 0,0,\"#CoberturaCLARO Claro\",7\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_CPIN_RESPONSE                               "\r\n+CPIN: READY\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_CPIN_RESPONSE_BAD                           "\r\n+CME ERROR: SIM failure\r\n"
#define CM_CMD_MOCK_QIACT_GET_RESPONSE                          "\r\n+QIACT: 1,1,1,\"10.101.234.65\"\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIOPEN_RESPONSE                             "\r\nOK\r\n\r\n+QIOPEN: 1,0\r\n"
#define CM_CMD_MOCK_QISTATE_RESPONSE                            "\r\n+QISTATE: 3,\"TCP LISTENER\",\"10.106.6.155\",0,5500,3,1,3,0,\"uart1\"\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QISTATE_SERIAL_RESPONSE                     "AT+QISTATE=1,3\r\n+QISTATE: 3,\"TCP\",\"192.168.32.48\",6000,0,1,1,3,0,\"uart1\"\r\n\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QISTATE_RESPONSE2_ECHO                      "AT+QISTATE=1,3\r\r\n+QISTATE: 3,\"TCP LISTENER\",\"10.101.234.65\",0,2020,4,1,3,0,\"uart1\"\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QISTATE_RESPONSE_INCOMING_CLIENT            "\r\n+QISTATE: 11,\"TCP INCOMING\",\"192.168.32.68\",54842,5500,2,1,3,0,\"uart1\"\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIRD_RESPONSE                               "\r\n+QIRD: 6\r\njulian\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIRD_RESPONSE_EMPTY_BUFF                    "\r\n+QIRD: 0\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIRD_RESPONSE_LONG                          "\r\n+QIRD: 112\r\n7EA036610330A166E6E7006128A109060760857405080102A203020101A305A10302010DBE0F040D0800065F04001802201388FA0074EA7E\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIRD_RESPONSE_STR                           "\r\n+QIRD: 246\r\n" "Buenas a todos! Estaba chusmeando el proyecto. Los navigation devices esten teniendo un conjunto de posiciones, que se espera hacer con eso? Ponemos todas las posiciones en el mapa, hacemos un calculo en el front, que representan esas posiciones?" "\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIRD_RESPONSE_ECHO                          "AT+QIRD=10,1024\r\r\n+QIRD: 6\r\njulian\r\n\r\nOK\r\n"
#define CM_CMD_MOCK_QIURC_CONN_INCOMING_RESPONSE                "\r\n+QIURC: \"incoming\",11,1,\"192.168.32.68\",63713\r\n" /*URC of Incoming Connection*/
#define CM_CMD_MOCK_QIURC_RECV_RESPONSE                         "\r\n+QIURC: \"recv\",10\r\n"      /*The URC indicates the incoming data in buffer access mode. The host can receive data via AT+QIRD.*/
#define CM_CMD_MOCK_QIURC_CONN_CLOSED_RESPONSE                  "\r\n+QIURC: \"closed\",10\r\n"    /*Socket service connection is closed.*/
#define CM_CMD_MOCK_QIURC_CONN_FULL_RESPONSE                    "\r\n+QIURC: \"incoming full\"\r\n"  /*URC Indicating Incoming Connection Full*/
#define CM_CMD_MOCK_QISEND_OK_RESPONSE                          "SEND OK\r\n"
#ifdef __cplusplus
}
#endif
#endif /*end _TESTMOCKCOMMANDS_H_*/




/*

AT
AT
ATE0
AT+CMEE=1
AT+CGMR
AT+CGSN
AT+CPIN?
AT+CPIN?
AT+CPIN?
AT+CIMI
AT+QCFG="nwscanmode",0
AT+CSQ
AT+COPS?
AT+COPS?
AT+COPS?
AT+COPS?
AT+QNWINFO
AT+QURCCFG="urcport","uart1"
AT+CNMI=2,1
AT+CPMS="SM","SM","SM"
AT+CMGF=0
AT+CMGL=4
AT+CMGD=1,4
AT+QIDEACT=1
AT+CGATT=1
AT+QICSGP=1,1,"VATIA.COMCEL.COM.CO","","",3
AT+QIACT=1
AT+QIACT?
AT
AT+QNWINFO
AT+CPIN?
AT+CSQ
AT+QICLOSE=0
AT+QIOPEN=1,0,"UDP","",0,0,0
AT+QIOPEN=1,0,"UDP","",0,0,0
AT+QIOPEN=1,0,"UDP","",0,0,0
AT+QICLOSE=3
AT+QIOPEN=1,3,"TCP LISTENER","127.0.0.1",0,5500,0
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT
AT+QNWINFO
AT+CPIN?
AT+CSQ
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QISTATE=1,3
AT+QIRD=3,1060
AT+QIACT?
AT+QISTATE=1,3
AT+QIRD=3,1060
AT
AT+QNWINFO
AT+CPIN?
AT+CSQ
AT+QISTATE=1,3
AT+QIRD=3,1060
*/