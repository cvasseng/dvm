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

#include <stdio.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>

#include "dvm.h"
#include "types.h"

//Convert the name of an instruction to its number
Instruction ins_name_to_num(std::string str) {
  //Instructions are not case sensitive
  std::transform(str.begin(), str.end(),str.begin(), ::toupper);

  if (str == "NOP")     return NOP;
  if (str ==  "ADD")    return ADD;   
  if (str ==  "INC")    return INC;   
  if (str ==  "SUB")    return SUB;   
  if (str ==  "MOV")    return MOV;   
  if (str ==  "PUSH")   return PUSH;  
  if (str ==  "CALL")   return CALL;    
  if (str ==  "CMP")    return CMP;   
  if (str ==  "JMP")    return JMP;   
  if (str ==  "JL")     return JL;  
  if (str ==  "JG")     return JG;  
  if (str ==  "JE")     return JE;  
  if (str ==  "JN")     return JN;  
  if (str ==  "JLE")    return JLE; 
  if (str ==  "JGE")    return JGE; 
  if (str ==  "LBL")    return LBL; 
  if (str ==  "BREAK")  return RET;
  if (str ==  "RET")    return RET;
  if (str ==  "DO")     return DO;
  if (str ==  "FN")     return FN;
  if (str ==  "DEC")    return DEC;
  if (str ==  "MUL")    return MUL;
  if (str ==  "DIV")    return DIV;
  if (str ==  "SIN")    return SIN;
  if (str ==  "COS")    return COS;
  if (str ==  "POP")    return POP;
  if (str ==  "ARG")    return ARG;

  return NOP;
}

//Convert the name of an operand to its number
Operand reg_name_to_num(std::string str) {
  //16-bit registers
  if (str== "as") return R_AS; //1
  if (str== "bs") return R_BS; //2
  if (str== "cs") return R_CS; //3
  if (str== "ds") return R_DS; //4

  //32-bit registers
  if (str== "ii") return R_II; //5
  if (str== "ji") return R_JI; //6
  if (str== "ki") return R_KI; //7
  if (str== "li") return R_LI; //8

  //Float registers
  if (str== "xf") return R_XF; //9
  if (str== "yf") return R_YF; //10
  if (str== "zf") return R_ZF; //11
  if (str== "wf") return R_WF; //12

 // R_SH, //13: Short constant follows
 // R_FL, //14: Float constant follows
 // R_IN, //15: Int constant follows

  return R_NONE;
}

typedef struct Program {
  short program[1024];
  int programSize;

  std::string symMap[256];
  int symCount;

  Program() {
    programSize = -1;
    symCount = 0;
  }

} Program;

//Find the number of a symbol. Returns -1 if not found
int sym_find(Program &p, const std::string& str) {
  for (int i = 0; i < p.symCount; i++) {
    if (p.symMap[i] == str) {
      return i;
    }
  }
  return -1;
}

//Returns the number of a symbol. Creates one if one does not exist.
int sym_get_or_create(Program &p, const std::string& str) {
  int index = sym_find(p, str);

  if (index == -1) {
    index = p.symCount;
    p.symMap[index] = str;
    p.symCount++;
  }

  printf("Assigned symbol '%s' -> %i\n", str.c_str(), index);

  return index;
}

//Parse a single tokenized line
void parse_line(Program &p, std::vector<std::string> &l) {
  printf("Found %i tokens on line: \n", l.size());
  for (int i = 0; i < l.size(); i++) {
    printf("    %s\n", l[i].c_str());
  }

  //Check if we're dealing with a label definition
  if (l[0][l[0].size() - 1] == ':') {
    std::string label = l[0].substr(0, l[0].size() - 1);
    int index = sym_get_or_create(p, label);
    p.program[++p.programSize] = LBL << 8 | (char)index;
    return;
  }

  if (l[0] == "symbol") {
    sym_get_or_create(p, l[1]);
    return;
  }

  Instruction op = ins_name_to_num(l[0]);

  if (op == NOP) {
    return;
  }

  p.program[++p.programSize] = (op << 8);
  
  printf("Instruction: %X\n", p.program[p.programSize]);

  //Now we need to figure out what kind of arguments we're dealing with
  for (int i = 1; i < l.size(); i++) {
    //Is it a register?
    Operand r = reg_name_to_num(l[i]);
    if (r != R_NONE) {
      p.program[p.programSize] |= (char)r << (i == 1 ? 4 : 0);
    } else {

      //Is it a number?
      if (l[i][0] == '#') {
        //We need to figure out which type to use based on the previous register
        std::stringstream ss(l[i].substr(1));
        short num = 0;
        ss >> num ;
        //memcpy(&p.program[++p.programSize], &num, sizeof(float));

        p.program[p.programSize] |= (char)R_SH << (i == 1 ? 4 : 0);
        p.program[++p.programSize] = num;

      } else {
        //Assume it's a symbol.
        p.program[p.programSize] |= (char)sym_get_or_create(p, l[i]);
      }
    }
  }
}

//Opens a file and parses and compiles it to bytecodes
ProgramSource dvm_compile(const char* filename) {
  ProgramSource src;
  src.programSize = 0;
  
  FILE *f = fopen(filename, "r");
  if (!f) {
    return src;
  }

  Program prog;
  
  
  char c;
  bool inString = false;

  std::vector<std::string> line;
  std::string token;

  while(fread(&c, 1, 1, f)) {
    if (c == '\n' || c == ';') {
      if (token.size() > 0) {
        line.push_back(token);
      }
      if (line.size() > 0) {
        parse_line(prog, line);
      }
      token = "";
      line.clear();
      if (c == ';') {
        while(c != '\n') {
          fread(&c, 1, 1, f);
        }
      }
    } else if (c == '"') {  
      inString = !inString;
      token += c;
    } else if (((c == ' ' && !inString) || c == '\t' || c == ',') && token.size() > 0) {
      line.push_back(token);
      token = "";
    } else if ((c != ' ' || inString) && c != '\t') {
      token += c;
    }
  }

  if (token.size() > 0) {
    line.push_back(token);
  }
  if (line.size() > 0) {
    parse_line(prog, line);
  }

  printf("Compilation done. Result is %i bytes.\n", sizeof(short) * prog.programSize);
  printf("int program[] = {\n");
  for (int i = 0; i <= prog.programSize; i++) {
    if (i > 0) printf(",");
    printf("\n");
    printf("  0x%X", prog.program[i]);
  }
  printf("\n}\n");

  fclose(f);

  memcpy(&src.program, &prog.program, sizeof(short) * (prog.programSize + 1));

  src.programSize = prog.programSize + 1;

  return src;
}