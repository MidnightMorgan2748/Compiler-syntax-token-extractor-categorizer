#include "Tokenizer.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <unordered_set>

Tokenizer::Tokenizer() {}

bool Tokenizer::isOperatorChar(char c) const {
    static const std::string opChars = "+-*/%=!<>|&^~?:.";
    return opChars.find(c) != std::string::npos;
}

bool Tokenizer::isPunctuationChar(char c) const {
    static const std::string punctChars = ";,(){}[]";
    return punctChars.find(c) != std::string::npos;
}

std::string Tokenizer::readFileContents(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open source file: " + filePath);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string> Tokenizer::tokenizeToVector(const std::string& sourceCode, 
                                                    bool includeCommentsAndDirectives) {
    std::vector<Token> detailedTokens = tokenizeDetailed(sourceCode, includeCommentsAndDirectives);
    std::vector<std::string> rawTokens;
    rawTokens.reserve(detailedTokens.size());
    for (const auto& tok : detailedTokens) {
        rawTokens.push_back(tok.lexeme);
    }
    return rawTokens;
}

std::vector<Token> Tokenizer::tokenizeDetailed(const std::string& sourceCode, 
                                              bool includeCommentsAndDirectives) {
    std::vector<Token> tokens;
    const size_t len = sourceCode.length();
    size_t i = 0;
    size_t currentLine = 1;
    size_t lineStartPos = 0;

    auto getCol = [&](size_t pos) -> size_t {
        return (pos >= lineStartPos) ? (pos - lineStartPos + 1) : 1;
    };

    while (i < len) {
        char c = sourceCode[i];

        // 1. Handle Whitespace & Newlines
        if (c == '\r') {
            i++;
            if (i < len && sourceCode[i] == '\n') {
                i++;
            }
            currentLine++;
            lineStartPos = i;
            continue;
        } else if (c == '\n') {
            i++;
            currentLine++;
            lineStartPos = i;
            continue;
        } else if (std::isspace(static_cast<unsigned char>(c))) {
            i++;
            continue;
        }

        // 2. Preprocessor Directives (#...)
        if (c == '#' && (getCol(i) == 1 || [&]() {
            // Check if only whitespace before # on current line
            for (size_t k = lineStartPos; k < i; ++k) {
                if (!std::isspace(static_cast<unsigned char>(sourceCode[k]))) return false;
            }
            return true;
        }())) {
            size_t startCol = getCol(i);
            size_t startIdx = i;
            // Read until end of line (handling \ line continuations)
            while (i < len) {
                if (sourceCode[i] == '\\' && i + 1 < len && (sourceCode[i + 1] == '\n' || sourceCode[i + 1] == '\r')) {
                    if (sourceCode[i + 1] == '\r' && i + 2 < len && sourceCode[i + 2] == '\n') {
                        i += 3;
                    } else {
                        i += 2;
                    }
                    currentLine++;
                    lineStartPos = i;
                } else if (sourceCode[i] == '\n' || sourceCode[i] == '\r') {
                    break;
                } else {
                    i++;
                }
            }
            std::string directive = sourceCode.substr(startIdx, i - startIdx);
            if (includeCommentsAndDirectives) {
                tokens.emplace_back(directive, TokenType::PREPROCESSOR, currentLine, startCol);
            }
            continue;
        }

        // 3. Comments (// or /* ... */)
        if (c == '/' && i + 1 < len) {
            if (sourceCode[i + 1] == '/') {
                size_t startCol = getCol(i);
                size_t startIdx = i;
                while (i < len && sourceCode[i] != '\n' && sourceCode[i] != '\r') {
                    i++;
                }
                std::string comment = sourceCode.substr(startIdx, i - startIdx);
                if (includeCommentsAndDirectives) {
                    tokens.emplace_back(comment, TokenType::COMMENT, currentLine, startCol);
                }
                continue;
            } else if (sourceCode[i + 1] == '*') {
                size_t startCol = getCol(i);
                size_t startIdx = i;
                size_t commentStartLine = currentLine;
                i += 2;
                while (i + 1 < len && !(sourceCode[i] == '*' && sourceCode[i + 1] == '/')) {
                    if (sourceCode[i] == '\n') {
                        currentLine++;
                        lineStartPos = i + 1;
                    }
                    i++;
                }
                if (i + 1 < len) {
                    i += 2; // skip */
                } else {
                    i = len;
                }
                std::string comment = sourceCode.substr(startIdx, i - startIdx);
                if (includeCommentsAndDirectives) {
                    tokens.emplace_back(comment, TokenType::COMMENT, commentStartLine, startCol);
                }
                continue;
            }
        }

        // 4. String Literals ("...")
        if (c == '"') {
            size_t startCol = getCol(i);
            size_t startIdx = i++;
            while (i < len && sourceCode[i] != '"') {
                if (sourceCode[i] == '\\' && i + 1 < len) {
                    i += 2; // skip escape character
                } else {
                    if (sourceCode[i] == '\n') {
                        currentLine++;
                        lineStartPos = i + 1;
                    }
                    i++;
                }
            }
            if (i < len && sourceCode[i] == '"') {
                i++; // closing quote
            }
            std::string strLit = sourceCode.substr(startIdx, i - startIdx);
            tokens.emplace_back(strLit, TokenType::STRING_LITERAL, currentLine, startCol);
            continue;
        }

        // 5. Character Literals ('.')
        if (c == '\'') {
            size_t startCol = getCol(i);
            size_t startIdx = i++;
            while (i < len && sourceCode[i] != '\'') {
                if (sourceCode[i] == '\\' && i + 1 < len) {
                    i += 2;
                } else {
                    i++;
                }
            }
            if (i < len && sourceCode[i] == '\'') {
                i++;
            }
            std::string charLit = sourceCode.substr(startIdx, i - startIdx);
            tokens.emplace_back(charLit, TokenType::CHAR_LITERAL, currentLine, startCol);
            continue;
        }

        // 6. Identifiers and Keywords
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t startCol = getCol(i);
            size_t startIdx = i++;
            while (i < len && (std::isalnum(static_cast<unsigned char>(sourceCode[i])) || sourceCode[i] == '_')) {
                i++;
            }
            std::string ident = sourceCode.substr(startIdx, i - startIdx);
            if (ident == "true" || ident == "false") {
                tokens.emplace_back(ident, TokenType::BOOLEAN_LITERAL, currentLine, startCol);
            } else {
                // We initially mark identifiers; KeywordExtractor will accurately classify keywords
                // or we can detect known keywords here.
                tokens.emplace_back(ident, TokenType::IDENTIFIER, currentLine, startCol);
            }
            continue;
        }

        // 7. Numbers (Integers & Floating-point)
        if (std::isdigit(static_cast<unsigned char>(c)) || (c == '.' && i + 1 < len && std::isdigit(static_cast<unsigned char>(sourceCode[i + 1])))) {
            size_t startCol = getCol(i);
            size_t startIdx = i;
            bool isFloat = (c == '.');

            if (c == '0' && i + 1 < len && (sourceCode[i + 1] == 'x' || sourceCode[i + 1] == 'X')) {
                // Hexadecimal
                i += 2;
                while (i < len && (std::isxdigit(static_cast<unsigned char>(sourceCode[i])) || sourceCode[i] == '\'')) {
                    i++;
                }
            } else if (c == '0' && i + 1 < len && (sourceCode[i + 1] == 'b' || sourceCode[i + 1] == 'B')) {
                // Binary
                i += 2;
                while (i < len && (sourceCode[i] == '0' || sourceCode[i] == '1' || sourceCode[i] == '\'')) {
                    i++;
                }
            } else {
                // Decimal or float
                while (i < len && (std::isdigit(static_cast<unsigned char>(sourceCode[i])) || sourceCode[i] == '\'')) {
                    i++;
                }
                if (i < len && sourceCode[i] == '.') {
                    isFloat = true;
                    i++;
                    while (i < len && (std::isdigit(static_cast<unsigned char>(sourceCode[i])) || sourceCode[i] == '\'')) {
                        i++;
                    }
                }
                // Exponent part (e or E)
                if (i < len && (sourceCode[i] == 'e' || sourceCode[i] == 'E')) {
                    isFloat = true;
                    i++;
                    if (i < len && (sourceCode[i] == '+' || sourceCode[i] == '-')) {
                        i++;
                    }
                    while (i < len && std::isdigit(static_cast<unsigned char>(sourceCode[i]))) {
                        i++;
                    }
                }
            }
            // Suffixes: f, F, l, L, u, U, ll, ull
            while (i < len && (std::isalpha(static_cast<unsigned char>(sourceCode[i])))) {
                i++;
            }

            std::string numLex = sourceCode.substr(startIdx, i - startIdx);
            tokens.emplace_back(numLex, isFloat ? TokenType::FLOATING_LITERAL : TokenType::INTEGER_LITERAL, currentLine, startCol);
            continue;
        }

        // 8. Multi-character Operators and Punctuation
        size_t startCol = getCol(i);
        // Check for 3-character operators
        if (i + 2 < len) {
            std::string op3 = sourceCode.substr(i, 3);
            if (op3 == "..." || op3 == "<<=" || op3 == ">>=" || op3 == "<=>" || op3 == "->*") {
                tokens.emplace_back(op3, TokenType::OPERATOR, currentLine, startCol);
                i += 3;
                continue;
            }
        }
        // Check for 2-character operators
        if (i + 1 < len) {
            std::string op2 = sourceCode.substr(i, 2);
            static const std::unordered_set<std::string> twoCharOps = {
                "==", "!=", "<=", ">=", "&&", "||", "++", "--", "::", "->",
                "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<", ">>"
            };
            if (twoCharOps.find(op2) != twoCharOps.end()) {
                tokens.emplace_back(op2, TokenType::OPERATOR, currentLine, startCol);
                i += 2;
                continue;
            }
        }

        // 9. Single-character Punctuation
        if (isPunctuationChar(c)) {
            tokens.emplace_back(std::string(1, c), TokenType::PUNCTUATION, currentLine, startCol);
            i++;
            continue;
        }

        // 10. Single-character Operator
        if (isOperatorChar(c)) {
            tokens.emplace_back(std::string(1, c), TokenType::OPERATOR, currentLine, startCol);
            i++;
            continue;
        }

        // 11. Unknown / Fallback
        tokens.emplace_back(std::string(1, c), TokenType::UNKNOWN, currentLine, startCol);
        i++;
    }

    return tokens;
}

std::vector<std::string> Tokenizer::tokenizeFileToVector(const std::string& filePath,
                                                        bool includeCommentsAndDirectives) {
    std::string sourceCode = readFileContents(filePath);
    return tokenizeToVector(sourceCode, includeCommentsAndDirectives);
}

std::vector<Token> Tokenizer::tokenizeFileDetailed(const std::string& filePath,
                                                  bool includeCommentsAndDirectives) {
    std::string sourceCode = readFileContents(filePath);
    return tokenizeDetailed(sourceCode, includeCommentsAndDirectives);
}
