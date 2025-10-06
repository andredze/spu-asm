# RPN-calculator
Calculator using reverse Polish notation

## Build compiler

```powershell
  make compiler
```

## Build programm

```powershell
  make processor
```

### Build with debug mode

Build compiler
```powershell
  make compiler DEBUG=1
```

Build programm
```powershell
  make processor DEBUG=1
```

## Compiling
```powershell
  compile.exe
```

## Usage
Input file example
```text
  PUSH 10
  PUSH 20
  MUL
  PUSH 5
  DIV
  OUT
  HLT
```

Execute programm
```powershell
  run.exe
```

Output file example
```text
  ANSWER = 40
```
