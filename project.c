#include "spimcore.h"
/*
NOTES TO ME

1. Look over reading and writing to memory. It could be that an older process isint working exactly too, so
2. I dont think branch is working either, look at how branch works
3. Cover addidion and subtraction overflow.


*/

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	if(ALUControl == 0)//TODO: need to cover overflow :)
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
		*ALUresult = (aHold + bHold);
		if((*ALUresult & 0b10000000000000000000000000000000)>>31)
		{
			*ALUresult = ~*ALUresult + 1;//back to twos comp
		}
	}
	else if(ALUControl == 1)//TODO: cover overflow
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
		*ALUresult = (aHold - bHold);
		if((*ALUresult & 0b10000000000000000000000000000000)>>31)
		{
			*ALUresult = ~*ALUresult + 1;//back to twos comp
		}
	
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
	fprintf(stdout,"alu result: %d",*ALUresult);
	
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
	fprintf(stdout,"inst %d\n",instruction);
	//my dyslexia kicked in trying to do this by hand :(
	//but i got through it!!!
	*op = (instruction & 0b11111100000000000000000000000000)>>26;//c
	*r1 = (instruction & 0b00000011111000000000000000000000)>>21;
	*r2 = (instruction & 0b00000000000111110000000000000000)>>16;
	*r3 = (instruction & 0b00000000000000001111100000000000)>>11;
	*funct = (instruction & 0b00000000000000000000000000111111);
	*offset = (instruction & 0b00000000000000001111111111111111);
	*jsec = (instruction & 0b00000011111111111111111111111111);

	fprintf(stdout,"checking split\nop %d\nr1 %d\nr2 %d\nr3 %d\n funct %d\n offset %d\n jsec %d\n",*op,*r1,*r2,*r3,*funct,*offset, *jsec);
}



/* instruction decode */
/* 15 Points */
//check here for some errors 
int instruction_decode(unsigned op,struct_controls *controls)
{
		fprintf(stdout,"in decode %d\n",op);

		unsigned lastThree = (op & 0b000111);
		unsigned firstThree = (op & 0b111000) >>3;
		fprintf(stdout,"fl %d %d\n",lastThree,firstThree);	
		
		if(op == 0b0){
			fprintf(stdout,"r\n");
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
		//TODO: dont think everything is a 2
		else if(firstThree == 0 && lastThree == 2){
			fprintf(stdout,"j\n");
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
		else if(firstThree == 0 && lastThree == 4){
			fprintf(stdout,"beq\n");
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
		else if(firstThree == 1){
			//i type
		fprintf(stdout,"i type!");	
			controls->RegDst=0;
			controls->Jump= 0;
			controls->Branch=0;
			controls->MemRead=0;
			controls->MemtoReg=0;
			controls->MemWrite =0;
			controls->ALUSrc=1;
			controls->RegWrite=1;

			if(lastThree == 0b0){
				//addi
					fprintf(stdout,"addi");	
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
			//this needs work
			else if(lastThree == 7)
			{
				//load upper immediate TODO
				controls->ALUOp=0b110;
				return 0;
			}
			
		}
		
		else if(firstThree == 4 && lastThree == 3){
			//lw
			fprintf(stdout,"lw\n");
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
		else if(firstThree == 5 && lastThree == 3){
			//sw
			fprintf(stdout,"sw\n");
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
	if((offset & 0b00000000000000001000000000000000)>>15)//so if the sign bit is one this will run, otherwise no go
	{
		*extended_value = offset | 0b11111111111111110000000000000000;
	}
	else{
		*extended_value = offset;
	}
	fprintf(stdout,"e val  %d \n",*extended_value);	
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	if(ALUOp==0b111){
		//its an r type
		unsigned upperHalf = (funct & 0b111000)>>3;
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
					//sub
					ALUOp = 0b010;
				}
			if(lowerHalf == 0b011){
					//sub
					ALUOp = 0b011;
				}
		}
		else{
			return 1;
		}
	}
	//TODO: missing one thing, a check whether im using extended or data
	if(ALUSrc == 0b0){
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
//TODO:sw not working, has to be coming from here. FIX THIS
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	if(data2%4!=0 && (MemWrite||MemRead)){
		return 0b1;
	}

	if(MemWrite==0b1){
			Mem[data2/4]=ALUresult;
			fprintf(stdout,"mem at sw %d\n",Mem[data2/4]);
	}
	else if(MemRead==0b1){
			*memdata=Mem[data2/4];
	}

	return 0;
}


/* Write Register */
/* 10 Points */
//TODO: RED COMMENT IN FUNC
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if(RegWrite == 0){
			return;
	}
	else{
		if(MemtoReg == 1){
			//lw
			Reg[r2] = memdata;
		}
		else{
			if(RegDst == 0 ){
					fprintf(stdout,"wrote to reg  :))))))))\n");	
			   	Reg[r2] = ALUresult;
			}
			else{
					fprintf(stdout,"wrote to reg  r\n");	
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
	
	*PC = *PC+4;

	if(Branch){
		if(!Zero){
			*PC = *PC +  4*(jsec & 0b00000000001111111111111111);
		}
	}
	else if(Jump){//might be wrong idk :C
			*PC = ((*PC &0b11110000000000000000000000000000)&(jsec<<2));
	}
}

