CXX = g++

CXXFLAGS =  -Iinclude \
			-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef \
			-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
			-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ \
			-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion \
			-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers \
			-Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual \
			-Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing \
			-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG \
			-D_EJUDGE_CLIENT_SIDE

SOURCES = source/main.cpp \
		  source/stack.cpp \
		  source/maths.cpp \
		  source/input.cpp \
		  source/calculator.cpp \
		  source/assembler.cpp

EXECUTABLE = run.exe

OUTPUTS = stack.log

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

release:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

debug:
	$(CXX) -DDEBUG $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

canary:
	$(CXX) -DCANARY $(CXXFLAGS) $(SOURCES) -o $(EXECUTABLE)

clean:
	rm $(OUTPUTS) $(EXECUTABLE)

cleanlogs:
	rm $(OUTPUTS)
