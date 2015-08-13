#include <stdio.h>
#include <string.h>
#include "FdRE.h"

int uint8RESetExpression(uint8_t* reStr, int reLen)
{
    int i = reLen;
    int lenStart = 0, lenType = 0, firstLenFlag = 0;

    //sanity check
    while((reStr[reLen-i] - MUL_BYTES) && i)
    {
        i--;
    }
    if(!i)
    {
        printf("RE Format value is wrong\n");
        return -1;
    }

    i = reLen;
    while((reStr[reLen-i] - LEN_BYTE) && i)
    {
        i--;
    }
    if(!i)
    {
        printf("RE Format LEN_BYTE is wrong\n");
        return -1;
    }

    gPatternStr = (uint8_t*)malloc(sizeof(uint8_t)*reLen);
    memset(gPatternStr, 0, reLen);
    memcpy(gPatternStr, reStr, reLen);
    gPatternLen = reLen;

    //parse value type
    i = reLen;
    while(i)
    {
        if(!(reStr[reLen-i] - LEN_BYTE) && !firstLenFlag)
        {
            lenStart = reLen - i;
            firstLenFlag = 1;
        }

        if(!(reStr[reLen-i] - LEN_BYTE) && firstLenFlag)
            lenType++;
        i--;
    }

	if(!firstLenFlag)
	{
	   printf( "No LEN_BYTE Flag\n" );
	   return -1;
	}

	if( !( sizeof(uint8_t) == lenType || 
		   sizeof(uint16_t)== lenType || 
		   sizeof(uint32_t) == lenType ) )
	{
	   printf( "Len type is wrong\n" );
	   return -1;
	}
	
    gValLenByteType = lenType;
    gValLenByteStart = lenStart;
	return 0;
}

int uint8RESetValueLenOffSet(int offset)
{
    gValLenOffSet = offset;
}

#define MATCH 0
#define NOT_ENOUGH_LENGTH_FOR_LEN_BYTE -1
#define NOT_ENOUGH_LENGTH_FOR_VALUE -2
#define NOT_RES_ENOUGH_BUFFER -3
#define NO_MATCH -4
#define WRONG_VALUE_LEN_OFFSET -5
#define PARAMETER_ERROR -6
int uint8REParse(uint8_t* srcStr, int srcLen, uint8_t* resStr, int resStrLen, int* actualLen, int* offSet)
{
    int srcIndex =0 , ptnIndex = 0, startPos = 0;
    uint8_t u8ValLen;
    uint16_t u16ValLen;
    uint32_t u32ValLen;
	int ret = MATCH;

	if( srcLen < gPatternLen - 1 )
	{
	   ret = PARAMETER_ERROR;
	   goto end;
	}
	
    while(srcIndex < srcLen)
    {
        //for normal stable bytes
        if( LEN_BYTE != gPatternStr[ptnIndex]&&
            SINGLE_BYTE != gPatternStr[ptnIndex] &&
            MUL_BYTES != gPatternStr[ptnIndex] )
        {
            if(srcStr[srcIndex] != gPatternStr[ptnIndex])
            {
                startPos++;
                srcIndex = startPos;
                ptnIndex = 0;
                continue;
            }
        }

		//for length bytes
        if( LEN_BYTE == gPatternStr[ptnIndex] )
        {
            if( sizeof(uint8_t) == gValLenByteType )
            {
                u8ValLen = srcStr[srcIndex];
                srcIndex += sizeof(uint8_t);
                ptnIndex += sizeof(uint8_t);                
                gValueLen = u8ValLen;
				printf( "gValueLen uint8_t is %d\n",gValueLen );
            }
            else if( sizeof(uint16_t) == gValLenByteType )
            {
                memcpy(&u16ValLen, srcStr+srcIndex, sizeof(uint16_t));
                srcIndex+=sizeof(uint16_t);
                ptnIndex+=sizeof(uint16_t);
                gValueLen = u16ValLen;
				printf( "gValueLen uint16_t is %d\n",gValueLen );
            }
            else if( sizeof(uint32_t) == gValLenByteType )
            {
                memcpy(&u32ValLen, srcStr+srcIndex, sizeof(uint32_t));
                srcIndex+=sizeof(uint32_t);
                ptnIndex+=sizeof(uint32_t);
                gValueLen = u32ValLen;
				printf( "gValueLen uint32_t is %d\n",gValueLen );
            }
			
			if(srcIndex >= srcLen)
            {
               ret = NOT_ENOUGH_LENGTH_FOR_LEN_BYTE;
			   *offSet = startPos;
			   goto end;
			}

			if( gValueLen <= gValLenOffSet )
			{
				ret = WRONG_VALUE_LEN_OFFSET;
				*offSet = 0;
				goto end;
			}

            continue;
        }

		//for value part
        if( MUL_BYTES == gPatternStr[ptnIndex] )
        {
            printf( "valueLen is %d offset is %d\n", gValueLen, gValLenOffSet);
            if( srcIndex + gValueLen - gValLenOffSet > srcLen )
            {
                ret = NOT_ENOUGH_LENGTH_FOR_VALUE;
				*offSet = 0;				
				goto end;
            }
			
			if( ptnIndex + gValueLen - gValLenOffSet > resStrLen )
			{
				ret = NOT_RES_ENOUGH_BUFFER;
				*offSet = 0;
				goto end;
			}
			
            memcpy(resStr , srcStr+startPos, ptnIndex + gValueLen - gValLenOffSet );
            *actualLen =  ptnIndex + gValueLen - gValLenOffSet;
            break;
        }
		
        srcIndex++;
        ptnIndex++;
    }

    if(srcIndex >= srcLen)
    {
       ret = NO_MATCH;
	   goto end;
    }

end:
	*offSet = startPos;
    return ret;
}

