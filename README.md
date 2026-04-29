# C-compiler-Scratch

This project is a lightweight C compiler built from scratch in C, designed to transform custom source code into executable assembly. It features a hand-written Lexer and a Recursive Descent Parser to build a structured Abstract Syntax Tree (AST). For robust memory management and minimalist data structures, I’ve integrated Alexey Kutepov’s (Tsoding) ds.h library. The backend generates ELF64 assembly with efficient stack-based variable allocation, showcasing the core principles of compiler design and low-level system programming.

This project is specifically targeted for Linux x86_64 systems, generating ELF64 assembly. It was built as a learning project inspired by the "Alex the Dev" YouTube channel, following his compiler series to understand the end-to-end process of language implementation.

YouTube link: https://www.youtube.com/watch?v=HOe2YFnzO2I

