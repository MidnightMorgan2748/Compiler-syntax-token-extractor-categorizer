#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <memory>

class Tokenizer {
public:
    Tokenizer();

    std::vector<std::string> tokenizeToVector(const std::string& sourceCode, 
                                             bool includeCommentsAndDirectives = false);

    std::vector<Token> tokenizeDetailed(const std::string& sourceCode, 
                                       bool includeCommentsAndDirectives = false);

    std::vector<std::string> tokenizeFileToVector(const std::string& filePath,
                                                 bool includeCommentsAndDirectives = false);

    std::vector<Token> tokenizeFileDetailed(const std::string& filePath,
                                           bool includeCommentsAndDirectives = false);

    static std::string readFileContents(const std::string& filePath);

private:
    bool isOperatorChar(char c) const;
    bool isPunctuationChar(char c) const;
};
