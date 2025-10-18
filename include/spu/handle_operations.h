#ifndef HANDLE_OPERATIONS_H
#define HANDLE_OPERATIONS_H

#include <TXLib.h>
#include <math.h>
#include <windows.h>
#include "stack.h"
#include "input.h"
#include "spu_types.h"

typedef enum HandleOpErr {
    HANDLE_OP_SUCCESS,
    HANDLE_OP_DIVISION_BY_ZERO,
    HANDLE_OP_STACK_ERROR,
    HANDLE_OP_INVALID_JUMP_ARG,
    HANDLE_OP_INVALID_REG_ARG,
    HANDLE_OP_INVALID_RAM_ARG,
    HANDLE_OP_INPUT_ERROR,
    HANDLE_OP_BREAK_LOOP
} HandleOpErr_t;

int Jump(Proc_t* proc_data, int new_cmd_count);

HandleOpErr_t HandleHLT   (Proc_t* proc_data);
HandleOpErr_t HandlePUSH  (Proc_t* proc_data);
HandleOpErr_t HandleOUT   (Proc_t* proc_data);
HandleOpErr_t HandleIN    (Proc_t* proc_data);
HandleOpErr_t HandleCALL  (Proc_t* proc_data);
HandleOpErr_t HandleRET   (Proc_t* proc_data);
HandleOpErr_t HandleADD   (Proc_t* proc_data);
HandleOpErr_t HandleSUB   (Proc_t* proc_data);
HandleOpErr_t HandleMUL   (Proc_t* proc_data);
HandleOpErr_t HandleDIV   (Proc_t* proc_data);
HandleOpErr_t HandleMOD   (Proc_t* proc_data);
HandleOpErr_t HandleSQRT  (Proc_t* proc_data);
HandleOpErr_t HandleSQR   (Proc_t* proc_data);
HandleOpErr_t HandleJMP   (Proc_t* proc_data);
HandleOpErr_t HandleJB    (Proc_t* proc_data);
HandleOpErr_t HandleJBE   (Proc_t* proc_data);
HandleOpErr_t HandleJA    (Proc_t* proc_data);
HandleOpErr_t HandleJAE   (Proc_t* proc_data);
HandleOpErr_t HandleJE    (Proc_t* proc_data);
HandleOpErr_t HandleJNE   (Proc_t* proc_data);
HandleOpErr_t HandlePUSHR (Proc_t* proc_data);
HandleOpErr_t HandlePOPR  (Proc_t* proc_data);
HandleOpErr_t HandlePUSHM (Proc_t* proc_data);
HandleOpErr_t HandlePOPM  (Proc_t* proc_data);
HandleOpErr_t HandleDRAW  (Proc_t* proc_data);
HandleOpErr_t DrawInWindow(Proc_t* proc_data);

#endif /* HANDLE_OPERATIONS_H */
