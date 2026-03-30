#include "Scanner.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Parser.h"
#include "Interpreter.h"

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

int main(int argc, char* argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;


    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
        return 0;
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

    else if(command == "parse"){
        Parser parser(tokens);
        std::unique_ptr<Expr> expr = parser.parse();
        if(expr){
            std::cout << expr->print() << std::endl;
        }
        else{
            std::cerr << "error" << std::endl;
            return_code = 70;
        }
    }
    else if (command == "evaluate") {
        Parser parser(tokens);
        try{
            std::unique_ptr<Expr> expr = parser.parse();
            if (expr) {
                Interpreter interpreter;
                interpreter.evaluate(expr.get());
            }
            else {
                return_code = 65;
            }
        }
        catch(const std::runtime_error& error){
            std::cerr << "Runtime Error: " << error.what() << std::endl;
        }
        
    }
    else if(command == "run"){
        Parser parser(tokens);
        Interpreter interpreter;
        try{
            std::vector<std::unique_ptr<Stmt>> statements = parser.run();
            interpreter.interpret(statements);
        }
        catch(const std::runtime_error& e){
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