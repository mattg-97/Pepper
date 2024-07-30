# Pepper
A statically typed programming language, mainly for learning purposes

## Compiler Pipeline
1. Tokenize Source Code
2. Parse Tokens into AST
3. Semantic Analysis and Type Checking
4. Generate Intermediate Representation (IR)
5. Optimization Passes on IR
6. Generate Target-specific Code (Assembly for compilation, Bytecode for vm/repl)
7. Assembly and Linking

### 1. Tokenize Source Code
Robust lexical analysis with error handling and comment skipping.
- Error Handling: Include mechanisms for reporting and recovering from lexical errors.
- Comment Handling: Ensure comments are ignored or handled appropriately.

### 2. Parse Tokens into AST
Syntax analysis with detailed error reporting.
- Error Reporting and Recovery: Implement robust error reporting and recovery mechanisms to handle syntax errors gracefully.
- Abstract Syntax Tree (AST): Consider building a more detailed AST that can represent all syntactic constructs of your language.

### 3. Semantic Analysis and Type Checking
Ensure semantic correctness and enforce type rules.
- Symbol Table Management: Maintain symbol tables to handle scope and binding of identifiers.
- Type Checking: Perform type checks and enforce type rules.
- Semantic Checks: Include additional checks like constant expressions, unreachable code, etc.

### 4. Generate Intermediate Representation (IR)
Create a flexible IR suitable for optimizations.
- IR Design: Choose a flexible and powerful IR (like Static Single Assignment (SSA) form) that simplifies optimization.
- Instrumentation: Add debugging information and instrumentation if needed.

### 5. Optimization Passes on IR
Apply a series of optimization passes to improve the IR.
- Control Flow Analysis: Implement more sophisticated control flow and data flow analyses.
- Advanced Optimizations: Add more advanced optimization passes like loop unrolling, inlining, and interprocedural optimizations.

### 6. Generate Target-specific Code (Assembly or Bytecode)
Convert optimized IR to efficient assembly or bytecode.
- Target Abstraction: Abstract the target machine specifics to support multiple backends.
- Instruction Selection: Implement sophisticated instruction selection algorithms.
- Register Allocation: Use advanced register allocation techniques (e.g., graph coloring).

### 7. Assembly and Linking
Final assembly and linking steps to produce an executable.
- Assembly Generation: Generate efficient assembly code with appropriate directives and optimizations.
- Linking: Link with standard libraries and handle external dependencies.
