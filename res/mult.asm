one		=	$0050
x_val	=	$0051
y_val	=	$0052
product	=	$0053

init:
	LDI %01
	STA one
	LDI %16
	STA $0051
	LDI %09
	STA y_val
	LDI %00
	STA product

top:
	LDA x_val
	SUB one
	JC continue

	LDA product
	PRT
	HLT

continue:
	STA x_val
	LDA product
	ADD y_val
	STA product
	JMP top