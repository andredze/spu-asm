#include "commands.h"

int CmdArgsCount(Command_t command)
{
    // switch
    if (command == CMD_PUSH)
        return 1;
    if (command == CMD_POPR)
        return 1;
    if (command == CMD_PUSHR)
        return 1;
    if (command == CMD_JMP)
        return 1;
    if (command == CMD_JB)
        return 1;
    if (command == CMD_JBE)
        return 1;
    if (command == CMD_JA)
        return 1;
    if (command == CMD_JAE)
        return 1;
    if (command == CMD_JE)
        return 1;
    if (command == CMD_JNE)
        return 1;

    return 0;
}
