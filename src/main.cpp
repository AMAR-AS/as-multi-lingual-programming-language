// ================================================
// main.cpp — Entry point for AskLang (.as)
// ================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

// Core pipeline includes
#include "core/lexer.h"
#include "core/parser.h"
#include "core/semantic.h"
#include "core/ast.h"
#include "core/ir.h"
#include "core/optimizer.h"
#include "core/sandbox.h"
#include "core/codegen.h"
#include "core/vm.h"

// Runtime
#include "runtime/runtime.h"

// --------------------------------
// Helper: read entire file into string
// --------------------------------
std::string readFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// --------------------------------
// Global debug flag
// --------------------------------
bool DEBUG_MODE = false;
#define DEBUG(msg) if (DEBUG_MODE) { std::cout << msg << std::endl; }

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ask <file.as> [args...] [--debug]\n";
        return 1;
    }

    try {
        std::string filename;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--debug") {
                DEBUG_MODE = true;
            } else {
                filename = argv[i];
            }
        }

        if (filename.empty()) {
            throw std::runtime_error("No source file provided.");
        }

        std::string source = readFile(filename);

        // --------------------------------
        // 1. Lexical Analysis
        // --------------------------------
        DEBUG("[Lexer] Tokenizing source...");
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        DEBUG("[Lexer] Tokens = " << tokens.size());

        // --------------------------------
        // 2. Parsing -> AST
        // --------------------------------
        DEBUG("[Parser] Building AST...");
        Parser parser(tokens);
        auto ast = parser.parse();
        DEBUG("[Parser] AST built successfully.");

        // --------------------------------
        // 3. Semantic Analysis
        // --------------------------------
        DEBUG("[Semantic] Verifying...");
        SemanticAnalyzer sema;
        sema.verify(ast);
        DEBUG("[Semantic] Passed.");

        // --------------------------------
        // 4. IR Generation
        // --------------------------------
        DEBUG("[IR] Generating...");
        IRGenerator irgen;
        auto ir = irgen.generate(ast);
        DEBUG("[IR] Generated IR nodes = " << ir.size());

        // --------------------------------
        // 5. Optimization
        // --------------------------------
        DEBUG("[Optimizer] Running...");
        Optimizer optimizer;
        ir = optimizer.optimize(ir);
        DEBUG("[Optimizer] Optimization complete.");

        // --------------------------------
        // 6. Sandbox Execution Check
        // --------------------------------
        DEBUG("[Sandbox] Validating IR...");
        Sandbox sandbox;
        if (!sandbox.validate(ir)) {
            std::cerr << "[Sandbox] Program failed validation — unsafe ops detected.\n";
            return 1;
        }
        DEBUG("[Sandbox] Safe.");

        // --------------------------------
        // 7. Code Generation (Bytecode)
        // --------------------------------
        DEBUG("[CodeGen] Generating bytecode...");
        CodeGen codegen;
        auto bytecode = codegen.generate(ir);
        DEBUG("[CodeGen] Bytecode size = " << bytecode.size());

        // --------------------------------
        // 8. VM + Runtime binding
        // --------------------------------
        DEBUG("[VM] Initializing...");
        VM vm;
        Runtime::bindNatives(vm);  // connect built-in stdlib functions
        vm.load(bytecode);

        // --------------------------------
        // 9. Execute
        // --------------------------------
        DEBUG("[VM] Running program...");
        vm.run();
        DEBUG("[VM] Execution complete.");

    } catch (const std::exception& ex) {
        std::cerr << "[AskLang Error] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
