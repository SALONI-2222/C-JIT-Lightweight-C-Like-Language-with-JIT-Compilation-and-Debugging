# C-JIT-Lightweight-C-Like-Language-with-JIT-Compilation-and-Debugging

Our project aims to build a lightweight compiler for a simplified C-like programming language. The compiler includes a lexer, parser, abstract syntax tree (AST) generator, intermediate representation (IR) generation, and a Just-In-Time (JIT) execution backend. The system also supports basic optimizations and includes debugging features like IR printing. This tool is designed for educational purposes and prototyping, enabling real-time analysis and transformation of simple C-like programs. The ultimate goal is to provide a functional compilation pipeline that can execute source programs directly, while allowing room for further enhancements like function support, memory inspection, and interactive debugging.



Lexer: Tokenizes input source code (from lexer.c/h).  
Parser: Builds an AST from tokens using a recursive-descent parser (parser.c/h).  
IR Generator: Converts AST to a 3-address code-style intermediate representation (ir.c/h).  
Optimizer: Performs constant folding and IR-level simplifications (optimizer.c/h). 
