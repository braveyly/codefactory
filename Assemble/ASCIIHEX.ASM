DATA SEGMENT
  HEX DB ?
  ASCII DB 'B'
DATA ENDS

CODE SEGMENT
  ASSUME CS:CODE,DS:DATA

START:MOV AX,DATA
      MOV DS,AX
      MOV AL,ASCII
      CMP AL,'0'

LAB: JB LAB5    ;ASCII<'0'
     MOV AH,AL
     SUB AH,'0'
     CMP AL,'9'
     JBE LAB6   ;'0'=<ASCII<='9'

     CMP AL,'A'
     JB LAB5    ;'9'<ASCII<'A'

     MOV AH,AL
     SUB AH,'A'-10
     CMP AL,'F'
     JBE LAB6   ;'A'=<ASCII<='F'

     CMP AL,'a'
     JB LAB5    ;'F'<ASCII<'a'

     MOV AH,AL
     SUB AH,'a'-10
     CMP AL,'f'
     JBE LAB6   ;'a'=<ASCII<='f'

     ;default goto LAB5

LAB5:MOV AH,-1
LAB6:MOV HEX,AH
     MOV AH,4CH
     INT 21H
CODE ENDS
     END START 