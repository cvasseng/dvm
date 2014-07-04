dvm
===

A small virtual machine written in C/C++.

The implementation is written to be easy to read and understand, and to be small (both in terms of the bytecode it runs on, and for the implementation). 

# Example program

This is an example of a program written in assembly-esque syntax 
that compiles to dvm bytecode. 

  MOV a,#0				;a = 0
  MOV b,#10				;b = 10
  
  LOOP: 				
	  INC a					;a++
	  CMP a,b 			
	  JL LOOP				;if (a < b) goto LOOP
  
The program will do a loop until the value of the a and b registers are equal. 