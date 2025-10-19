CXX = g++

CXXFLAGS =  -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
			-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
			-Wmissing-include-dirs -Wswitch-default -Weffc++ \
			-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion \
			-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers \
			-Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual \
			-Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
			-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG \
			-D_EJUDGE_CLIENT_SIDE

PROCFLAGS = $(CXXFLAGS) -Iinclude -Iinclude/spu

PROCSOURCES = source/spu/main.cpp \
			  source/spu/stack.cpp \
			  source/spu/handle_operations.cpp \
			  source/spu/processor.cpp \
			  source/input.cpp \
			  source/cmd_cases.cpp

PROCEXECUTABLE = run.exe

PROCOUTPUTS = answers.txt \
			  processor.log \
			  stack.log

ASMSOURCES = source/asm/main.cpp \
			 source/asm/assembler.cpp \
			 source/asm/listing.cpp \
			 source/asm/add_operations.cpp \
			 source/input.cpp \
			 source/cmd_cases.cpp

ASMEXECUTABLE = compile.exe

ASMFLAGS = $(CXXFLAGS) -Iinclude -Iinclude/asm

ASMOUTPUTS = bytecode.bin \
			 pretty_bc.txt

ifdef DEBUG
ASMFLAGS += -DASM_DEBUG
PROCFLAGS += -DPROC_DEBUG -DSTACK_DEBUG -DDEBUG
endif

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

asm:
	$(CXX) -DASM $(ASMFLAGS) $(ASMSOURCES) -o $(ASMEXECUTABLE)

spu:
	$(CXX) -DSPU $(PROCFLAGS) $(PROCSOURCES) -o $(PROCEXECUTABLE)

clean:
	rm $(PROCOUTPUTS) $(ASMOUTPUTS) $(PROCEXECUTABLE) $(ASMEXECUTABLE)
