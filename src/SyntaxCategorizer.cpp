#include "SyntaxCategorizer.hpp"
#include <iomanip>

SyntaxCategorizer::SyntaxCategorizer() {}

CategorizationReport SyntaxCategorizer::categorize(const std::vector<Token>& tokens) const {
    CategorizationReport report{};
    report.totalTokens = tokens.size();

    const std::vector<TokenType> allTypes = {
        TokenType::KEYWORD,
        TokenType::IDENTIFIER,
        TokenType::INTEGER_LITERAL,
        TokenType::FLOATING_LITERAL,
        TokenType::STRING_LITERAL,
        TokenType::CHAR_LITERAL,
        TokenType::BOOLEAN_LITERAL,
        TokenType::OPERATOR,
        TokenType::PUNCTUATION,
        TokenType::PREPROCESSOR,
        TokenType::COMMENT,
        TokenType::UNKNOWN
    };

    for (auto t : allTypes) {
        report.tokenTypeStats[t] = CategoryStat{tokenTypeToString(t), 0, 0, 0.0, {}};
    }

    const std::vector<KeywordCategory> allKwCats = {
        KeywordCategory::CONTROL_FLOW,
        KeywordCategory::DATA_TYPE,
        KeywordCategory::MODIFIER_STORAGE,
        KeywordCategory::CLASS_STRUCT_ACCESS,
        KeywordCategory::MEMORY_EXCEPTION,
        KeywordCategory::TEMPLATE_CAST_SPEC,
        KeywordCategory::CONCURRENCY,
        KeywordCategory::OTHER_KEYWORD
    };

    for (auto kc : allKwCats) {
        report.keywordCategoryStats[kc] = CategoryStat{keywordCategoryToString(kc), 0, 0, 0.0, {}};
    }

    for (const auto& token : tokens) {
        TokenType effectiveType = token.type;
        KeywordCategory effectiveKwCat = token.keywordCategory;

        if (effectiveType == TokenType::IDENTIFIER && KeywordExtractor::isKeyword(token.lexeme)) {
            effectiveType = TokenType::KEYWORD;
            effectiveKwCat = KeywordExtractor::getCategory(token.lexeme);
        }

        auto& tStat = report.tokenTypeStats[effectiveType];
        tStat.totalCount++;
        tStat.uniqueLexemes.insert(token.lexeme);

        if (effectiveType == TokenType::KEYWORD && effectiveKwCat != KeywordCategory::NOT_A_KEYWORD) {
            auto& kStat = report.keywordCategoryStats[effectiveKwCat];
            kStat.totalCount++;
            kStat.uniqueLexemes.insert(token.lexeme);
        }
    }

    for (auto& [type, stat] : report.tokenTypeStats) {
        stat.uniqueCount = stat.uniqueLexemes.size();
        if (report.totalTokens > 0) {
            stat.percentageOfTotal = (static_cast<double>(stat.totalCount) / report.totalTokens) * 100.0;
        }
    }

    size_t totalKwTokens = report.tokenTypeStats[TokenType::KEYWORD].totalCount;
    for (auto& [kwCat, stat] : report.keywordCategoryStats) {
        stat.uniqueCount = stat.uniqueLexemes.size();
        if (totalKwTokens > 0) {
            stat.percentageOfTotal = (static_cast<double>(stat.totalCount) / totalKwTokens) * 100.0;
        }
    }

    return report;
}

void SyntaxCategorizer::printFormattedReport(const CategorizationReport& report, std::ostream& os) {
    os << "\n========================================================================================\n";
    os << "                   COMPREHENSIVE SOURCE CODE SYNTAX CATEGORIZATION\n";
    os << "========================================================================================\n";
    os << "  Total Syntactic Tokens Analyzed: " << report.totalTokens << "\n";
    os << "----------------------------------------------------------------------------------------\n";
    os << std::left 
       << std::setw(22) << "Token Category"
       << std::setw(14) << "Total Tokens"
       << std::setw(14) << "Unique Items"
       << std::setw(16) << "Stream Share"
       << std::setw(24) << "Sample Items (up to 3)"
       << "\n";
    os << "----------------------------------------------------------------------------------------\n";

    for (const auto& [type, stat] : report.tokenTypeStats) {
        if (stat.totalCount == 0) continue;

        std::string sample = "";
        size_t count = 0;
        for (const auto& lex : stat.uniqueLexemes) {
            if (count > 0) sample += ", ";
            sample += "'" + (lex.length() > 8 ? lex.substr(0, 8) + ".." : lex) + "'";
            if (++count >= 3) break;
        }

        os << std::left
           << std::setw(22) << stat.categoryName
           << std::setw(14) << stat.totalCount
           << std::setw(14) << stat.uniqueCount
           << std::fixed << std::setprecision(2)
           << std::setw(15) << (std::to_string(stat.percentageOfTotal).substr(0, 5) + " %")
           << std::setw(24) << sample
           << "\n";
    }

    os << "----------------------------------------------------------------------------------------\n";
    os << "                 KEYWORD SUB-TAXONOMY BREAKDOWN (std::set Segregation)\n";
    os << "----------------------------------------------------------------------------------------\n";
    os << std::left 
       << std::setw(32) << "Keyword Subgroup"
       << std::setw(14) << "Occurrences"
       << std::setw(14) << "Unique Kwds"
       << std::setw(16) << "Keyword Share"
       << std::setw(18) << "Sample Keywords"
       << "\n";
    os << "----------------------------------------------------------------------------------------\n";

    for (const auto& [kwCat, stat] : report.keywordCategoryStats) {
        if (stat.totalCount == 0) continue;

        std::string sample = "";
        size_t count = 0;
        for (const auto& lex : stat.uniqueLexemes) {
            if (count > 0) sample += ", ";
            sample += lex;
            if (++count >= 3) break;
        }

        os << std::left
           << std::setw(32) << stat.categoryName
           << std::setw(14) << stat.totalCount
           << std::setw(14) << stat.uniqueCount
           << std::fixed << std::setprecision(2)
           << std::setw(15) << (std::to_string(stat.percentageOfTotal).substr(0, 5) + " %")
           << std::setw(18) << sample
           << "\n";
    }

    os << "========================================================================================\n\n";
}
