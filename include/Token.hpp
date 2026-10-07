#pragma once

#include <string>
#include <string_view>
#include <iostream>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOATING_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    BOOLEAN_LITERAL,
    OPERATOR,
    PUNCTUATION,
    PREPROCESSOR,
    COMMENT,
    UNKNOWN
};

enum class KeywordCategory {
    CONTROL_FLOW,
    DATA_TYPE,
    MODIFIER_STORAGE,
    CLASS_STRUCT_ACCESS,
    MEMORY_EXCEPTION,
    TEMPLATE_CAST_SPEC,
    CONCURRENCY,
    OTHER_KEYWORD,
    NOT_A_KEYWORD
};

struct Token {
    std::string lexeme;
    TokenType type;
    KeywordCategory keywordCategory;
    size_t line;
    size_t column;

    Token() 
        : lexeme(""), type(TokenType::UNKNOWN), keywordCategory(KeywordCategory::NOT_A_KEYWORD), line(0), column(0) {}

    Token(std::string lex, TokenType t, size_t ln = 0, size_t col = 0, 
          KeywordCategory kwCat = KeywordCategory::NOT_A_KEYWORD)
        : lexeme(std::move(lex)), type(t), keywordCategory(kwCat), line(ln), column(col) {}
};

std::string tokenTypeToString(TokenType type);
std::string keywordCategoryToString(KeywordCategory category);
