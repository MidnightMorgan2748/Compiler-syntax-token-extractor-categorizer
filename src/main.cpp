#include "Token.hpp"
#include "Tokenizer.hpp"
#include "KeywordExtractor.hpp"
#include "SyntaxCategorizer.hpp"
#include "ComplexityEvaluator.hpp"
#include "JsonExporter.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

// Built-in demonstration C++ program snippet
const std::string DEFAULT_DEMO_CODE = R"CPP(// Sample C++ Program for Compiler Syntax Token Extraction
#include <iostream>
#include <vector>
#include <string>

namespace ParserDemo {
    template <typename T>
    class DynamicContainer {
    private:
        std::vector<T> elements;
        const size_t capacityLimit = 1000;
        static int totalContainers;

    public:
        DynamicContainer() {
            totalContainers++;
        }

        virtual ~DynamicContainer() noexcept {
            totalContainers--;
        }

        void addElement(const T& item) {
            if (elements.size() >= capacityLimit) {
                throw std::runtime_error("Capacity limit exceeded");
            }
            elements.push_back(item);
        }

        auto getSize() const noexcept -> size_t {
            return elements.size();
        }
    };

    template <typename T>
    int DynamicContainer<T>::totalContainers = 0;
}

int main(int argc, char* argv[]) {
    using namespace ParserDemo;
    
    // Allocate dynamic container
    DynamicContainer<int>* containerPtr = new DynamicContainer<int>();
    
    for (int i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            containerPtr->addElement(i * 10);
        } else {
            continue;
        }
    }

    std::cout << "Container size: " << containerPtr->getSize() << std::endl;

    delete containerPtr;
    containerPtr = nullptr;
    return 0;
}
)CPP";

struct SessionState {
    std::string sourceName = "Default Built-in C++ Demo";
    std::string sourceCode = DEFAULT_DEMO_CODE;
    std::vector<std::string> rawTokens;
    std::vector<Token> detailedTokens;
    ExtractionResult extractionResult;
    CategorizationReport categorizationReport;
    std::vector<BenchmarkRow> benchmarkResults;
    bool hasAnalyzed = false;
};

void runAnalysis(SessionState& state) {
    Tokenizer tokenizer;
    KeywordExtractor extractor;
    SyntaxCategorizer categorizer;

    state.detailedTokens = tokenizer.tokenizeDetailed(state.sourceCode, true);
    state.rawTokens = tokenizer.tokenizeToVector(state.sourceCode, true);
    state.extractionResult = extractor.extract(state.rawTokens);
    state.categorizationReport = categorizer.categorize(state.detailedTokens);
    state.hasAnalyzed = true;

    std::cout << "\n[+] Source successfully ingested and analyzed!\n";
    std::cout << "    - Ingested Raw Tokens (std::vector)    : " << state.rawTokens.size() << "\n";
    std::cout << "    - Unique Keywords Isolated (std::set)  : " << state.extractionResult.uniqueKeywordCount << "\n";
    std::cout << "    - Duplicate Rejections Suppressed      : " << state.extractionResult.duplicateRejectionsCount << "\n";
    std::cout << "    - Keyword Density in Stream            : " << std::fixed << std::setprecision(2) 
              << state.extractionResult.keywordDensityPercentage << " %\n";
}

void printBanner() {
    std::cout << "========================================================================================\n";
    std::cout << "     GROUP 5: COMPILER SYNTAX TOKEN UNIQUE EXTRACTOR & CATEGORIZER\n";
    std::cout << "     Core Concepts: STL std::set, std::vector, std::string, C++ Basics\n";
    std::cout << "========================================================================================\n";
}

void printMenu() {
    std::cout << "\n-------------------------------- MENU OPTIONS ------------------------------------------\n";
    std::cout << " [1] Analyze Default Built-in Demonstration C++ Program\n";
    std::cout << " [2] Ingest & Analyze a C++ Source File from Disk\n";
    std::cout << " [3] Enter / Paste Custom C++ Source Code Interactively\n";
    std::cout << " [4] Display Full Ingested Raw Tokens Stream (std::vector<std::string>)\n";
    std::cout << " [5] Display Lexicographical Unique Keywords & Frequency Metrics (std::set)\n";
    std::cout << " [6] Display Comprehensive Syntax Categorization Breakdown\n";
    std::cout << " [7] Run Logarithmic Bounds Benchmark & Complexity Evaluation (std::set vs vector)\n";
    std::cout << " [8] Export Analysis to JSON & Generate Frontend HTML Dashboard\n";
    std::cout << " [9] View Theoretical Red-Black Tree Properties of std::set\n";
    std::cout << " [0] Exit Program\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << " Select option [0-9]: ";
}

void handleAnalyzeFile(SessionState& state) {
    std::cout << "\nEnter file path to ingest (e.g., samples/sample1_simple.cpp): ";
    std::string filePath;
    std::getline(std::cin, filePath);
    if (filePath.empty()) {
        std::cout << "File path cannot be empty.\n";
        return;
    }

    try {
        Tokenizer tokenizer;
        state.sourceCode = Tokenizer::readFileContents(filePath);
        state.sourceName = filePath;
        runAnalysis(state);
    } catch (const std::exception& ex) {
        std::cerr << "[-] Error reading file: " << ex.what() << "\n";
    }
}

void handleCustomInput(SessionState& state) {
    std::cout << "\nEnter / paste your C++ code. Type 'END_OF_CODE' on a new line to finish:\n";
    std::ostringstream ss;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "END_OF_CODE") break;
        ss << line << "\n";
    }
    std::string customCode = ss.str();
    if (customCode.empty()) {
        std::cout << "No code provided.\n";
        return;
    }
    state.sourceCode = customCode;
    state.sourceName = "Custom User Input";
    runAnalysis(state);
}

void handleDisplayRawTokens(const SessionState& state) {
    if (!state.hasAnalyzed) {
        std::cout << "Please analyze a program first (Option 1, 2, or 3).\n";
        return;
    }
    std::cout << "\n========================================================================================\n";
    std::cout << "         INGESTED RAW TOKEN STREAM (std::vector<std::string> Dynamic Array)\n";
    std::cout << "========================================================================================\n";
    std::cout << "Total Elements in vector: " << state.rawTokens.size() << "\n\n";

    size_t col = 0;
    for (size_t i = 0; i < state.rawTokens.size(); ++i) {
        std::string tokStr = "[" + std::to_string(i) + "] " + state.rawTokens[i];
        if (tokStr.length() > 22) tokStr = tokStr.substr(0, 19) + "..";
        std::cout << std::left << std::setw(24) << tokStr;
        if (++col % 4 == 0) std::cout << "\n";
    }
    if (col % 4 != 0) std::cout << "\n";
    std::cout << "========================================================================================\n";
}

void handleExport(SessionState& state) {
    if (!state.hasAnalyzed) {
        std::cout << "Please analyze a program first.\n";
        return;
    }

    // Ensure benchmark results are generated
    if (state.benchmarkResults.empty()) {
        std::cout << "[*] Generating benchmark metrics for the export...\n";
        ComplexityEvaluator evaluator;
        state.benchmarkResults = evaluator.runBenchmark({500, 1500, 5000, 15000});
    }

    // Create export folder if it doesn't exist
    fs::create_directories("export");

    std::string jsonPath = "export/analysis_result.json";
    std::string htmlPath = "export/dashboard.html";

    bool jsonOk = JsonExporter::exportAnalysis(
        jsonPath,
        state.sourceName,
        state.rawTokens,
        state.detailedTokens,
        state.extractionResult,
        state.categorizationReport,
        state.benchmarkResults
    );

    bool htmlOk = JsonExporter::exportHtmlDashboard(htmlPath, "analysis_result.json");

    if (jsonOk && htmlOk) {
        std::cout << "\n[+] Successfully exported data for frontend interface!\n";
        std::cout << "    - JSON Payload   : " << fs::absolute(jsonPath).string() << "\n";
        std::cout << "    - HTML Dashboard : " << fs::absolute(htmlPath).string() << "\n";
        std::cout << "    You can open '" << htmlPath << "' in any web browser to view the visual UI!\n";
    } else {
        std::cerr << "[-] Error exporting files.\n";
    }
}

int main(int argc, char* argv[]) {
    SessionState session;

    // Check for CLI arguments
    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "--help" || arg1 == "-h") {
            printBanner();
            std::cout << "Usage:\n";
            std::cout << "  " << argv[0] << "                   : Launch interactive menu interface\n";
            std::cout << "  " << argv[0] << " --file <path>     : Analyze specified file and display metrics\n";
            std::cout << "  " << argv[0] << " --benchmark       : Run empirical complexity benchmark\n";
            std::cout << "  " << argv[0] << " --demo            : Run demo, export JSON/HTML, and print report\n";
            return 0;
        } else if (arg1 == "--file" && argc > 2) {
            std::string filePath = argv[2];
            session.sourceCode = Tokenizer::readFileContents(filePath);
            session.sourceName = filePath;
            runAnalysis(session);

            std::string jsonPath = "export/analysis_result.json";
            for (int a = 3; a < argc; ++a) {
                if (std::string(argv[a]) == "--json" && a + 1 < argc) {
                    jsonPath = argv[a + 1];
                }
            }
            fs::create_directories("export");
            JsonExporter::exportAnalysis(jsonPath, session.sourceName, session.rawTokens,
                                         session.detailedTokens, session.extractionResult,
                                         session.categorizationReport);

            KeywordExtractor::printFormattedReport(session.extractionResult);
            SyntaxCategorizer::printFormattedReport(session.categorizationReport);
            return 0;
        } else if (arg1 == "--benchmark") {
            ComplexityEvaluator evaluator;
            evaluator.printTheoreticalAnalysis();
            auto results = evaluator.runBenchmark();
            evaluator.printEvaluationReport(results);
            return 0;
        } else if (arg1 == "--demo") {
            printBanner();
            runAnalysis(session);
            KeywordExtractor::printFormattedReport(session.extractionResult);
            SyntaxCategorizer::printFormattedReport(session.categorizationReport);
            handleExport(session);
            return 0;
        }
    }

    printBanner();
    // Default analyze demo snippet initially
    runAnalysis(session);

    std::string inputLine;
    while (true) {
        printMenu();
        if (!std::getline(std::cin, inputLine)) break;
        if (inputLine.empty()) continue;

        char choice = inputLine[0];
        switch (choice) {
            case '1': {
                session.sourceCode = DEFAULT_DEMO_CODE;
                session.sourceName = "Default Built-in C++ Demo";
                runAnalysis(session);
                KeywordExtractor::printFormattedReport(session.extractionResult);
                break;
            }
            case '2': {
                handleAnalyzeFile(session);
                if (session.hasAnalyzed) {
                    KeywordExtractor::printFormattedReport(session.extractionResult);
                }
                break;
            }
            case '3': {
                handleCustomInput(session);
                if (session.hasAnalyzed) {
                    KeywordExtractor::printFormattedReport(session.extractionResult);
                }
                break;
            }
            case '4': {
                handleDisplayRawTokens(session);
                break;
            }
            case '5': {
                if (session.hasAnalyzed) {
                    KeywordExtractor::printFormattedReport(session.extractionResult);
                } else {
                    std::cout << "Please analyze source code first.\n";
                }
                break;
            }
            case '6': {
                if (session.hasAnalyzed) {
                    SyntaxCategorizer::printFormattedReport(session.categorizationReport);
                } else {
                    std::cout << "Please analyze source code first.\n";
                }
                break;
            }
            case '7': {
                ComplexityEvaluator evaluator;
                evaluator.printTheoreticalAnalysis();
                std::cout << "[*] Executing empirical benchmark across scales (N = 500 to 30,000)...\n";
                session.benchmarkResults = evaluator.runBenchmark();
                evaluator.printEvaluationReport(session.benchmarkResults);
                break;
            }
            case '8': {
                handleExport(session);
                break;
            }
            case '9': {
                ComplexityEvaluator::printTheoreticalAnalysis();
                break;
            }
            case '0': {
                std::cout << "\nExiting Compiler Syntax Token Extractor. Goodbye!\n";
                return 0;
            }
            default: {
                std::cout << "Invalid choice. Please enter a valid number [0-9].\n";
                break;
            }
        }
    }

    return 0;
}
