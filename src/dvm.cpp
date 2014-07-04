/*******************************************************************************

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

*******************************************************************************/

/*

- data types determined by type of register.
  supported:
    * int16 [as, bs, cs, ds]
    * int32 [ii, ji, ki, li]
    * float [xf, yf, zf, wf]

  Test program:

  MOV as,#0       ;a = 0
  MOV bs,#10      ;b = 10
  
  LOOP:         
    INC as        ;a++
    CMP as,bs       
    JL LOOP       ;if (a < b) goto LOOP

  =>
  
  0x0910 0x0000 //mov as,0
  0x0920 0x000A //mov bs,10
  0x1200        //LOOP:
  0x0210        //inc as
  0xE011        //cmp as,bs
  0x1400        //jl loop

*/


////////////////////////////////////////////////////////////////////////////////

#define MAX_PROGRAM_SIZE  1024
#define MAX_SYMBOLS       256

#define PROGRAM_LOG 1

//If enabled, the VM will log everything it does to stdout
#ifdef PROGRAM_LOG
#   define DEBUG_PLOG(x) printf x
#else
#   define DEBUG_PLOG(x) do {} while (0)
#endif

#include <string.h>
#include <stdio.h>

#include "dvm.h"

//Instructions supported by the VM
enum Instruction {
  NOP = 0,

  //MATH OPERATIONS
  ADD,    //1: Add something to a register
  INC,    //2: Increase the value in a register
  SUB,    //3: Subtract something from a register
  MUL,    //4: Decrease the value in a register
  DEC,    //5: Decrease the value in a register
  DIV,    //6: Divide
  SIN,    //7: Sin
  COS,    //8: Cos
  
  //MISC
  MOV,    //9: Move something into a register
  PUSH,   //10: Push something to the stack
  POP,    //11: Pop something off the stack
  ARG,    //12: Push an argument onto the call stack
  CALL,   //13: Call something
  CMP,    //14: Compare two registers
  RET,    //15: Return from a sub routine
  FN,     //16: Function label
  DO,     //17: Call a function label
  LBL,    //18: A label (15)
  
  //JUMPS
  JMP,    //19: Do a jump
  JL,     //20: Jump if less than
  JG,     //21: Jump if greater than
  JE,     //22: Jump if equals
  JN,     //23: Jump if not equals
  JLE,    //24: Jump if less than or equal
  JGE,    //25: Jump if greater than or equal
  
};

//Describes a comparison result
enum CompareResult {
  LESS,
  GREATER,
  LEQUAL,
  GEQAUL,
  EQUAL,
  NEQUAL
};

//Register
enum Operand {
  R_NONE = 0,

  //16-bit registers
  R_AS, //1
  R_BS, //2
  R_CS, //3
  R_DS, //4

  //32-bit registers
  R_II, //5
  R_JI, //6
  R_KI, //7
  R_LI, //8

  //Float registers
  R_XF, //9
  R_YF, //10
  R_ZF, //11
  R_WF, //12

  R_SH, //13: Short constant follows
  R_FL, //14: Float constant follows
  R_IN, //15: Int constant follows

};

//Contains the current state of a VM
typedef struct VM {
  //int16 registers
  short int16Reg[4]; //as, bs, cs, ds
  //int registers
  int   int32Reg[4]; //ii, ji, ki, li
  //float registers
  float floatReg[4]; //xf, yf, zf, wf

  //Our symbols
  short symbols[MAX_SYMBOLS];

  //The program we're currently running
  short program[MAX_PROGRAM_SIZE];
  //The program cursor - our position within the program array
  int programCursor;
  //The size of the program in memory
  int programSize;

  //Stores the result of the last compare preformed
  CompareResult lastCmp;
  //The cursor before a jump
  int beforeCursor;

} VM;

////////////////////////////////////////////////////////////////////////////////

//Read two bytes from the program and return it
inline short vm_read2b(VM &v) {
  return v.program[++v.programCursor];  
}

//Read four bytes from the program and return it
inline int vm_read4b(VM &v) {
  return v.program[++v.programCursor] << 8 | v.program[++v.programCursor];
}

//Get the value of an operand relative to current program cursor
inline float vm_getOperandValue(Operand opa, VM &v) {
  if (opa > 0) {
    if (opa < 5) {
      return v.int16Reg[opa];
    } else if (opa < 9) {
      return v.int32Reg[opa];
    } else if (opa < 13) {
      return v.floatReg[opa];
    } else if (opa == 13) {
      //Read 2 bytes
      return vm_read2b(v);
    } else {
      //Read 4 bytes
      return vm_read4b(v);
    }
  }
  return -1.1337f;
}

//Write to a register
inline void regw(Operand opa, VM &v, float val) {
  if (opa > 0) {
    if (opa < 5) {
      v.int16Reg[opa] = val;
    } else if (opa < 9) {
      v.int32Reg[opa] = val;
    } else if (opa < 13) {
      v.floatReg[opa] = val;
    }
  }
}

//Preform a jump in the program. This is repeated quite a lot, so it's 
//refactored into a separate function to avoid too much code repetition.
inline void jmp(int where, VM &v) {
  if (v.symbols[where] < v.programSize) {
    v.beforeCursor = v.programCursor;
    v.programCursor = v.symbols[where];

    DEBUG_PLOG(("Jumped to %i\n", v.programCursor));
  }
}

////////////////////////////////////////////////////////////////////////////////

//Prepare a VM 
void dvm_vm_clear(VM &v) {
  v.programSize = 0;
  v.programCursor = 0;
}

//Reset the program within a vm
void dvm_vm_reset(VM &v) {
  v.programCursor = 0;
}

//Run the program in a vm
void dvm_run(VM &v) {
  Instruction ins;
  Operand opa;
  Operand opb;
  float lValue;
  float rValue;
  short c;
  char lbyte;

  //We first need to do a pre-pass to gather all the symbols in the program.
  int cursor = 0;
  while (cursor < v.programSize) {
    ins = Instruction((v.program[cursor] & 0xFF00) >> 8);
    if (ins == LBL || ins == FN) {
      v.symbols[v.program[cursor] & 0x00FF] = cursor;
    } 
    cursor++;
  }

  //Start our actual pass (or resume where we left of)
  while (v.programCursor < v.programSize) {
    c = v.program[v.programCursor];

    ins = Instruction((c & 0xFF00) >> 8); //The instruction
    opa = Operand((c & 0x00F0) >> 4);     //Left side operand
    opb = Operand( c & 0x000F);           //Right side operand
    lbyte = c & 0x00FF;                   //Symbol 

    lValue = vm_getOperandValue(opa, v);
    rValue = vm_getOperandValue(opb, v);

    switch (ins) {
      
      //Moves the right value into a register. 
      case MOV: 
        if (opa > 0 && opa < 13) {
          regw(opa, v, rValue);
          
          DEBUG_PLOG(("MOV %f into register %i\n", rValue, opa));
        }
        break;
      
      //Increments the value in a register
      case INC:
        if (opa > 0 && opa < 13) {
          regw(opa, v, ++lValue);
          
          DEBUG_PLOG(("INC register %i\n", opa));
        }
        break;
      
      //Compare two registers or values
      case CMP:     
        if (lValue > rValue) v.lastCmp = GREATER;
        else if (lValue < rValue) v.lastCmp = LESS;
        else if (lValue == rValue) v.lastCmp = EQUAL;
        else v.lastCmp = NEQUAL;

        DEBUG_PLOG(("CMP %f with %f\n", lValue, rValue));
    
        break;

      //Jump if less than
      case JL:  
        if (v.lastCmp == LESS) jmp(lbyte, v);
        break;    
      
      //Jump if greater than
      case JG:    
        if (v.lastCmp == GREATER) jmp(lbyte, v);
        break;
      
      //Jump if equals
      case JE:  
        if (v.lastCmp == EQUAL) jmp(lbyte, v);
        break;
      
      //Jump if not equals
      case JN:    
        if (v.lastCmp != EQUAL) jmp(lbyte, v);
        break;
      
      //Jump if less than or equal
      case JLE:   
        if (v.lastCmp == EQUAL || v.lastCmp == LESS) jmp(lbyte, v);
        break;
      
      //Jump if greater than or equal
      case JGE:   
        if (v.lastCmp == EQUAL || v.lastCmp == GREATER) jmp(lbyte, v);
        break;

      //Do a jump
      case JMP:   
        jmp(lbyte, v);
        break;
      
      default:
        break;
    };

    ++v.programCursor;
  }

}

void dvm_run(const short *prog, unsigned int size) {
  VM v;
  dvm_vm_clear(v);
  v.programSize = size;
  memcpy(&v.program, prog, sizeof(short) * size);
  dvm_run(v);
}

