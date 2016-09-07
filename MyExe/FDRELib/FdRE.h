#ifndef _FD_RE_H_
#define _FD_RE_H_

#define SINGLE_BYTE 0xff
#define LEN_BYTE    0xfe
#define MUL_BYTES   0xfd

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int

int uint8RESetExpression( uint8_t* reStr, int reLen); 
int uint8RESetValueLenOffSet( int offset );
int uint8RESetHeadStableValue( int start, int end, uint8_t** value, int flen, int slen);
int uint8RESetDepth( int depth );
int uint8REParse( uint8_t* srcStr, int srcLen, uint8_t** resStr, int** desLen );

uint8_t *gPatternStr;
int gPatternLen;
 
int gValueLen;
int gValLenByteType;// uint8_t uint16_t uint32_t
int gValLenByteStart;//the index of LEN_BYTE byte start
int gValLenOffSet;// = value length - actual data length

#endif
