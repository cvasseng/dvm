dvm
===

A small virtual machine written in C/C++.

The implementation is written to be easy to read and understand, and to be small (both in terms of the bytecode it runs on, and for the implementation). 

# Why

I built it because I needed a small lightweight virtual machine for one of my own projects. Existing projects were generally way to large for my use case which was size limited. Once I started writing it I figured it might be useful for other 
people, both in terms of serving an educational purpose and also as a very
lean alternative/starting point for doing scripting in C/C++ applications.

# Features
 * Sub routines
 * C functions can be bound and called from the VM
 * Mathematical operations (sin, cos, etc.)
 * Supports 16 and 32-bit integers as well as floats
 * 12 registers (4x 16 bit, 4x 32 bit, 4x float)
 * Comes with a simple parser/compiler that compiles assembly-ish syntax to bytecode
 * Does not require Boost or any other bloated libraries (the VM itself only rely on string.h, stdio.h (if logging is enabled) and math.h)
 * The VM core is less than 500 lines of well-commented code

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

## Built-in functions

todo

## Build-Time Defines 

There are a couple of defines you can use to specify how much (if any) logging
you want to have from the vm. If `PROGRAM_LOG` is set, the VM will spit out 
information for each instruction it executes, which can sometimes be useful
for debugging.

## Supported Operations
This is a list of all the supported operations in the VM itself. 

### Mathematical operations
 * Add - Basic mathematical add. Can be used to add one register to another, or 
 to add a constant number to the value of a register
 * Inc - Increments the value in a register by one
 * Dec - Decrements the value of a register by one
 * Sub - Subtracts the value in a register by one
 * Mul - Multiplies the value in a register by either a constant value or the value in a second register
 * Div - Divides the value in a register by either a constant value or the value in a second register

### Jumps
 * Jmp - Regular jump
 * Jl - Jump if last comparison was less than
 * Jg - Jump if last comparison was greater than
 * Je - Jump if last comparison was equal
 * Jn - Jump if last comparison was not equal
 * Jle - Jump if last comparison was less than or equal
 * Jge - Jump if last comparison was greater than or equal

## Bytecode Details

todo

# The DVM assembly language

todo

# Integrating in C/C++ applications

todo

# Further reading

todo

# License

Copyright (c) 2014, Chris Vasseng  
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of the <organization> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

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
