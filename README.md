# RPN-calculator
Calculator using reverse Polish notation

## Build compiler

```powershell
  make compiler
```

## Build programm

```powershell
  make spu
```

### Build with debug mode

Build compiler
```powershell
  make compiler DEBUG=1
```

Build programm
```powershell
  make spu DEBUG=1
```

## Compiling
```powershell
  compile.exe
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
  run.exe
```

Output of the example
```text
  ANSWER = 1
  ANSWER = 4
  ANSWER = 9
  ANSWER = 16
```
