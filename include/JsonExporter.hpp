#pragma once

#include "Token.hpp"
#include "KeywordExtractor.hpp"
#include "SyntaxCategorizer.hpp"
#include "ComplexityEvaluator.hpp"
#include <string>
#include <vector>

/**
 * @file JsonExporter.hpp
 * @brief Exports tokenization, keyword extraction, and benchmark results to standard JSON.
 *        Serves as the data interchange bridge for future Frontend GUI / Web applications.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

class JsonExporter {
public:
    JsonExporter();

    /**
     * @brief Serializes all parser data, keyword analytics, and benchmark results into a JSON file.
     * 
     * @param outputPath File path to write the JSON data.
     * @param sourceName Identifier or file name of the analyzed source code.
     * @param rawTokens Ingested dynamic vector of strings (std::vector<std::string>).
     * @param detailedTokens Enriched tokens with location and type metadata.
     * @param extractionResult Lexicographically sorted unique keywords and frequency metrics.
     * @param categorizationReport Comprehensive category breakdown.
     * @param benchmarkRows Complexity evaluation benchmarks.
     * @return bool True if export succeeded, false otherwise.
     */
    static bool exportAnalysis(const std::string& outputPath,
                               const std::string& sourceName,
                               const std::vector<std::string>& rawTokens,
                               const std::vector<Token>& detailedTokens,
                               const ExtractionResult& extractionResult,
                               const CategorizationReport& categorizationReport,
                               const std::vector<BenchmarkRow>& benchmarkRows = {});

    /**
     * @brief Generates an interactive HTML dashboard that visualizes the analysis results.
     */
    static bool exportHtmlDashboard(const std::string& htmlOutputPath,
                                   const std::string& jsonFileName = "analysis_result.json");

private:
    static std::string escapeJsonString(const std::string& input);
};
