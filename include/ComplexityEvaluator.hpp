#pragma once

#include <string>
#include <vector>
#include <iostream>

/**
 * @file ComplexityEvaluator.hpp
 * @brief Evaluates std::set unique insertion properties and logarithmic bounds O(log N)
 *        with empirical benchmarking and theoretical mathematical validation.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

struct BenchmarkRow {
    size_t tokenCount;
    size_t uniqueCount;
    double vectorLinearTimeMs;    // std::vector + linear search O(N^2)
    double setTimeMs;             // std::set Red-Black Tree O(N log U)
    double unorderedSetTimeMs;    // std::unordered_set Hash Table O(N)
    double setTheoreticalScaling; // Theoretical ratio compared to baseline
    double setEmpiricalScaling;   // Empirical ratio compared to baseline
    size_t estimatedTreeHeight;   // Max theoretical Red-Black tree height: <= 2 * log2(U + 1)
};

class ComplexityEvaluator {
public:
    ComplexityEvaluator();

    /**
     * @brief Executes the benchmark across standard dataset scales.
     * Evaluates std::set unique insertion properties and logarithmic bounds.
     */
    std::vector<BenchmarkRow> runBenchmark(const std::vector<size_t>& sampleSizes = {500, 1500, 5000, 15000, 30000});

    /**
     * @brief Formats and prints the empirical evaluation report and logarithmic analysis.
     */
    static void printEvaluationReport(const std::vector<BenchmarkRow>& results, std::ostream& os = std::cout);

    /**
     * @brief Explains the theoretical Red-Black Tree mechanics behind std::set.
     */
    static void printTheoreticalAnalysis(std::ostream& os = std::cout);

private:
    std::vector<std::string> generateSyntheticTokenStream(size_t totalTokens, double duplicateRatio);
};
