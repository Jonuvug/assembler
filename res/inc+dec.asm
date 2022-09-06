one	= $0020

init:
	LDI, %01
	STA, one

inc:
	PRT
	ADD, one
	JC, dec
	JMP, inc

dec:
	SUB, one
	PRT
	JZ, inc
	JMP, dec