// the purpose of this lesson is changing the flow of control through code
// pc-program counter, xpsr-program status register
// cmp compare r0 with value-> subs r0 with 0x04 and update psr true then BGT->branch if N=0-> 
// compare was positive false skip
// changing the code a bit can save some branches and runtime
int main(){
	int counter= 0;
	/*
	while(counter<5){ 
		++counter;
		++counter;
	}
	*/
		while(counter<5){ 
		++counter;
		if((counter & 1)!=0){ //if odd
		__asm("NOP"); // do nothing
		}
	}
return 0;
}