#ifndef ASK_LEXER_H
#define ASK_LEXER_H

#include <string>
#include <vector>

namespace ask {

enum class TokenType {
    // basic
    Identifier,
    Keyword,
    Number,
    String,
    Bool,
    NoneLiteral,

    // operators & symbols
    Operator,
    Symbol,

    // layout
    Newline,

    // end
    EndOfFile,
    Error
};

struct Token {
    TokenType type;
    std::string lexeme;   // raw text (unescaped for strings)
    int line;
    int column;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);

    // Produce all tokens (Newline tokens included), ending with EndOfFile.
    std::vector<Token> tokenize();

private:
    // input
    const std::string src;
    size_t pos;
    int line;
    int col;

    // helpers
    char peek(int lookahead = 0) const;
    bool match(char expected);
    char advance();
    bool atEnd() const;

    // skipping
    void skipWhitespace();
    void skipCommentLine();     // # until end of line
    void skipCommentBlock();    // /* ... */

    // tokenizers
    Token makeNumber();
    Token makeString();
    Token makeIdentifierOrKeyword();
    Token makeOperatorOrSymbol();

    // utilities
    static bool isIdentStart(char c);
    static bool isIdentContinue(char c);
};

} // namespace ask

#endif // ASK_LEXER_H
