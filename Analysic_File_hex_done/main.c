#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#define FILE_NAME "D:/Khoa_RTOS/Build_my_project/Analysic_File_hex_done/Running.hex" // truy .hex -> link / ten / -> 1. hala1.hex / 2. hala2.hex -> 1 / 2

typedef enum {
    START_CODE  =   0U,
    BYTE_COUNT  =   1U,
    ADDRESS     =   2U,
    RECORD_TYPE =   3U,
    DATA        =   4U,
    CHECKSUM    =   5U,
    CAlCULATE   =   6U
}FieldHexFile;

typedef enum {
    READ_SUCCESSFULLY = 0U,
    READ_FAILED
}StatusReadHexFile;

typedef struct {
    // struct
    // {
        uint8_t u8ByteCount;
        uint8_t u8Addr[2];
        uint8_t u8RecordType;
        uint8_t u8Data[16];
        uint8_t u8CheckSum;
    // };

} HexFormData;

int ReadLine(FILE *fp, char *buffer);
void ConvertStringToHex(char *buff, int u32Length);
StatusReadHexFile ReadAllLine(FILE *fp, char *pBufferData);
char cBufferData[50];
char iInternalBufferData[50];

HexFormData hexStruct;
FILE *fp;
int i = 0;

int main () {
    fp = fopen(FILE_NAME, "r+");
    
    if (fp == NULL) {
        printf("FAIL TO OPEN FILE\n");
    }else {
        if (READ_SUCCESSFULLY == ReadAllLine(fp, cBufferData)) {
            printf("DONE");
        }else {
            printf("FALSE");
        }
    }
    fclose(fp);
}

StatusReadHexFile ReadAllLine(FILE *fp, char *pBufferData) {
    uint8_t u8CalculCheckSum;
    FieldHexFile State;
    int u32Count = 0U;
    int u32DataCount = 0U;
    int cntLine = 0U;

    while ((ReadLine(fp, pBufferData)) != 0U) {
        for (u32DataCount = 0U; u32DataCount < 16U; u32DataCount++) {
            hexStruct.u8Data[u32DataCount] = 0U;
        }
        cntLine++;
        State = START_CODE;
        for (u32Count = 0U; u32Count < strlen(cBufferData); u32Count++) {
            switch (State) {
                case START_CODE:
                {
                    if (cBufferData[u32Count] != ':') {
                        return READ_FAILED;
                    } else {
                        State = BYTE_COUNT;
                    }
                    break;
                }
                case BYTE_COUNT:
                {
                    ConvertStringToHex(&cBufferData[u32Count], 2U);
                    hexStruct.u8ByteCount = iInternalBufferData[0];
                    u32Count = 2U;
                    
                    State = ADDRESS;
                    break;
                }
                case ADDRESS:
                {
                    ConvertStringToHex(&cBufferData[u32Count], 4U);
                    hexStruct.u8Addr[0] = iInternalBufferData[0];
                    hexStruct.u8Addr[1] = iInternalBufferData[1];
                    u32Count = 6U;
                    State = RECORD_TYPE;
                    break;
                }
                case RECORD_TYPE:
                {
                    ConvertStringToHex(&cBufferData[u32Count], 2U);
                    hexStruct.u8RecordType = iInternalBufferData[0];
                    u32Count = 8U;
                    State = DATA;
                    break;
                }
                case DATA:
                {
                    ConvertStringToHex(&cBufferData[u32Count], (hexStruct.u8ByteCount * 2U));
                    for (u32DataCount = 0; u32DataCount < hexStruct.u8ByteCount; u32DataCount++)
                    {
                        hexStruct.u8Data[u32DataCount] = iInternalBufferData[u32DataCount];
                        if (hexStruct.u8RecordType == 0x00U)
                        {
                            printf("%.2x", hexStruct.u8Data[u32DataCount]);
                        }
                        
                    }
                    if (hexStruct.u8RecordType == 0x00U) {i = i + (uint32_t)hexStruct.u8ByteCount;}
                    
                    printf("\n");
                    // printf("%d", i);
                    // printf("%d", hexStruct.u8ByteCount);
                    printf("\n");
                    u32Count = 8U + (hexStruct.u8ByteCount * 2U);
                    State = CHECKSUM;
                    break;
                }
                case CHECKSUM:
                {
                    ConvertStringToHex(&cBufferData[u32Count], 2U);
                    hexStruct.u8CheckSum = iInternalBufferData[0];
                    State = CAlCULATE;
                    break;
                }
                case CAlCULATE:
                {
                    u8CalculCheckSum = hexStruct.u8ByteCount + hexStruct.u8Addr[0] + hexStruct.u8Addr[1] + hexStruct.u8RecordType;
                    for (u32DataCount = 0U; u32DataCount < hexStruct.u8ByteCount; u32DataCount++) {
                        u8CalculCheckSum = u8CalculCheckSum + hexStruct.u8Data[u32DataCount];

                    }
                    u8CalculCheckSum = ~u8CalculCheckSum + 1U;
                    cntLine = u8CalculCheckSum;
                    if (u8CalculCheckSum == hexStruct.u8CheckSum) {
                        if (hexStruct.u8RecordType == 0x01) {
                            return READ_SUCCESSFULLY;
                        }
                    } else {
                        return READ_FAILED;
                    }
                    break;
                }
                default:
                    return READ_FAILED; 
                    break;
            }

        }

    }

    return READ_SUCCESSFULLY;

}

int ReadLine(FILE *fp, char *pBufferData) {
    int u32Count = 0U;
    char cDataLine;

    while ((cDataLine = fgetc(fp)) != '\n')
    {
        pBufferData[u32Count] = cDataLine;
        u32Count++;
    }
    
    if (cDataLine == EOF)
    {
        return 0;
    }
    
    return 1;

}

void ConvertStringToHex(char *buff, int u32Length) {
    int u32Count = 4U;
    int u32Count1 = 0U;
    int tempBuffer[100];

    for (u32Count = 0U; u32Count < u32Length; u32Count++) {
        if (buff[u32Count] >= '0' && buff[u32Count] <= '9') {
            tempBuffer[u32Count] = buff[u32Count] - 48;
        } else if (buff[u32Count] >= 'A' && buff[u32Count] <= 'F') {
            tempBuffer[u32Count] = buff[u32Count] - 65 + 10;
        } else {
            return;
        }

        if ((u32Count %2) != 0) {
        iInternalBufferData[u32Count1] = ((tempBuffer[u32Count - 1] << 4) | (tempBuffer[u32Count]));
        u32Count1++;
        }
    } 
      
    
}