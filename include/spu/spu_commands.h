#ifndef SPU_COMMANDS_H
#define SPU_COMMANDS_H

#include "operations.h"

int (* const SPU_HANDLE_OP_TABLE[]) (Proc_t*) = {
    NULL,
    HandlePush,
    HandleOut,
    HandleIn,
    HandleCall,
    HandleRet,
    HandleAdd,
    HandleSub,
    HandleMul,
    HandleDiv,
    HandleMod,
    HandleSqrt,
    HandleSqr,
    HandleJmp,
    HandleJb,
    HandleJbe,
    HandleJa,
    HandleJae,
    HandleJe,
    HandleJne,
    HandlePushr,
    HandlePopr,
    HandlePushm,
    HandlePopm,
    DrawInWindow
};

const size_t SPU_HANDLE_OP_TABLE_SIZE = sizeof(SPU_HANDLE_OP_TABLE) /
                                        sizeof(SPU_HANDLE_OP_TABLE[0]);

#endif /* SPU_COMMANDS_H */
