#pragma once

#include "Token.hpp"
#include "KeywordExtractor.hpp"
#include "SyntaxCategorizer.hpp"
#include "ComplexityEvaluator.hpp"
#include <string>
#include <vector>

class JsonExporter {
public:
    JsonExporter();

    static bool exportAnalysis(const std::string& outputPath,
                               const std::string& sourceName,
                               const std::vector<std::string>& rawTokens,
                               const std::vector<Token>& detailedTokens,
                               const ExtractionResult& extractionResult,
                               const CategorizationReport& categorizationReport,
                               const std::vector<BenchmarkRow>& benchmarkRows = {});

    static bool exportHtmlDashboard(const std::string& htmlOutputPath,
                                   const std::string& jsonFileName = "analysis_result.json");

private:
    static std::string escapeJsonString(const std::string& input);
};
