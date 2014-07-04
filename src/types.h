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

#ifndef h__dvm_types__
#define h__dvm_types__

//Instructions supported by the VM
enum Instruction {
  NOP = 0,

  //MATH OPERATIONS
  ADD,    //1: Add something to a register
  INC,    //2: Increase the value in a register
  DEC,    //3: Decrease the value in a register
  SUB,    //4: Subtract something from a register
  MUL,    //5: Decrease the value in a register
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


//Register
/* Yes, we don't really need to number everything, but
   this lets us move stuff around without breaking 
   existing bytecode */
enum Operand {
  R_NONE = 0,

  //16-bit registers
  R_AS = 1,
  R_BS = 2, 
  R_CS = 3, 
  R_DS = 4, 

  //32-bit registers
  R_II = 5, 
  R_JI = 6, 
  R_KI = 7, 
  R_LI = 8, 

  //Float registers
  R_XF = 9, 
  R_YF = 10, 
  R_ZF = 11, 
  R_WF = 12, 

  R_SH = 13, //Short constant follows
  R_FL = 14, //Float constant follows
  R_IN = 15, //Int constant follows

};





#endif