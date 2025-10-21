g++ ^
source/spu/main.cpp ^
source/spu/stack.cpp ^
source/spu/handle_operations.cpp ^
source/spu/processor.cpp ^
source/input.cpp ^
source/cmd_cases.cpp ^
-Iinclude -Iinclude/spu -DSPU ^
-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef ^
-Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations ^
-Wmissing-include-dirs -Wswitch-default -Weffc++ ^
-Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion ^
-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 ^
-Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers ^
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo ^
-Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits ^
-Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE ^
-o run.exe
