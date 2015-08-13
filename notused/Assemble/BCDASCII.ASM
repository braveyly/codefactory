DSEG SEGMENT
   BCD DB 86H
   ASCII DB 2 DUP(0)
DSEG ENDS

CSEG SEGMENT
   ASSUME CS:CSEG, DS:DSEG
START:
   MOV AX,DSEG  ;Set data segment
   MOV DS,AX
              
   MOV AL,BCD   ;Convert low 4 bits to ascii and save it in ASCII[1] 
   MOV DL,0FH
   AND AL,DL
   ADD AL,30H
   MOV ASCII+1,AL
    
   MOV AL,BCD
   MOV CL,4
   SHR AL,CL
   ADD AL,30H
   MOV ASCII,AL
CSEG ENDS
    END START
   