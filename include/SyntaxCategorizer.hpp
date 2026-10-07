#pragma once

#include "Token.hpp"
#include "KeywordExtractor.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

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

    CategorizationReport categorize(const std::vector<Token>& tokens) const;
    static void printFormattedReport(const CategorizationReport& report, std::ostream& os = std::cout);
};
