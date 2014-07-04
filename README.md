dvm
===

A small virtual machine written in C/C++.

The implementation is written to be easy to read and understand, and to be small (both in terms of the bytecode it runs on, and for the implementation). 

# Features
 * Sub routines
 * C functions can be bound and called from the VM
 * Mathematical operations (sin, cos, etc.)
 * Supports 16 and 32-bit integers as well as floats
 * 12 registers (4x 16 bit, 4x 32 bit, 4x float)
 * Comes with a compiler that compiles assembly-ish syntax to bytecode

# Example program

This is an example of a program written in assembly-ish syntax 
that compiles to dvm bytecode. 

    
    MOV as,#0	    ;as = 0
    MOV bs,#10    ;bs = 10
     
    LOOP: 				
	  INC as        ;as++
	  CMP as,bs 			
	  JL LOOP       ;if (as < bs) goto LOOP

The program will do a loop until the value of the a and b registers are equal. 
In byte code the program looks like this:

    0x091D 0x0000     ;mov as,0
    0x092D 0x000A     ;mov bs,10
    0x1200            ;LOOP:
    0x0210            ;inc as
    0x0E12            ;cmp as,bs
    0x1400            ;jl loop

# Usage

# License

Copyright (c) 2014, Chris Vasseng
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IQUMULUS LLC BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
