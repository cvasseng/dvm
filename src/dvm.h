#ifndef h__dvm__
#define h__dvm__

struct ProgramSource {
	short program[2048];
	int programSize;
};

extern void dvm_run(const short *prog, unsigned int size);
extern ProgramSource dvm_compile(const char* filename);

#endif
