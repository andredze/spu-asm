#include "cmd_cases.h"

// SET_CMD_CASE(CMD_NAME, args_count, args_type)

CmdCase_t CMD_CASES[] = {
    SET_CMD_CASE (HLT,   1, No),
    SET_CMD_CASE (PUSH,  2, Normal),
    SET_CMD_CASE (OUT,   1, No),
    SET_CMD_CASE (IN,    1, No),
    SET_CMD_CASE (CALL,  2, Label),
    SET_CMD_CASE (RET,   1, No),
    SET_CMD_CASE (ADD,   1, No),
    SET_CMD_CASE (SUB,   1, No),
    SET_CMD_CASE (MUL,   1, No),
    SET_CMD_CASE (DIV,   1, No),
    SET_CMD_CASE (MOD,   1, No),
    SET_CMD_CASE (SQRT,  1, No),
    SET_CMD_CASE (SQR,   1, No),
    SET_CMD_CASE (JMP,   2, Label),
    SET_CMD_CASE (JB,    2, Label),
    SET_CMD_CASE (JBE,   2, Label),
    SET_CMD_CASE (JA,    2, Label),
    SET_CMD_CASE (JAE,   2, Label),
    SET_CMD_CASE (JE,    2, Label),
    SET_CMD_CASE (JNE,   2, Label),
    SET_CMD_CASE (PUSHR, 2, Reg),
    SET_CMD_CASE (POPR,  2, Reg),
    SET_CMD_CASE (PUSHM, 2, Ram),
    SET_CMD_CASE (POPM,  2, Ram),
    SET_CMD_CASE (DRAW,  2, Normal),
#ifdef GRAPHICS
    SET_CMD_CASE (WDRAW, 2, Normal)
#endif /* GRAPHICS */
};

const size_t CMD_CASES_SIZE = sizeof(CMD_CASES) / sizeof(CMD_CASES[0]);
