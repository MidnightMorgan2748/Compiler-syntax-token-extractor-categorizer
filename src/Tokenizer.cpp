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

        if (c == '#' && (getCol(i) == 1 || [&]() {
            for (size_t k = lineStartPos; k < i; ++k) {
                if (!std::isspace(static_cast<unsigned char>(sourceCode[k]))) return false;
            }
            return true;
        }())) {
            size_t startCol = getCol(i);
            size_t startIdx = i;
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
                    i += 2;
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

        if (c == '"') {
            size_t startCol = getCol(i);
            size_t startIdx = i++;
            while (i < len && sourceCode[i] != '"') {
                if (sourceCode[i] == '\\' && i + 1 < len) {
                    i += 2;
                } else {
                    if (sourceCode[i] == '\n') {
                        currentLine++;
                        lineStartPos = i + 1;
                    }
                    i++;
                }
            }
            if (i < len && sourceCode[i] == '"') {
                i++;
            }
            std::string strLit = sourceCode.substr(startIdx, i - startIdx);
            tokens.emplace_back(strLit, TokenType::STRING_LITERAL, currentLine, startCol);
            continue;
        }

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
                tokens.emplace_back(ident, TokenType::IDENTIFIER, currentLine, startCol);
            }
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)) || (c == '.' && i + 1 < len && std::isdigit(static_cast<unsigned char>(sourceCode[i + 1])))) {
            size_t startCol = getCol(i);
            size_t startIdx = i;
            bool isFloat = (c == '.');

            if (c == '0' && i + 1 < len && (sourceCode[i + 1] == 'x' || sourceCode[i + 1] == 'X')) {
                i += 2;
                while (i < len && (std::isxdigit(static_cast<unsigned char>(sourceCode[i])) || sourceCode[i] == '\'')) {
                    i++;
                }
            } else if (c == '0' && i + 1 < len && (sourceCode[i + 1] == 'b' || sourceCode[i + 1] == 'B')) {
                i += 2;
                while (i < len && (sourceCode[i] == '0' || sourceCode[i] == '1' || sourceCode[i] == '\'')) {
                    i++;
                }
            } else {
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
            while (i < len && (std::isalpha(static_cast<unsigned char>(sourceCode[i])))) {
                i++;
            }

            std::string numLex = sourceCode.substr(startIdx, i - startIdx);
            tokens.emplace_back(numLex, isFloat ? TokenType::FLOATING_LITERAL : TokenType::INTEGER_LITERAL, currentLine, startCol);
            continue;
        }

        size_t startCol = getCol(i);
        if (i + 2 < len) {
            std::string op3 = sourceCode.substr(i, 3);
            if (op3 == "..." || op3 == "<<=" || op3 == ">>=" || op3 == "<=>" || op3 == "->*") {
                tokens.emplace_back(op3, TokenType::OPERATOR, currentLine, startCol);
                i += 3;
                continue;
            }
        }
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

        if (isPunctuationChar(c)) {
            tokens.emplace_back(std::string(1, c), TokenType::PUNCTUATION, currentLine, startCol);
            i++;
            continue;
        }

        if (isOperatorChar(c)) {
            tokens.emplace_back(std::string(1, c), TokenType::OPERATOR, currentLine, startCol);
            i++;
            continue;
        }

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
