#include "Ec25.h"
#include "stdio.h"

char inputBuffer[128];

#include <sys/time.h>
#include <time.h>
#include <unistd.h>


//------------build-----------
//   gcc Ec25_main.c Ec25.c atCmdDevice.c -Wall -D ENABLED_WAIT_BLOCK -o  main
//-----------------------

uint32_t GetTickCountMs(void);
void OutputFunction(void *sp, char c);
cmData_t cmData = CM_DATA_INITIALIZER;



//------testing --- change to unity test
int main(){

    uint8_t rssi;
    cmError_t retValue;
    
    retValue = EC25_InitModem(&cmData,inputBuffer,CM_BUFFER_LEN_DEF,GetTickCountMs,OutputFunction);
    printf("returned error : [%d] \n",retValue);

    retValue=CM_ERR_WAITING;
    while (retValue == CM_ERR_WAITING){ //for non blocking
        retValue = EC25_GetSignalQuality(&cmData,&rssi);
        printf("\nreturned cmGetSignalQuality : [%d] \n",retValue);
    }
    return 0;
}







//---systick generator
uint32_t GetTickCountMs(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_nsec / 1000000) + ((uint32_t)ts.tv_sec * 1000ull);
}

//-----------uart tx mock ---
void OutputFunction(void *sp, char c){
	int i = 0; 
    char ResponseExpected[100] = {CM_CMD_MOCK_GENERAL_RESPONSE};
	if(c != '\r')
    	printf("%c",c); /*no print \r in console */
	
	if(c == '\r') {	 /*if send command to module*/
		while(ResponseExpected[i] != '\0') { /*then the module response is pass to isr char by char */
		 	EC25_IsrRx(&cmData, ResponseExpected[i]); // comentar para nonblocking test wait
			i++;
		}
		printf(" --> %s\n",ResponseExpected);  /*print command and response*/

	}
}