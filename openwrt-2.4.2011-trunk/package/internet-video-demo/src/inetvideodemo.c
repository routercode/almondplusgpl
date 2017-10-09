#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FALSE               0 
#define TRUE                1 
#define BOOLEAN             unsigned char
 
#define SVERSION            "1.0"
char VersionInfo[] = "Cortina-Systems Inc. GoldenGate Internet Video Demo "SVERSION" ["__DATE__" "__TIME__"]";


static const char UsageMsg[] =
       "Usage: %s (<URL>)"\
        " \n";


#define DEMO_REALLINK_CONF                  "/etc/inetvideodemo.conf"
#define IVDO_PROGRAM_SAMPLE_NO              3
#define IVDO_REALLINK_LENGTH                256
#define IVDO_REALLINK_FILENAME_BUF_SIZE     32
#define FFMPEG_COMMAND_BUF_LENGTH           512
#define DEMO_TS_FILE_NAME                   "demo"


BOOLEAN isFileExist(char *pFile)
{
    BOOLEAN bRet = FALSE;
//    printf("isFileExist: Enter\n");
    FILE* fp = fopen(pFile, "r");
    if (fp) {
        bRet = TRUE;
        fclose(fp);
    }
//    printf("isFileExist: Left %d\n", bRet);
    return bRet;
}// end isFileExist()


int main(int argc, char *argv[]) 
{
    char realLink[IVDO_PROGRAM_SAMPLE_NO][IVDO_REALLINK_LENGTH];
    char tsFile[IVDO_PROGRAM_SAMPLE_NO][IVDO_REALLINK_FILENAME_BUF_SIZE];
    char ffmpegCmd[FFMPEG_COMMAND_BUF_LENGTH];
    int inputStreamNo = 0, i, systemStatus;
    FILE *fPtr;
    
    
    printf("\n%s\n", VersionInfo);
	if (argc > 2) {
		fprintf(stderr, UsageMsg, argv[0]);
		exit(1);
	}/* if() */
    
    bzero(realLink, sizeof(realLink));
    if (argc == 2) {
        strcpy(realLink[0], argv[1]);
        inputStreamNo = 1;
    } else if (argc == 1) {
        if ((fPtr = fopen(DEMO_REALLINK_CONF, "r")) == NULL) {
            printf("ERROR: %s open fail\n", DEMO_REALLINK_CONF);
            exit(1);
        }/* if() */

        while (fgets(realLink[inputStreamNo], IVDO_REALLINK_LENGTH, fPtr) != NULL) {
            realLink[inputStreamNo][strlen(realLink[inputStreamNo])-2] = '\0';
            inputStreamNo++;

            if (inputStreamNo > IVDO_PROGRAM_SAMPLE_NO) {
                printf("ERROR: Support %d internet video contents only\n", IVDO_PROGRAM_SAMPLE_NO);
                fclose(fPtr);
                exit(1);
            }
        }/* while() */
        printf("\n");
        
        fclose(fPtr);
    }/* if() */
    
    // start ffmpeg
    bzero(tsFile, sizeof(tsFile));
    for (i=0; i<inputStreamNo; i++) {
        bzero(ffmpegCmd, sizeof(ffmpegCmd));
        sprintf(tsFile[i], "%s%d.ts", DEMO_TS_FILE_NAME, i+1);
        sprintf(ffmpegCmd, "ffmpeg -i %s -vcodec copy -acodec copy -vbsf h264_mp4toannexb %s > /dev/null 2>&1 &", realLink[i], tsFile[i]);
//        printf("ffmpeg command: %s\n", ffmpegCmd);
        printf("%s: %s\n", tsFile[i], realLink[i]);
        systemStatus = system(ffmpegCmd);
    }/* for() */
    
    // sleep 1 second to wait ffmpeg process start
    printf("Wait for live555 start.");
    for (i=0; i<3; i++) {
        sleep(1);
        printf(".");
    }
    printf("\n");
//    printf(" inputStreamNo= %d\n", inputStreamNo);
    // check ts file
    for (i=0; i<inputStreamNo; i++) {
        if (!isFileExist(tsFile[i])) {
//            printf("ERORR: File %s not found\n", tsFile[i]);
            printf("ERORR: Please check link: %s\n", realLink[i]);
//            exit(1);
        }/* if() */
    }/* for() */
 
    // start live555
//    systemStatus = system("live555MediaServer > /dev/null 2>&1");
    systemStatus = system("live555MediaServer");
    
	return 0;
}// end main()

