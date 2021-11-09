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
//problem here :(
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if(PC % 4 != 0){
		return 1;//makes halt 1 lolll
	} 
	else{
		instruction = &Mem[MEM(PC)];
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
			//r type
			controls->RegDst=1;
			controls->Jump=0;
			controls->Branch=0;
			controls->MemRead=0;
			controls->MemtoReg=0;
			controls->ALUOp=0b111;
			controls->MemWrite =0;
			controls->ALUSrc=0;
			controls->RegWrite=1;
			return 0;
		}
		if(firstThree == 0 && lastThree == 2){
			//jump
			controls->RegDst=2;
			controls->Jump= 1;
			controls->Branch=2;
			controls->MemRead=2;
			controls->MemtoReg=2;
			controls->ALUOp=0;
			controls->MemWrite =2;
			controls->ALUSrc=2;
			controls->RegWrite=2;
			return 0;

		}
		if(firstThree == 0 && lastThree == 4){
			//beq
			controls->RegDst=2;
			controls->Jump= 0;
			controls->Branch=1;
			controls->MemRead=0;
			controls->MemtoReg=2;
			controls->ALUOp=0;//means dont care
			controls->MemWrite =0;
			controls->ALUSrc=0;
			controls->RegWrite=0;
			return 0;
		}
		if(firstThree == 1){
			//i type
			controls->RegDst=0;
			controls->Jump= 0;
			controls->Branch=0;
			controls->MemRead=0;
			controls->MemtoReg=0;
			controls->MemWrite =0;
			controls->ALUSrc=1;
			controls->RegWrite=1;

			if(lastThree == 0){
				//addi
				controls->ALUOp=0;
				return 0;
			}
			else if(lastThree == 2)
			{
				//slti signed
				controls->ALUOp=0b010;
				controls->RegWrite=0;
				return 0;
			}
			else if(lastThree == 3)
			{
				//slti unsigned
				controls->ALUOp=0b011;
				controls->RegWrite=0;
				return 0;
			}
			else if(lastThree == 7)
			{
				//load upper immediate TODO
				controls->ALUOp=0b110;
				return 0;
			}
			
		}
		
		if(firstThree == 4 && lastThree == 3){
			//lw
			controls->RegDst=0;
			controls->Jump= 0;
			controls->Branch=0;
			controls->MemRead=1;
			controls->MemtoReg=1;
			controls->ALUOp=0;//lw adds registers
			controls->MemWrite =0;
			controls->ALUSrc=1;
			controls->RegWrite=1;
			return 0;
		}
		if(firstThree == 5 && lastThree == 3){
			//sw
			controls->RegDst=1;
			controls->Jump= 0;
			controls->Branch=0;
			controls->MemRead=0;
			controls->MemtoReg=0;
			controls->ALUOp=0;
			controls->MemWrite =1;
			controls->ALUSrc=1;
			controls->RegWrite=0;
			return 0;
		}
		return 1;
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
	if(ALUOp==0b111){
		//do things with the func (its an R type)
	}

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

