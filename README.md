dvm
===

A small register based virtual machine written in C/C++.

The implementation is written to be easy to read and understand, and to be small (both in terms of the bytecode it runs on, and for the implementation). 

Might be useful for educational purposes, or as a starting point/inspiration
for a more complete VM.

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

## Using DVM as an embedded scripting solution

Include the cpp files in `src/` in your project. You only need to include `dvm.h`.
To compile a DVM assembly program to bytecode, use `dvm_compile(const char* filename)`. It accepts a filename and returns a structure containing the 
bytecode and the size of the program. This can be fed into the VM using `dvm_run`.

Example:
    
    #include "dvm.h"

    int main(int argc, const char * argv[]) {

      ProgramSource p = dvm_compile("examples/test.dvm");
  
      dvm_run(p.program, p.programSize);
      
      return 0;
    }

### Binding C functions to the VM

Functions can be binded to the VM by using the `void dvm_include()` function in `dvm.h`. This function accepts two arguments - a numeric ID unique for the function (0..255) and a pointer to a function with the signature `void fn(float *args, int argc);`.

C functions are called as such in DVM ASM:

    symbol printf,<id>  ;allows us to use a string instead of a number when calling it

    arg as      ;push as onto the argument stack
    arg bs      ;push bs onto the argument stack
    call printf ;printf(as, bs)
    
Notice the `symbol` keyword - this isn't used in the bytecode, but is 
used by the parser as an alias.     


## Build-Time Defines 

There are a couple of defines you can use to specify how much (if any) logging
you want to have from the vm. If `PROGRAM_LOG` is set, the VM will spit out 
information for each instruction it executes, which can sometimes be useful
for debugging.

## Supported Operations
This is a list of all the supported operations in the VM itself. 

### General
 * MOV - Move a value (literal or contents of a register) into a register. Note: syntax is destination,source.
 * Push - Push a value onto the stack (either a literal or a register)
 * Pop - Pop the top item off of the stack and into a register
 * Call - Call a C function
 * Cmp - Compare two values (or registers)
 * Ret - Return from a sub-routine
 * Fn - Sub routine function declaration
 * Do - Call a sub-routine

### I/O
  * Print - Prints a literal or the contents of a register
  * Printl - Same as print, but adds a newline

### Mathematical Operations
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

Each instruction is 16 bits long, and includes both the operation itself
and up to two operands (either registers or constant data). The operation is stored in byte 1, and the registers is stored in byte 2:

| BIT 5-8 | BIT 3-4  | BIT 1-2  |
|---------|----------|----------|
| OP CODE | OPERAND1 | OPERAND2 |

So in the case of the instruction `mov as,bs`, it would look like this `0x0912`
in bytecode since the mov instruction is `9`, register `as` is `1` and register `bs`
is `2`. Instructions with constant values will instead of a register number have a 
number corresponding with the data type of the constant, followed by the data 
itself in separate 16 bit integers. So the instruction `mov as,#10` would look
like this `0x091D 0x000A` in byte code. `9` is the mov operation, `1` is `as`, and `D` indicates that the right side has a constant and it's a 16-bit integer. The next 2 bytes is the number itself. It deduces the type of the constant based on the 
register type on the left side. The registers ending in s are all 16-bit 
(`as bs cs ds`), the registers ending in i are all 32-bit (`ii ji ki li`), and
the registers ending in f are all floats (`xf, yf, zf, wf`).

## Adding new Operations

Adding new operations is fairly simple. There's no need to change the parser/compiler
to make it work, other than adding your operation to `ins_name_to_num` in
`compiler.cpp`. The VM itself requires that you add the operations in two places;
it must be added to the `Instruction` enum in `types.h`, and the logic must be
implemented in the `switch` in `dvm_run` in `dvm.cpp`. That's pretty much it.

# The DVM Assembly Language

The syntax is loosly based on NASM syntax. This means that in general the 
destination for an operation is defined first on operations, e.g. `mov destination,source`, `add destination,what`.

Literals must be prefixed with `#`, e.g. `mov bs,#10` will move the number 10 into register `bs`.

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
