#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <memory>

/**
 * @file Tokenizer.hpp
 * @brief Lexical analyzer that parses source code into a dynamic std::vector<std::string>
 *        and enriched std::vector<Token>.
 * 
 * Part of Group 5: Compiler Syntax Token Unique Extractor & Categorizer
 */

class Tokenizer {
public:
    Tokenizer();

    /**
     * @brief Ingests raw source code string and extracts tokens into a std::vector<std::string>.
     * Satisfies the core requirement: "Ingest a sequence of source code tokens into a dynamic vector of strings (std::vector<std::string>)".
     * 
     * @param sourceCode The input source code text.
     * @param includeCommentsAndDirectives Flag to include comments and #preprocessor tokens.
     * @return std::vector<std::string> Sequence of extracted token lexemes.
     */
    std::vector<std::string> tokenizeToVector(const std::string& sourceCode, 
                                             bool includeCommentsAndDirectives = false);

    /**
     * @brief Ingests raw source code and returns detailed Token objects with type, line, column.
     * 
     * @param sourceCode The input source code text.
     * @param includeCommentsAndDirectives Flag to include comments and #preprocessor tokens.
     * @return std::vector<Token> Enriched token sequence.
     */
    std::vector<Token> tokenizeDetailed(const std::string& sourceCode, 
                                       bool includeCommentsAndDirectives = false);

    /**
     * @brief Helper to tokenize a file directly into std::vector<std::string>.
     * 
     * @param filePath Path to the source code file.
     * @param includeCommentsAndDirectives Flag to include comments and #preprocessor tokens.
     * @return std::vector<std::string>
     */
    std::vector<std::string> tokenizeFileToVector(const std::string& filePath,
                                                 bool includeCommentsAndDirectives = false);

    /**
     * @brief Helper to tokenize a file directly into std::vector<Token>.
     * 
     * @param filePath Path to the source code file.
     * @param includeCommentsAndDirectives Flag to include comments and #preprocessor tokens.
     * @return std::vector<Token>
     */
    std::vector<Token> tokenizeFileDetailed(const std::string& filePath,
                                           bool includeCommentsAndDirectives = false);

    /**
     * @brief Reads entire contents of a file into a std::string.
     */
    static std::string readFileContents(const std::string& filePath);

private:
    // Internal scanner state and helper functions
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
};
