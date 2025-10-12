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

PROCFLAGS = $(CXXFLAGS) -Iinclude

PROCSOURCES = processor/main.cpp \
			  processor/stack.cpp \
			  processor/operations.cpp \
			  processor/input.cpp \
			  processor/commands.cpp \
			  processor/processor.cpp

PROCEXECUTABLE = run.exe

PROCOUTPUTS = answers.txt \
			  processor.log \
			  stack.log

ASMSOURCES = compile/main.cpp \
			 compile/assembler.cpp \
			 processor/input.cpp \
			 processor/commands.cpp \
			 compile/listing.cpp

ASMEXECUTABLE = compile.exe

ASMFLAGS = $(CXXFLAGS) -Icompile -Iinclude

ASMOUTPUTS = bitecode.bin \
			 pretty_bc.txt

ifdef DEBUG
ASMFLAGS += -DDEBUG
PROCFLAGS += -DPROC_DEBUG -DSTACK_DEBUG -DDEBUG
endif

ifdef LIST
ASMFLAGS += -DLISTING
endif

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

compiler:
	$(CXX) $(ASMFLAGS) $(ASMSOURCES) -o $(ASMEXECUTABLE)

spu:
	$(CXX) $(PROCFLAGS) $(PROCSOURCES) -o $(PROCEXECUTABLE)

clean:
	rm $(PROCOUTPUTS) $(ASMOUTPUTS) $(PROCEXECUTABLE) $(ASMEXECUTABLE)
