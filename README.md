# MyInterpreter 🚀

![C++](https://img.shields.io/badge/C++-17%2F20-blue.svg?style=flat&logo=c%2B%2B)
![Build](https://img.shields.io/badge/Build-Visual_Studio-blueviolet.svg)

A high-performance, tree-walk interpreter for the **Lox** programming language, built entirely from scratch in C++.

Designed with a strict adherence to **modern C++ paradigms**, this project utilizes `std::variant` for runtime dynamic typing, smart pointers (`std::unique_ptr` and `std::shared_ptr`) for safe AST memory management, and classic object-oriented polymorphism for implementing the Visitor pattern. It serves as an excellent foundation for exploring compiler design, lexical analysis, recursive descent parsing, and semantic resolution.

## ✨ Technical Highlights

* **Modular Pipeline Architecture:** Cleanly separates lexical scanning, parsing, semantic resolution, and runtime evaluation into granular, testable passes.
* **Modern C++ Memory Safety:** Eliminates memory leaks and manual `delete` calls by relying on unique and shared pointers throughout the Abstract Syntax Tree (`expr.h`, `stmt.h`).
* **Safe Dynamic Typing:** The `LoxValue` class leverages `<variant>` to safely and efficiently represent dynamic types (Numbers, Strings, Booleans, Callables) at runtime, completely avoiding unsafe `void*` or heavy base-class casting.
* **Polymorphic Visitor Pattern:** AST traversal (used heavily by both `Resolver` and `Interpreter`) relies on pure virtual functions, decoupling the tree structure from the specific execution or resolution logic being performed on it.
* **Static Scoping via Resolution:** Implements a full semantic analysis pass (`Resolver.cpp`) prior to execution to handle lexical block scoping and variable bindings for closures.
* **Exception-Based Control Flow:** Cleverly utilizes C++ exceptions (`ReturnException.h`) to gracefully unwind the call stack when `return` statements are triggered deep inside nested function calls.

## 📂 Project Structure

```text
├── MyInterpreter.slnx                # Visual Studio Solution File
├── MyInterpreter/
│   ├── include/                      # Header Files (.h)
│   │   ├── Environment.h             # Lexical environment mapping
│   │   ├── Interpreter.h             # Tree-walk execution engine
│   │   ├── LoxCallable.h             # Interface for functions and classes
│   │   ├── LoxFunction.h             # User-defined function implementation
│   │   ├── LoxValue.h                # Variant-based dynamic type system
│   │   ├── Parser.h                  # Recursive descent parser
│   │   ├── Resolver.h                # Semantic analysis and variable resolution
│   │   ├── Scanner.h                 # Lexical analyzer / Tokenizer
│   │   ├── Token.h                   # Source code token representation
│   │   ├── expr.h                    # AST Node definitions for Expressions
│   │   ├── stmt.h                    # AST Node definitions for Statements
│   │   └── ReturnException.h         # Control flow exception for returns
│   │
│   ├── src/                          # Implementation Files (.cpp)
│   │   ├── Environment.cpp           
│   │   ├── Interpreter.cpp           
│   │   ├── Parser.cpp                
│   │   ├── Resolver.cpp              
│   │   ├── Scanner.cpp               
│   │   └── main.cpp                  # CLI Entry point & REPL loop
│   │
│   └── tests/                        # Lox test scripts
│       ├── Parser.txt                
│       └── scanner.txt

🚀 Getting Started
Prerequisites
A modern C++ compiler supporting C++17 or higher (MSVC, GCC, or Clang).

Visual Studio (Recommended for Windows users).

Building the Project
Using Visual Studio:

Open MyInterpreter.slnx at the root of the repository.

Ensure the build target is set to Release for best performance.

Build the solution using Ctrl + Shift + B.

Using GCC/Clang (CLI):
Navigate into the MyInterpreter/ directory and compile the src/ files, ensuring you include the include/ directory in your path:

Bash
cd MyInterpreter
g++ -std=c++17 -I./include src/*.cpp -o lox_interpreter
💻 CLI Usage
The entry point (main.cpp) acts as a powerful testing and debugging harness, allowing you to stop the execution pipeline at various distinct phases.

The tool takes exactly two arguments: the command and the filename.

Bash
# 1. Lexical Analysis Only (Outputs recognized Tokens)
$ ./lox_interpreter tokenize script.lox

# 2. Parse Only (Outputs the stringified AST representation)
$ ./lox_interpreter parse script.lox

# 3. Evaluate Single Expression (Parses and evaluates without full statement execution)
$ ./lox_interpreter evaluate script.lox

# 4. Full Execution (Runs the Scanner, Parser, Resolver, and Interpreter)
$ ./lox_interpreter run script.lox
Running the executable without arguments will launch the interactive REPL mode.

📝 Supported Lox Features
Data Types: Integers/Doubles, Strings, Booleans, Nil.

Control Flow: if, else, while, for loops.

Functions: First-class functions, anonymous functions, and full closure support.

Lexical Scoping: Block scoping using { ... } managed gracefully by the semantic Resolver.

Example Script

// Demonstrating functions, recursion, and closures
fun makeCounter() {
  var i = 0;
  fun count() {
    i = i + 1;
    print i;
  }
  return count;
}

var counter = makeCounter();
counter(); // Prints 1
counter(); // Prints 2