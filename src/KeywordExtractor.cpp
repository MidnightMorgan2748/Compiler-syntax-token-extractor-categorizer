#include "KeywordExtractor.hpp"
#include <iomanip>

KeywordExtractor::KeywordExtractor() {}

const std::unordered_map<std::string, KeywordCategory>& KeywordExtractor::getKeywordRegistry() {
    static const std::unordered_map<std::string, KeywordCategory> registry = {
        // Control Flow
        {"if", KeywordCategory::CONTROL_FLOW},
        {"else", KeywordCategory::CONTROL_FLOW},
        {"switch", KeywordCategory::CONTROL_FLOW},
        {"case", KeywordCategory::CONTROL_FLOW},
        {"default", KeywordCategory::CONTROL_FLOW},
        {"while", KeywordCategory::CONTROL_FLOW},
        {"do", KeywordCategory::CONTROL_FLOW},
        {"for", KeywordCategory::CONTROL_FLOW},
        {"break", KeywordCategory::CONTROL_FLOW},
        {"continue", KeywordCategory::CONTROL_FLOW},
        {"return", KeywordCategory::CONTROL_FLOW},
        {"goto", KeywordCategory::CONTROL_FLOW},

        // Data Types / Primitives
        {"int", KeywordCategory::DATA_TYPE},
        {"char", KeywordCategory::DATA_TYPE},
        {"float", KeywordCategory::DATA_TYPE},
        {"double", KeywordCategory::DATA_TYPE},
        {"void", KeywordCategory::DATA_TYPE},
        {"bool", KeywordCategory::DATA_TYPE},
        {"short", KeywordCategory::DATA_TYPE},
        {"long", KeywordCategory::DATA_TYPE},
        {"signed", KeywordCategory::DATA_TYPE},
        {"unsigned", KeywordCategory::DATA_TYPE},
        {"wchar_t", KeywordCategory::DATA_TYPE},
        {"char16_t", KeywordCategory::DATA_TYPE},
        {"char32_t", KeywordCategory::DATA_TYPE},
        {"char8_t", KeywordCategory::DATA_TYPE},
        {"auto", KeywordCategory::DATA_TYPE},

        // Storage Classes & Modifiers
        {"const", KeywordCategory::MODIFIER_STORAGE},
        {"volatile", KeywordCategory::MODIFIER_STORAGE},
        {"static", KeywordCategory::MODIFIER_STORAGE},
        {"extern", KeywordCategory::MODIFIER_STORAGE},
        {"register", KeywordCategory::MODIFIER_STORAGE},
        {"mutable", KeywordCategory::MODIFIER_STORAGE},
        {"constexpr", KeywordCategory::MODIFIER_STORAGE},
        {"consteval", KeywordCategory::MODIFIER_STORAGE},
        {"constinit", KeywordCategory::MODIFIER_STORAGE},
        {"inline", KeywordCategory::MODIFIER_STORAGE},

        // OOP, Classes, Structs & Access
        {"class", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"struct", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"union", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"enum", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"public", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"private", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"protected", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"friend", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"virtual", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"override", KeywordCategory::CLASS_STRUCT_ACCESS},
        {"final", KeywordCategory::CLASS_STRUCT_ACCESS},

        // Memory & Exception Handling
        {"new", KeywordCategory::MEMORY_EXCEPTION},
        {"delete", KeywordCategory::MEMORY_EXCEPTION},
        {"this", KeywordCategory::MEMORY_EXCEPTION},
        {"try", KeywordCategory::MEMORY_EXCEPTION},
        {"catch", KeywordCategory::MEMORY_EXCEPTION},
        {"throw", KeywordCategory::MEMORY_EXCEPTION},
        {"noexcept", KeywordCategory::MEMORY_EXCEPTION},
        {"nullptr", KeywordCategory::MEMORY_EXCEPTION},

        // Templates, Casts, Types & Namespaces
        {"template", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"typename", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"namespace", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"using", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"static_cast", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"dynamic_cast", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"const_cast", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"reinterpret_cast", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"typeid", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"sizeof", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"decltype", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"typedef", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"explicit", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"export", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"concept", KeywordCategory::TEMPLATE_CAST_SPEC},
        {"requires", KeywordCategory::TEMPLATE_CAST_SPEC},

        // Concurrency & Coroutines
        {"thread_local", KeywordCategory::CONCURRENCY},
        {"co_await", KeywordCategory::CONCURRENCY},
        {"co_return", KeywordCategory::CONCURRENCY},
        {"co_yield", KeywordCategory::CONCURRENCY},

        // Other Standard Keywords
        {"asm", KeywordCategory::OTHER_KEYWORD},
        {"static_assert", KeywordCategory::OTHER_KEYWORD},
        {"alignas", KeywordCategory::OTHER_KEYWORD},
        {"alignof", KeywordCategory::OTHER_KEYWORD}
    };
    return registry;
}

bool KeywordExtractor::isKeyword(const std::string& lexeme) {
    const auto& registry = getKeywordRegistry();
    return registry.find(lexeme) != registry.end();
}

KeywordCategory KeywordExtractor::getCategory(const std::string& keyword) {
    const auto& registry = getKeywordRegistry();
    auto it = registry.find(keyword);
    if (it != registry.end()) {
        return it->second;
    }
    return KeywordCategory::NOT_A_KEYWORD;
}

ExtractionResult KeywordExtractor::extract(const std::vector<std::string>& tokens) const {
    ExtractionResult result{};
    result.totalTokensIngested = tokens.size();
    result.totalKeywordOccurrences = 0;
    result.duplicateRejectionsCount = 0;

    // Ingest and isolate unique keywords using std::set<std::string>
    for (const auto& token : tokens) {
        if (isKeyword(token)) {
            result.totalKeywordOccurrences++;
            result.keywordFrequencies[token]++;

            // Demonstrating std::set unique insertion properties:
            // insert() returns std::pair<iterator, bool>
            // .second is true if new element was inserted, false if duplicate rejected
            auto [iter, inserted] = result.uniqueKeywords.insert(token);
            if (!inserted) {
                result.duplicateRejectionsCount++;
            }
        }
    }

    result.uniqueKeywordCount = result.uniqueKeywords.size();

    // Calculate statistical metrics
    if (result.totalTokensIngested > 0) {
        result.keywordDensityPercentage = 
            (static_cast<double>(result.totalKeywordOccurrences) / result.totalTokensIngested) * 100.0;
    } else {
        result.keywordDensityPercentage = 0.0;
    }

    if (result.totalKeywordOccurrences > 0) {
        result.uniquenessRatio = 
            (static_cast<double>(result.uniqueKeywordCount) / result.totalKeywordOccurrences) * 100.0;
        result.duplicateSuppressionRatio = 
            (static_cast<double>(result.duplicateRejectionsCount) / result.totalKeywordOccurrences) * 100.0;
    } else {
        result.uniquenessRatio = 0.0;
        result.duplicateSuppressionRatio = 0.0;
    }

    // Populate detailed metrics in lexicographical order (from std::set traversal)
    for (const auto& kw : result.uniqueKeywords) {
        size_t freq = result.keywordFrequencies[kw];
        double relFreq = (result.totalKeywordOccurrences > 0)
            ? (static_cast<double>(freq) / result.totalKeywordOccurrences) * 100.0
            : 0.0;
        double strmPct = (result.totalTokensIngested > 0)
            ? (static_cast<double>(freq) / result.totalTokensIngested) * 100.0
            : 0.0;

        result.detailedMetrics.push_back({
            kw,
            freq,
            relFreq,
            strmPct,
            getCategory(kw)
        });
    }

    return result;
}

void KeywordExtractor::printFormattedReport(const ExtractionResult& result, std::ostream& os) {
    os << "\n========================================================================================\n";
    os << "                 LEXICOGRAPHICALLY SORTED UNIQUE SYNTAX KEYWORDS & METRICS\n";
    os << "========================================================================================\n";
    os << "  Total Tokens Ingested into std::vector<std::string> : " << result.totalTokensIngested << "\n";
    os << "  Total Syntax Keyword Occurrences in Stream          : " << result.totalKeywordOccurrences << "\n";
    os << "  Unique Keywords Isolated in std::set<std::string>   : " << result.uniqueKeywordCount << "\n";
    os << "  Duplicate Insertions Suppressed by std::set         : " << result.duplicateRejectionsCount << "\n";
    os << "  Keyword Density in Source Stream                    : " 
       << std::fixed << std::setprecision(2) << result.keywordDensityPercentage << " %\n";
    os << "  Uniqueness Ratio (Distinct / Total Keywords)        : " 
       << std::fixed << std::setprecision(2) << result.uniquenessRatio << " %\n";
    os << "  Duplicate Suppression Efficiency                    : " 
       << std::fixed << std::setprecision(2) << result.duplicateSuppressionRatio << " %\n";
    os << "----------------------------------------------------------------------------------------\n";
    os << std::left 
       << std::setw(6)  << "No."
       << std::setw(20) << "Keyword (std::set)"
       << std::setw(30) << "Category"
       << std::setw(12) << "Frequency"
       << std::setw(14) << "Share (Kwds)"
       << std::setw(14) << "Share (Stream)"
       << "\n";
    os << "----------------------------------------------------------------------------------------\n";

    size_t idx = 1;
    for (const auto& metric : result.detailedMetrics) {
        os << std::left
           << std::setw(6)  << idx++
           << std::setw(20) << metric.keyword
           << std::setw(30) << keywordCategoryToString(metric.category)
           << std::setw(12) << metric.originalFrequency
           << std::fixed << std::setprecision(2)
           << std::setw(13) << (std::to_string(metric.relativeFrequency).substr(0, 5) + " %")
           << std::setw(14) << (std::to_string(metric.streamPercentage).substr(0, 5) + " %")
           << "\n";
    }

    os << "========================================================================================\n\n";
}
