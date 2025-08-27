// =========================================
// AskLang VM Surface API
// =========================================

// The VM provides direct execution of AST programs.
// These builtins are available when running with `--run`.

// Print a value to stdout
builtin print(val)

// Input a line from stdin (returns txt)
builtin input(prompt)

// Convert values
builtin int(val)
builtin dec(val)
builtin txt(val)

// Math helpers
builtin sqrt(num)
builtin pow(base, exp)

// String helpers
builtin len(str)
builtin split(str, sep)
builtin join(arr, sep)

// Array creation
builtin array(size, initVal)

// Time helpers
builtin now_ms()

// =========================================
// Notes for developers:
// - These functions are implemented in C++ (Runtime + VM).
// - When a program uses them, the VM maps to native functions.
// - User can also define new functions in .as, which the VM executes.
// =========================================
