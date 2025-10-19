#include "cmd_cases.h"

CmdCase_t CMD_CASES[] = {
    SET_CMD_CASE (HLT,   1),
    SET_CMD_CASE (PUSH,  2),
    SET_CMD_CASE (OUT,   1),
    SET_CMD_CASE (IN,    1),
    SET_CMD_CASE (CALL,  2),
    SET_CMD_CASE (RET,   1),
    SET_CMD_CASE (ADD,   1),
    SET_CMD_CASE (SUB,   1),
    SET_CMD_CASE (MUL,   1),
    SET_CMD_CASE (DIV,   1),
    SET_CMD_CASE (MOD,   1),
    SET_CMD_CASE (SQRT,  1),
    SET_CMD_CASE (SQR,   1),
    SET_CMD_CASE (JMP,   2),
    SET_CMD_CASE (JB,    2),
    SET_CMD_CASE (JBE,   2),
    SET_CMD_CASE (JA,    2),
    SET_CMD_CASE (JAE,   2),
    SET_CMD_CASE (JE,    2),
    SET_CMD_CASE (JNE,   2),
    SET_CMD_CASE (PUSHR, 2),
    SET_CMD_CASE (POPR,  2),
    SET_CMD_CASE (PUSHM, 2),
    SET_CMD_CASE (POPM,  2),
    SET_CMD_CASE (DRAW,  2),
    SET_CMD_CASE (WDRAW, 2),
};

const size_t CMD_CASES_SIZE = sizeof(CMD_CASES) / sizeof(CMD_CASES[0]);
