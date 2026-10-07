#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>

struct KeywordMetric {
    std::string keyword;
    size_t originalFrequency;
    double relativeFrequency;
    double streamPercentage;
    KeywordCategory category;
};

struct ExtractionResult {
    std::set<std::string> uniqueKeywords;
    std::map<std::string, size_t> keywordFrequencies;
    std::vector<KeywordMetric> detailedMetrics;
    size_t totalTokensIngested;
    size_t totalKeywordOccurrences;
    size_t uniqueKeywordCount;
    size_t duplicateRejectionsCount;
    double keywordDensityPercentage;
    double uniquenessRatio;
    double duplicateSuppressionRatio;
};

class KeywordExtractor {
public:
    KeywordExtractor();

    static bool isKeyword(const std::string& lexeme);
    static KeywordCategory getCategory(const std::string& keyword);

    ExtractionResult extract(const std::vector<std::string>& tokens) const;
    static void printFormattedReport(const ExtractionResult& result, std::ostream& os = std::cout);

private:
    static const std::unordered_map<std::string, KeywordCategory>& getKeywordRegistry();
};
