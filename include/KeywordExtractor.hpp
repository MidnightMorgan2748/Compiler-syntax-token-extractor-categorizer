#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>

/**
 * @file KeywordExtractor.hpp
 * @brief Isolates unique syntax keywords in lexicographically sorted order using std::set<std::string>
 *        and calculates comprehensive original frequency metrics.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

struct KeywordMetric {
    std::string keyword;
    size_t originalFrequency;   // Occurrences in original token stream
    double relativeFrequency;   // Percentage of total keywords
    double streamPercentage;    // Percentage of total source tokens
    KeywordCategory category;
};

struct ExtractionResult {
    std::set<std::string> uniqueKeywords;                 // Lexicographically sorted unique keywords (std::set)
    std::map<std::string, size_t> keywordFrequencies;     // Frequency of each keyword in the source stream
    std::vector<KeywordMetric> detailedMetrics;           // Sorted enriched metrics
    size_t totalTokensIngested;                           // Total tokens in std::vector<std::string>
    size_t totalKeywordOccurrences;                       // Total keyword appearances in vector
    size_t uniqueKeywordCount;                            // Number of distinct keywords
    size_t duplicateRejectionsCount;                      // Number of duplicate insertions rejected by std::set
    double keywordDensityPercentage;                      // (totalKeywords / totalTokens) * 100
    double uniquenessRatio;                               // (uniqueKeywords / totalKeywords) * 100
    double duplicateSuppressionRatio;                     // (duplicateRejections / totalKeywords) * 100
};

class KeywordExtractor {
public:
    KeywordExtractor();

    /**
     * @brief Checks if a lexeme is a standard C++ language keyword.
     */
    static bool isKeyword(const std::string& lexeme);

    /**
     * @brief Returns the semantic category of a C++ keyword.
     */
    static KeywordCategory getCategory(const std::string& keyword);

    /**
     * @brief Ingests a dynamic sequence of tokens from std::vector<std::string>
     *        and utilizes std::set<std::string> to isolate unique syntax keywords
     *        in lexicographical order while tracking original frequency metrics.
     * 
     * @param tokens Ingested token sequence (std::vector<std::string>).
     * @return ExtractionResult Full extraction analytics and lexicographical keyword set.
     */
    ExtractionResult extract(const std::vector<std::string>& tokens) const;

    /**
     * @brief Formats and prints the unique keywords and frequency metrics to an output stream.
     */
    static void printFormattedReport(const ExtractionResult& result, std::ostream& os = std::cout);

private:
    static const std::unordered_map<std::string, KeywordCategory>& getKeywordRegistry();
};
