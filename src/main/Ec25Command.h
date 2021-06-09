
#ifndef _CMEC25COMMANDS_H_
#define _CMEC25COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif


#define CM_CMD_SIGNAL_QUALITY_REPORT            "AT+CSQ\r"
#define CM_CMD_QUERY_NETWORK_INFORMATION        "AT+QNWINFO\r"



#define EC25_CMD_SIGNAL_QUALITY_REPORT                   "\r\nOK\r\n"

#define CM_CMD_MOCK_GENERAL_RESPONSE             "\r\n+CSQ: 28,99\r\n\r\nOK\r\n"

#ifdef __cplusplus
}
#endif
#endif /*end _CMEC25COMMANDS_H_*/