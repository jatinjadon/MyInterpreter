#include "Scanner.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Parser.h"
#include "Interpreter.h"
#include "Resolver.h"

std::string read_file_contents(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

// Interactive REPL Mode implementation
void runPrompt() {
    std::string line;
    Interpreter interpreter;
    interpreter.isREPLmode = true;

    std::cout << "Welcome to MyInterpreter REPL ✨" << std::endl;
    std::cout << "Type 'exit' or press Ctrl+D / Ctrl+Z to quit.\n" << std::endl;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            break; // EOF reached
        }

        if (line == "exit" || line == "quit") {
            break;
        }

        if (line.empty()) {
            continue;
        }
        if (!line.empty() && line.back() != ';' && line.back() != '}') {
            line += ';';
        }
        int scan_return_code = 0;
        Scanner scanner(line, scan_return_code);
        std::vector<Token> tokens = scanner.scanTokens();

        Parser parser(tokens);
        try {
            std::vector<std::unique_ptr<Stmt>> statements = parser.run();

            if (parser.hadError) {
                continue;
            }

            Resolver resolver(&interpreter);
            resolver.resolve(statements);
            if (resolver.hadError) {
                continue;
            }

            interpreter.interpret(statements);
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Runtime Error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // 1. REPL Mode: If no arguments are passed, enter interactive prompt
    if (argc == 1) {
        runPrompt();
        return 0;
    }

    // 2. Existing Command Execution Modes (tokenize, parse, evaluate, run)
    if (argc < 3) {
        std::cerr << "Usage: ./your_program [command] <filename>" << std::endl;
        std::cerr << "Commands: tokenize | parse | evaluate | run" << std::endl;
        std::cerr << "To run REPL: Run without any arguments." << std::endl;
        return 64;
    }

    std::string file_contents = read_file_contents(argv[2]);
    const int length = file_contents.size();

    const std::string command = argv[1];
    int return_code = 0;

    Scanner scanner(file_contents, return_code);
    std::vector<Token> tokens = scanner.scanTokens();

    if (command == "tokenize") {
        for (const Token& token : tokens) {
            std::cout << token.toString() << std::endl;
        }
    }

    else if (command == "parse") {
        Parser parser(tokens);
        std::unique_ptr<Expr> expr = parser.parse();
        if (expr) {
            std::cout << expr->print() << std::endl;
        }
        else {
            std::cerr << "error" << std::endl;
            return_code = 70;
        }
    }

    else if (command == "evaluate") {
        Parser parser(tokens);
        try {
            std::unique_ptr<Expr> expr = parser.parse();
            if (parser.hadError) {
                return_code = 65;
                exit(return_code);
            }
            if (expr) {
                Interpreter interpreter;
                interpreter.evaluate(expr.get());
            }
            else {
                return_code = 65;
            }
        }
        catch (const std::runtime_error& error) {
            std::cerr << "Runtime Error: " << error.what() << std::endl;
        }
    }

    else if (command == "run") {
        Parser parser(tokens);
        Interpreter interpreter;
        try {
            std::vector<std::unique_ptr<Stmt>> statements = parser.run();

            if (parser.hadError) {
                return_code = 65;
                exit(return_code);
            }
            Resolver resolver(&interpreter);
            resolver.resolve(statements);
            if (resolver.hadError) {
                return_code = 65;
                exit(return_code);
            }
            interpreter.interpret(statements);
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Runtime Error: " << e.what() << std::endl;
            exit(70);
        }
    }

    else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 0;
    }

    return return_code;
}