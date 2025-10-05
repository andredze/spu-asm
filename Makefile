CXX = g++

CXXFLAGS =  -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
			-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
			-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
			-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion \
			-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers \
			-Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual \
			-Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
			-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG \
			-D_EJUDGE_CLIENT_SIDE

CALCFLAGS = $(CXXFLAGS) -Iinclude

CALCSOURCES = source/main.cpp \
			  source/stack.cpp \
			  source/maths.cpp \
			  source/input.cpp \
		  	  source/calculator.cpp \
			  source/processor.cpp

CALCEXECUTABLE = run.exe

CALCOUTPUTS = answers.txt \
			  stack.log

ASMSOURCES = compile/main.cpp \
			 compile/assembler.cpp \
			 source/input.cpp

ASMEXECUTABLE = compile.exe

ASMFLAGS = $(CXXFLAGS) -Icompile -Iinclude

ASMOUTPUTS = bitecode.bin pretty_bc.txt

ifdef DEBUG
ASMFLAGS += -DDEBUG
CALCFLAGS += -DPROC_DEBUG -DSTACK_DEBUG -DDEBUG
endif

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

compiler:
	$(CXX) $(ASMFLAGS) $(ASMSOURCES) -o $(ASMEXECUTABLE)

calculator:
	$(CXX) $(CALCFLAGS) $(CALCSOURCES) -o $(CALCEXECUTABLE)

clean:
	rm $(CALCOUTPUTS) $(ASMOUTPUTS) $(CALCEXECUTABLE) $(ASMEXECUTABLE)
