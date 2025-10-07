#include "commands.h"

int CmdArgsCount(Command_t command)
{
    if (command == CMD_PUSH)
        return 1;
    if (command == CMD_POPR)
        return 1;
    if (command == CMD_PUSHR)
        return 1;

    return 0;
}
