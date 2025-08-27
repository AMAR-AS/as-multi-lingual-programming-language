#include "lexer.h"
#include <cctype>
#include <unordered_set>
#include <unordered_map>

namespace ask {

// ---- Keyword & special literal tables (Ask -> categories) ----
static const std::unordered_set<std::string> KEYWORDS = {
    // control & structure
    "fcn","function","ret","giveback",
    "if","elif","orif","else","check",
    "for","let","while","when",
    "break","brk","continue","cont","skip","exit",
    "class","self","own","mine",
    "import","include","from","as",
    "try","hit","catch","except","finally","cleanup","raise","throw",
    "with","pass","donothing","leave","del","delete"
};

// boolean & none
static const std::unordered_map<std::string, TokenType> SPECIAL_LITERALS = {
    {"true",  TokenType::Bool},
    {"false", TokenType::Bool},
    {"yes",   TokenType::Bool},
    {"no",    TokenType::Bool},
    {"none",  TokenType::NoneLiteral},
    {"null",  TokenType::NoneLiteral},
};

// multi-char operators to detect greedily
static const std::unordered_set<std::string> TWO_CHAR_OPS = {
    "==","!=", "<=", ">=", "&&","||","->","=>","::","++","--",":=",
};

Lexer::Lexer(const std::string& source)
    : src(source), pos(0), line(1), col(1) {}

bool Lexer::atEnd() const { return pos >= src.size(); }

char Lexer::peek(int lookahead) const {
    size_t i = pos + static_cast<size_t>(lookahead);
    if (i >= src.size()) return '\0';
    return src[i];
}

char Lexer::advance() {
    if (atEnd()) return '\0';
    char c = src[pos++];
    if (c == '\n') { line++; col = 1; }
    else           { col++; }
    return c;
}

bool Lexer::match(char expected) {
    if (atEnd() || src[pos] != expected) return false;
    pos++;
    col++;
    return true;
}

bool Lexer::isIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
bool Lexer::isIdentContinue(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

void Lexer::skipCommentLine() {
    while (!atEnd() && peek() != '\n') advance();
}
void Lexer::skipCommentBlock() {
    // assumes we already consumed '/*'
    while (!atEnd()) {
        if (peek() == '*' && peek(1) == '/') { advance(); advance(); break; }
        advance();
    }
}

void Lexer::skipWhitespace() {
    for (;;) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
        // comments
        if (c == '#') { skipCommentLine(); continue; }
        if (c == '/' && peek(1) == '*') { advance(); advance(); skipCommentBlock(); continue; }
        break;
    }
}

Token Lexer::makeNumber() {
    int startCol = col;
    std::string lex;
    bool seenDot = false;

    while (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '_'
           || (!seenDot && peek() == '.')) {
        char c = advance();
        if (c == '.') seenDot = true;
        if (c != '_') lex.push_back(c); // ignore numeric separators
    }
    return {TokenType::Number, lex, line, startCol};
}

Token Lexer::makeString() {
    int startCol = col;
    char quote = advance(); // consume '"' or '\''
    std::string out;

    while (!atEnd() && peek() != quote) {
        char c = advance();
        if (c == '\\' && !atEnd()) { // basic escapes
            char e = advance();
            switch (e) {
                case 'n': out.push_back('\n'); break;
                case 't': out.push_back('\t'); break;
                case 'r': out.push_back('\r'); break;
                case '\\': out.push_back('\\'); break;
                case '\'': out.push_back('\''); break;
                case '\"': out.push_back('\"'); break;
                default: out.push_back(e); break;
            }
        } else {
            out.push_back(c);
        }
    }
    if (peek() == quote) advance(); // closing quote if present
    return {TokenType::String, out, line, startCol};
}

Token Lexer::makeIdentifierOrKeyword() {
    int startCol = col;
    std::string id;
    while (isIdentContinue(peek())) id.push_back(advance());

    // lower-case for keyword/special matching (Ask keywords are case-sensitive, but we assume lower)
    std::string lower = id;
    for (auto& ch : lower) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

    auto litIt = SPECIAL_LITERALS.find(lower);
    if (litIt != SPECIAL_LITERALS.end()) {
        return {litIt->second, lower, line, startCol};
    }
    if (KEYWORDS.count(lower)) {
        return {TokenType::Keyword, lower, line, startCol};
    }
    return {TokenType::Identifier, id, line, startCol};
}

Token Lexer::makeOperatorOrSymbol() {
    int startCol = col;
    char c0 = advance();
    std::string op(1, c0);

    // try two-char op
    char c1 = peek();
    if (c1 != '\0') {
        std::string two{c0, c1};
        if (TWO_CHAR_OPS.count(two)) {
            advance();
            return {TokenType::Operator, two, line, startCol};
        }
    }

    // classify single-char as operator or symbol
    switch (c0) {
        case '+': case '-': case '*': case '/': case '%':
        case '=': case '!': case '<': case '>':
        case '&': case '|': case '^': case '~': case '?':
        case ':': case '.':
            return {TokenType::Operator, op, line, startCol};
        case '(': case ')':
        case '{': case '}':
        case '[': case ']':
        case ',': case ';':
            return {TokenType::Symbol, op, line, startCol};
        default:
            return {TokenType::Error, op, line, startCol};
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> toks;
    while (!atEnd()) {
        skipWhitespace();

        if (atEnd()) break;

        char c = peek();
        if (std::isdigit(static_cast<unsigned char>(c))) {
            toks.push_back(makeNumber());
        } else if (c == '"' || c == '\'') {
            toks.push_back(makeString());
        } else if (isIdentStart(c)) {
            toks.push_back(makeIdentifierOrKeyword());
        } else if (c == '\n') {
            advance();
            toks.push_back({TokenType::Newline, "\\n", line, col});
        } else {
            toks.push_back(makeOperatorOrSymbol());
        }
    }
    toks.push_back({TokenType::EndOfFile, "", line, col});
    return toks;
}

} // namespace ask
