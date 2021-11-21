#include "spimcore.h"
/*
Bryson Paul  || 11.13.2021 || Professor Sarah Angell || CDA3013C
Thank you so much for grading my code!
*/
/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	//fprintf(stdout, "A and B are: %d  %d ",A,B);
	if(ALUControl == 0b0)
	{
		unsigned aHold = A;
		unsigned bHold = B;
		*ALUresult = (aHold + bHold);
	}
	else if(ALUControl == 0b1)
	{
		unsigned aHold = A;
		unsigned bHold = B;
		*ALUresult = (aHold - bHold);
	}
	else if(ALUControl == 0b10)//signed version as the values come in ALREADY SIGNED
	{
			int hold = A-B;
			if(hold<0){
				*ALUresult = 0b1;
			}
			else *ALUresult = 0b0;
	}
	else if(ALUControl == 0b11)//unsigned version, takes a signed val and takes it out of twos comp
	{
		//fprintf(stdout,"IN SLTU IN ALU \n");
		unsigned aHold = A;
		unsigned bHold = B;

		//fprintf(stdout,"a and b: %d\t%d\n",aHold,bHold);
		if((aHold & 0b10000000000000000000000000000000)>>31 == 0b1)
		{
			aHold = ~(aHold) + 0b1;
		//	fprintf(stdout,"a: %d\n",aHold);
		}
		if((bHold & 0b10000000000000000000000000000000)>>31 == 0b1)
		{
			bHold = ~(bHold) + 0b1;
			//fprintf(stdout,"b: %d\n",bHold);
		}
			int hold = aHold-bHold;
			if(hold < 0){
				*ALUresult = 0b1;
			}
			else *ALUresult = 0b0;
	}
	else if(ALUControl == 0b100)
	{
			*ALUresult = A & B;
	}
	else if(ALUControl == 0b101)
	{
			*ALUresult = A | B;
	}
	else if(ALUControl == 0b110)
	{
			*ALUresult = B<<16;
	}
	else if(ALUControl == 0b111)
	{
			*ALUresult = ~A;
	}		

	if(ALUresult == 0b0)
		{
				*Zero = 0b1;
		}
		else
		{
			*Zero = 0b0;
		}
//	fprintf(stdout,"alu result: %d",*ALUresult);
	
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if(PC % 4 != 0){
		return 1;//makes halt 1 lolll
	} 
	else{
		*instruction = Mem[PC/4];
	}
	return 0;

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
//	fprintf(stdout,"inst %d\n",instruction);
	//my dyslexia kicked in trying to do this by hand :(
	//but i got through it!!!
	*op = (instruction & 0b11111100000000000000000000000000)>>26;//c
	*r1 = (instruction & 0b00000011111000000000000000000000)>>21;
	*r2 = (instruction & 0b00000000000111110000000000000000)>>16;
	*r3 = (instruction & 0b00000000000000001111100000000000)>>11;
	*funct = (instruction & 0b00000000000000000000000000111111);
	*offset = (instruction & 0b00000000000000001111111111111111);
	*jsec = (instruction & 0b00000011111111111111111111111111);

//	fprintf(stdout,"checking split\nop %d\nr1 %d\nr2 %d\nr3 %d\n funct %d\n offset %d\n jsec %d\n",*op,*r1,*r2,*r3,*funct,*offset, *jsec);
}



/* instruction decode */
/* 15 Points */
//check here for some errors 
int instruction_decode(unsigned op,struct_controls *controls)
{
		//fprintf(stdout,"in decode %d\n",op);

		unsigned lastThree = (op & 0b000111);
		unsigned firstThree = (op & 0b111000) >>3;
		//fprintf(stdout,"fl %d %d\n",lastThree,firstThree);	
		
		if(op == 0b0){
		//	fprintf(stdout,"r\n");
			//r type
			controls->RegDst = 0b1;
			controls->Jump = 0b0;
			controls->Branch = 0b0;
			controls->MemRead = 0b0;
			controls->MemtoReg = 0b0;
			controls->ALUOp = 0b111;
			controls->MemWrite = 0b0;
			controls->ALUSrc = 0b0;
			controls->RegWrite = 0b1;
			return 0;
		}
		//TODO: dont think everything is a 2
		else if(firstThree == 0b0 && lastThree == 0b10){
		//	fprintf(stdout,"j\n");
			//jump
			controls->RegDst = 0b10;//ood
			controls->Jump = 0b1;//good
			controls->Branch = 0b0;//good
			controls->MemRead = 0b0;//good
			controls->MemtoReg = 0b10;
			controls->ALUOp = 0b0;//good
			controls->MemWrite = 0b0;//good
			controls->ALUSrc = 0b10;//good
			controls->RegWrite = 0b0;//good
			return 0;

		}
		else if(firstThree == 0b0 && lastThree == 0b100){
			//fprintf(stdout,"beq\n");
			//beq
			controls->RegDst = 0b10;
			controls->Jump = 0b0;
			controls->Branch = 0b1;
			controls->MemRead = 0b0;
			controls->MemtoReg = 0b10;
			controls->ALUOp = 0b0;
			controls->MemWrite = 0b0;
			controls->ALUSrc = 0b0;
			controls->RegWrite = 0b0;
			return 0;
		}
		else if(firstThree == 1){
			//i type
		//fprintf(stdout,"i type!");	
			controls->RegDst = 0b0;
			controls->Jump = 0b0;
			controls->Branch = 0b0;
			controls->MemRead = 0b0;
			controls->MemtoReg = 0b0;
			controls->MemWrite = 0b0;
			controls->ALUSrc = 0b1;
			controls->RegWrite = 0b1;

			if(lastThree == 0b0){
				//addi
				//fprintf(stdout,"addi");	
				controls->ALUOp = 0b0;
				return 0;
			}
			else if(lastThree == 0b10)
			{
				//slti signed
				controls->ALUOp = 0b010;
				return 0;
			}
			else if(lastThree == 0b11)
			{
				//slti unsigned
				controls->ALUOp = 0b011;
				return 0;
			}
			//this needs work
			else if(lastThree == 0b111)
			{
				controls->ALUOp = 0b110;
				return 0;
			}
			
		}
		
		else if(firstThree == 0b100 && lastThree == 0b011){
			//lw
			//fprintf(stdout,"lw\n");
			controls->RegDst = 0b0;
			controls->Jump = 0b0;
			controls->Branch = 0b0;
			controls->MemRead = 0b1;
			controls->MemtoReg = 0b1;
			controls->ALUOp = 0b0;//lw adds registers
			controls->MemWrite = 0b0;
			controls->ALUSrc = 0b1;
			controls->RegWrite = 0b1;
			return 0;
		}
		else if(firstThree == 0b101 && lastThree == 0b011){
			//sw
			//fprintf(stdout,"sw\n");
			controls->RegDst = 0b1;
			controls->Jump = 0;
			controls->Branch = 0b0;
			controls->MemRead = 0b0;
			controls->MemtoReg = 0b0;
			controls->ALUOp = 0b0;
			controls->MemWrite = 0b1;
			controls->ALUSrc = 0b1;
			controls->RegWrite = 0b0;
			return 0;
		}
		return 1;
}

/* Read Register */
/* 5 Points */
//TODO: check is working
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
//TODO: check if working
void sign_extend(unsigned offset,unsigned *extended_value)
{
	if((offset & 0b00000000000000001000000000000000)>>15 == 0b1)//so if the sign bit is one this will run, otherwise no go
	{
		*extended_value = offset | 0b11111111111111110000000000000000;
	}
	else{
		*extended_value = offset;
	}
//	fprintf(stdout,"e val  %d \n",*extended_value);	
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	if(ALUOp == 0b111){
		//its an r type
		unsigned upperHalf = (funct & 0b111000) >> 3;
		unsigned lowerHalf = funct & 0b000111;

		if(upperHalf == 0b100){

				if(lowerHalf == 0b0){
					//add
					ALUOp = 0b000;
				}	
				if(lowerHalf == 0b10){
					//sub
					ALUOp = 0b001;
				}
				if(lowerHalf == 0b100){
					//and
					ALUOp = 0b100;
				}
				if(lowerHalf == 0b101){
					//or
					ALUOp = 0b101;
				}
		}
		else if(upperHalf == 0b101){
			if(lowerHalf == 0b010){
					//slt
					//fprintf(stdout,"IN SLT \n");
					ALUOp = 0b010;
				}
			if(lowerHalf == 0b011){
					//sltu
					ALUOp = 0b011;
				}
		}
		else{
			return 1;
		}
	}
	
	if(ALUSrc == 0b0){
		//r
	//fprintf(stdout,"READ AS R TYPE \n");
		ALU(data1,data2,ALUOp,ALUresult,Zero);
	}
	else if(ALUSrc == 0b1){
		ALU(data1,extended_value,ALUOp,ALUresult,Zero);
	}
	else if(ALUSrc != 0b10){
		return 1;
	}

	return 0;
	 
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	if(ALUresult % 4 != 0b0 && (MemWrite || MemRead)){
		return 0b1;
	}

	if(MemWrite == 0b1){
			Mem[ALUresult/4] = data2;
		//	fprintf(stdout,"mem at sw %d\n",Mem[data2/4]);
	}
	else if(MemRead == 0b1){
			*memdata=Mem[ALUresult/4];
	}

	return 0;
}


/* Write Register */
/* 10 Points */
//TODO: RED COMMENT IN FUNC
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if(RegWrite == 0b0){
			return;
	}
	else{
		if(MemtoReg == 0b1){
			//lw
			Reg[r2] = memdata;
		}
		else{
			if(RegDst ==  0b0){
				//	fprintf(stdout,"wrote to reg  :))))))))\n");	
			   	Reg[r2] = ALUresult;
			}
			else{
			//		fprintf(stdout,"wrote to reg  r\n");	
					Reg[r3] = ALUresult;
			}
			
		}
	}
}

/* PC update */
/* 10 Points */
//TODO: NOT WORKING PROPERLY
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	
	*PC = *PC + 4;

	if(Branch == 0b1){
		if(Zero == 0b0){
			*PC = *PC + (extended_value<<2);
		}
	}
	else if(Jump == 0b1){//might be wrong idk :C
		//	fprintf(stdout,"we winnin j type \n");
			*PC = ( (*PC &0b11110000000000000000000000000000) | (jsec<<2));
	}
}

