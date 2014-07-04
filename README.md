dvm
===

A small virtual machine written in C/C++.

The implementation is written to be easy to read and understand, and to be small (both in terms of the bytecode it runs on, and for the implementation). 

# Features
	* Sub routines
	* Possible to call C functions from the VM

# Example program

This is an example of a program written in assembly-esque syntax 
that compiles to dvm bytecode. 

    
    MOV as,#0			;as = 0
    MOV bs,#10		;bs = 10
     
    LOOP: 				
	  INC as				;as++
	  CMP as,bs 			
	  JL LOOP				;if (as < bs) goto LOOP

The program will do a loop until the value of the a and b registers are equal. 
In byte code, the program looks like this:

    0x091D 0x0000 //mov as,0
    0x092D 0x000A //mov bs,10
    0x1200 				//LOOP:
    0x0210	 			//inc as
    0x0E12				//cmp as,bs
    0x1400				//jl loop
    
    
