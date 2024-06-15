/* the purpose of this lesson is 
showing the RISC arcitecutre by viewing the ops of the regs in order to manipulate the memory.
In ARM assembly language, 
MOVW R0, #0x1234   ; Move 0x1234 to the lower half of R0
MOVT R0, #0x5678   ; Move 0x5678 to the upper half of R0
Explanation
MOVT: This is the instruction mnemonic for "Move Top".
It moves a 16-bit immediate value into the upper half of a 32-bit register.
MOVW: This is the instruction mnemonic for "Move Wide".
It moves a 16-bit immediate value into the lower half of 32-bit register.
After executing both instructions, R0 will hold the value 0x56781234.

The S suffix in instructions like SUBS stands for "Set flags."->xPSR
*/
/*
int counter= 0;
int main(){
		while(counter<5){ 
		++counter;
	}
return 0;
}
*/
int counter =0;
int main(){
	int *p=&counter; // first right then left
		while(*p<5){ 
		++(*p);
	}
	//testing hack with pointers on board! Cortex M4 accept it M0 not
	p =(int*)0x20000060U;
	*p=0xDEADBEEF;
return 0;
}
// using pointers was more efficiante than the previous way
