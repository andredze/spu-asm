# SPU + ASM
Soft processor unit and assembler

## Build compiler

```powershell
  make asm
```

## Build programm

```powershell
  make spu
```

### Build with debug mode

Build compiler
```powershell
  make asm DEBUG=1
```

Build programm
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

Execute programm
```powershell
  run <bytecode_name.bin>
```

Output of the example
```text
  ANSWER = 1
  ANSWER = 4
  ANSWER = 9
  ANSWER = 16
```
