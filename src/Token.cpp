#include "Token.hpp"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:          return "KEYWORD";
        case TokenType::IDENTIFIER:       return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL:  return "INTEGER_LITERAL";
        case TokenType::FLOATING_LITERAL: return "FLOATING_LITERAL";
        case TokenType::STRING_LITERAL:   return "STRING_LITERAL";
        case TokenType::CHAR_LITERAL:     return "CHAR_LITERAL";
        case TokenType::BOOLEAN_LITERAL:  return "BOOLEAN_LITERAL";
        case TokenType::OPERATOR:         return "OPERATOR";
        case TokenType::PUNCTUATION:      return "PUNCTUATION";
        case TokenType::PREPROCESSOR:     return "PREPROCESSOR";
        case TokenType::COMMENT:          return "COMMENT";
        case TokenType::UNKNOWN:          return "UNKNOWN";
        default:                          return "UNDEFINED";
    }
}

std::string keywordCategoryToString(KeywordCategory category) {
    switch (category) {
        case KeywordCategory::CONTROL_FLOW:        return "Control Flow";
        case KeywordCategory::DATA_TYPE:           return "Data Type / Primitive";
        case KeywordCategory::MODIFIER_STORAGE:    return "Storage Class / Modifier";
        case KeywordCategory::CLASS_STRUCT_ACCESS: return "OOP / Type / Access Specifier";
        case KeywordCategory::MEMORY_EXCEPTION:    return "Memory / Exception / Special";
        case KeywordCategory::TEMPLATE_CAST_SPEC:  return "Template / Cast / Namespace";
        case KeywordCategory::CONCURRENCY:         return "Concurrency / Coroutines";
        case KeywordCategory::OTHER_KEYWORD:       return "Other Keyword";
        case KeywordCategory::NOT_A_KEYWORD:       return "N/A (Not a Keyword)";
        default:                                   return "Unknown";
    }
}
