#pragma once

#include <string>
#include <vector>
#include <iostream>

struct BenchmarkRow {
    size_t tokenCount;
    size_t uniqueCount;
    double vectorLinearTimeMs;
    double setTimeMs;
    double unorderedSetTimeMs;
    double setTheoreticalScaling;
    double setEmpiricalScaling;
    size_t estimatedTreeHeight;
};

class ComplexityEvaluator {
public:
    ComplexityEvaluator();

    std::vector<BenchmarkRow> runBenchmark(const std::vector<size_t>& sampleSizes = {500, 1500, 5000, 15000, 30000});
    static void printEvaluationReport(const std::vector<BenchmarkRow>& results, std::ostream& os = std::cout);
    static void printTheoreticalAnalysis(std::ostream& os = std::cout);

private:
    std::vector<std::string> generateSyntheticTokenStream(size_t totalTokens, double duplicateRatio);
};
