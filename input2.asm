MOVC R0,#0
MOVC R1,#1
MOVC R2,#2
MOVC R4,#4
MOVC R5,#1
MOVC R6,#1000
MOVC R7,#4060
MUL R2,R2,R4
ADDL R5,R5,#1
ADD R2,R1,R1
JALR R5,R7,#4
SUB R5,R2,R2
BZ #24
ADD R2,R2,R1
SUB R5,R5,R1
AND R4,R1,R5
ADD R0,R1,R2
RET R5
STORE R6,R4,#2
LOAD R0,R4,#2
HALT 