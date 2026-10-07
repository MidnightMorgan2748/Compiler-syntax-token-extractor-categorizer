#pragma once

#include "Token.hpp"
#include "KeywordExtractor.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

/**
 * @file SyntaxCategorizer.hpp
 * @brief Classifies and categorizes all ingested tokens into syntax groups and subgroups.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

struct CategoryStat {
    std::string categoryName;
    size_t totalCount;
    size_t uniqueCount;
    double percentageOfTotal;
    std::set<std::string> uniqueLexemes;
};

struct CategorizationReport {
    size_t totalTokens;
    std::map<TokenType, CategoryStat> tokenTypeStats;
    std::map<KeywordCategory, CategoryStat> keywordCategoryStats;
};

class SyntaxCategorizer {
public:
    SyntaxCategorizer();

    /**
     * @brief Analyzes detailed tokens and constructs a comprehensive syntax categorization report.
     */
    CategorizationReport categorize(const std::vector<Token>& tokens) const;

    /**
     * @brief Prints a formatted categorization report to the output stream.
     */
    static void printFormattedReport(const CategorizationReport& report, std::ostream& os = std::cout);
};
