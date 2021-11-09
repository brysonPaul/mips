#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	if(ALUControl == 0)
	{
			*ALUresult = (A + B);
	}
	else if(ALUControl == 1)
	{
			*ALUresult = (A - B);
	}
	else if(ALUControl == 2)//signed version
	{
		unsigned aHold = A;
		unsigned bHold = B;

		if((aHold & 0b10000000000000000000000000000000)>>31)
		{
			aHold = ~aHold + 1;
		}
		if((bHold & 0b10000000000000000000000000000000)>>31)
		{
			bHold = ~bHold + 1;
		}
			int hold = aHold-bHold;
			if(hold<0){
				*ALUresult = 1;
			}
			else ALUresult = 0;
	}
	else if(ALUControl == 3)//unsigned
	{
			int hold = A-B;
			if(hold<0){
				*ALUresult = 1;
			}
			else ALUresult = 0;
	}
	else if(ALUControl == 4)
	{
			*ALUresult = A & B;
	}
	else if(ALUControl == 5)
	{
			*ALUresult = A | B;
	}
	else if(ALUControl == 6)
	{
			*ALUresult = B<<16;
	}
	else if(ALUControl == 7)
	{
			*ALUresult = ~A;
	}		

	if(ALUresult == 0)
		{
				*Zero = 1;
		}
		else
		{
			*Zero = 0;
		}

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if(PC % 4 != 0){
		return 1;//makes halt 1 lolll
	} 
	else{
		*instruction = Mem[MEM(PC)];
		return 0;
	}

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	
	//my dyslexia kicked in trying to do this by hand :(
	//but i got through it!!!
	*op = (instruction & 0b11111100000000000000000000000000)>>26;
	*r1 = (instruction & 0b00000011111000000000000000000000)>>21;
	*r2 = (instruction & 0b00000000000111110000000000000000)>>16;
	*r3 = (instruction & 0b00000000000000001111100000000000)>>11;
	*funct = (instruction & 0b00000000000000000000000000111111);
	*offset = (instruction & 0b00000000000000001111111111111111);
	*jsec = (instruction & 0b00000011111111111111111111111111);
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
		unsigned firstThree = op & 000111;
		unsigned lastThree = (op & 111000) >>3;

		if(op == 0){
			//means it an r type, this is big sad
		}
		if(firstThree == 0 && lastThree == 2){
			//jump
		}
		if(firstThree == 0 && lastThree == 4){
			//beq
		}
		if(firstThree == 0 && lastThree == 5){
			//bne
		}
		if(firstThree == 1 && lastThree == 0){
			//addi
		}
		if(firstThree == 1 && lastThree == 2){
			//slti
		}
		if(firstThree == 1 && lastThree == 4){
			//andi
		}
		if(firstThree == 1 && lastThree == 5){
			//ori
		}
		if(firstThree == 4 && lastThree == 3){
			//lw
		}
		if(firstThree == 5 && lastThree == 3){
			//sw
		}

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	data1 = &Reg[r1];
	data2 = &Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	if((*extended_value & 0b00000000000000001000000000000000)>>15)//so if the sign bit is one this will run, otherwise no go
	{
		*extended_value = *extended_value | 0b11111111111111110000000000000000;
	}
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{


	 ALU(data1,data2,ALUOp,ALUresult,Zero);
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	if(*Mem%4!=0){
		return 0b1;
	}

	if(MemWrite==0b1){
		Mem[0]=data2;//might be wrong, ask prof
	}
	if(MemRead==0b1){
		*memdata = MEM(*Mem);
		
	}
	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if(RegWrite == 0){
			return;
	}
	else{
		if(MemtoReg == 1){
			Reg[RegDst] = memdata;
		}
		else{
			Reg[RegDst] = ALUresult;
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	PC +=4;

	if(Branch){
		if(!Zero){
			PC += 4 * (jsec & 0b00000000001111111111111111);
		}
	}
	if(Jump){//might be wrong idk :C
			PC += (*PC & 0b11110000000000000000000000000000)+(jsec<<2);
	}
}

