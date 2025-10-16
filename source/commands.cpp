#include "commands.h"

// TODO: args count in comm_cases
// в массив с командами вписывать колво аргументов, потом циклом проходить по нему и смотреть аргументы; либо пронумеровать команды так, чтобы они совпадали с индексами массива
int CmdArgsCount(Command_t command)
{
    switch (command)
    {
        case CMD_PUSH:  return 1;
        case CMD_POPR:  return 1;
        case CMD_PUSHR: return 1;
        case CMD_JMP:   return 1;
        case CMD_JB:    return 1;
        case CMD_JBE:   return 1;
        case CMD_JA:    return 1;
        case CMD_JAE:   return 1;
        case CMD_JE:    return 1;
        case CMD_JNE:   return 1;
        case CMD_CALL:  return 1;
        case CMD_PUSHM: return 1;
        case CMD_POPM:  return 1;
        case CMD_DRAW:  return 1;
        default:        return 0;
    }

    return 0;
}
