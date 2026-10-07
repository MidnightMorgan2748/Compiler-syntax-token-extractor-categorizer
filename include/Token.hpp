#pragma once

#include <string>
#include <string_view>
#include <iostream>

/**
 * @file Token.hpp
 * @brief Token types, keyword categories, and Token structure definitions.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

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
    CONTROL_FLOW,          // if, else, switch, case, default, while, do, for, break, continue, return, goto
    DATA_TYPE,             // int, char, float, double, void, bool, short, long, signed, unsigned, wchar_t, auto
    MODIFIER_STORAGE,      // const, volatile, static, extern, register, mutable, constexpr, inline, consteval, constinit
    CLASS_STRUCT_ACCESS,   // class, struct, union, enum, public, private, protected, friend, virtual, override, final
    MEMORY_EXCEPTION,      // new, delete, this, try, catch, throw, noexcept, nullptr
    TEMPLATE_CAST_SPEC,    // template, typename, namespace, using, static_cast, dynamic_cast, const_cast, reinterpret_cast, typeid, sizeof, decltype
    CONCURRENCY,           // thread_local, co_await, co_return, co_yield
    OTHER_KEYWORD,         // asm, explicit, export, etc.
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

// String conversion helper declarations
std::string tokenTypeToString(TokenType type);
std::string keywordCategoryToString(KeywordCategory category);
