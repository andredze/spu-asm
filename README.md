# SPU + ASM
Soft processor unit and assembler

## Build compiler

```powershell
  make asm
```

## Build program

```powershell
  make spu
```

### Build with debug mode

Build compiler
```powershell
  make asm DEBUG=1
```

Build program
```powershell
  make spu DEBUG=1
```

## Compiling
```powershell
  compile.exe <your_code.asm> <bitecode_name.bin>
```
### For assembler listing
```powershell
  compile.exe <your_code.asm> <bitecode_name.bin> LIST=1
```

## Usage
Input file example
```text
  PUSH 1
  POPR RAX
  PUSHR RAX
  PUSHR RAX
  MUL
  OUT
  PUSHR RAX
  PUSH 1
  ADD
  POPR RAX
  PUSHR RAX
  PUSH 5
  JB 4
  HLT
```

Execute program
```powershell
  run.exe <bytecode_name.bin>
```

Output of the example
```text
  ANSWER = 1
  ANSWER = 4
  ANSWER = 9
  ANSWER = 16
```

## Assembler commands
Comment
```
# <your_commentary>
```
End program
```
  HLT
```
Push to stack
```
  PUSH 1
```
Commands using console
```
  OUT
  IN
```
Labels
```
:10
```
Functions
```
  CALL :5
  RET
```
Math operations
```
  ADD
  SUB
  MUL
  DIV
  MOD
  SQRT
  SQR
```
Jumps
```
  JMP :19
  JB :19
  JBE :19
  JA :19
  JAE :19
  JE :19
  JNE :19
```
Commands using registers
```
  PUSHR RAX
  POPR RAX
```
Commands using ram
```
  PUSHM [RAX]
  POPM [RAX]
```
Draw in window
```
  DRAW 1000
```
