#include "Token.hpp"
#include "Tokenizer.hpp"
#include "KeywordExtractor.hpp"
#include "SyntaxCategorizer.hpp"
#include "ComplexityEvaluator.hpp"
#include "JsonExporter.hpp"

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

void testTokenizer() {
    std::cout << "[RUNNING TEST] testTokenizer...";
    Tokenizer tokenizer;
    std::string sample = "int x = 42; float y = 3.14f; return x;";
    std::vector<std::string> tokens = tokenizer.tokenizeToVector(sample);

    assert(!tokens.empty());
    assert(tokens[0] == "int");
    assert(tokens[1] == "x");
    assert(tokens[2] == "=");
    assert(tokens[3] == "42");
    assert(tokens[4] == ";");
    std::cout << " PASSED! (Extracted " << tokens.size() << " tokens)\n";
}

void testLexicographicalOrderingAndDuplicates() {
    std::cout << "[RUNNING TEST] testLexicographicalOrderingAndDuplicates...";
    Tokenizer tokenizer;
    std::string sample = "while (true) { for (int i = 0; i < 10; ++i) { if (i == 5) break; else continue; } return; }";
    std::vector<std::string> tokens = tokenizer.tokenizeToVector(sample);

    KeywordExtractor extractor;
    ExtractionResult result = extractor.extract(tokens);

    assert(result.uniqueKeywords.find("while") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("for") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("int") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("if") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("break") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("else") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("continue") != result.uniqueKeywords.end());
    assert(result.uniqueKeywords.find("return") != result.uniqueKeywords.end());

    std::string prev = "";
    for (const auto& kw : result.uniqueKeywords) {
        if (!prev.empty()) {
            assert(prev < kw && "std::set must preserve strict lexicographical order!");
        }
        prev = kw;
    }

    assert(result.totalKeywordOccurrences > result.uniqueKeywordCount || result.duplicateRejectionsCount >= 0);
    assert(result.totalKeywordOccurrences == result.uniqueKeywordCount + result.duplicateRejectionsCount);

    std::cout << " PASSED! (Unique: " << result.uniqueKeywordCount 
              << ", Duplicates rejected: " << result.duplicateRejectionsCount << ")\n";
}

void testSyntaxCategorizer() {
    std::cout << "[RUNNING TEST] testSyntaxCategorizer...";
    Tokenizer tokenizer;
    std::string sample = "class Data { private: int val; public: void set(int v) { val = v; } };";
    auto detailed = tokenizer.tokenizeDetailed(sample);

    SyntaxCategorizer categorizer;
    CategorizationReport report = categorizer.categorize(detailed);

    assert(report.totalTokens > 0);
    assert(report.tokenTypeStats[TokenType::KEYWORD].totalCount > 0);
    assert(report.tokenTypeStats[TokenType::IDENTIFIER].totalCount > 0);
    std::cout << " PASSED! (Total tokens categorized: " << report.totalTokens << ")\n";
}

void testComplexityEvaluator() {
    std::cout << "[RUNNING TEST] testComplexityEvaluator...";
    ComplexityEvaluator evaluator;
    auto benchmarks = evaluator.runBenchmark({100, 500});
    assert(benchmarks.size() == 2);
    assert(benchmarks[0].tokenCount == 100);
    assert(benchmarks[1].tokenCount == 500);
    assert(benchmarks[0].estimatedTreeHeight <= benchmarks[1].estimatedTreeHeight + 5);
    std::cout << " PASSED! (Benchmark runs completed)\n";
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "  RUNNING AUTOMATED UNIT TESTS & VERIFICATION SUITE\n";
    std::cout << "========================================================\n";

    try {
        testTokenizer();
        testLexicographicalOrderingAndDuplicates();
        testSyntaxCategorizer();
        testComplexityEvaluator();
        std::cout << "\n[SUCCESS] All unit tests passed cleanly!\n";
    } catch (const std::exception& ex) {
        std::cerr << "\n[FAILURE] Exception thrown during test: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
