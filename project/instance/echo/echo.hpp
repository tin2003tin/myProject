#ifndef ECHO_HPP
#define ECHO_HPP

#include <string>
#include "./echoCompiler.hpp"
#include "../../compiler/LRcomplier.hpp"

class Echo
{
public:
    Echo() : compiler(tin_compiler::LRCompiler(grammarRule, std::make_shared<Handler>())) {}

    void send(const std::string &input)
    {
        auto tokens = compiler.getLexer().setInput(input).tokenize();
        // tin_compiler::Token::display(tokens);
        // compiler.getParser().setLog(true);
        try
        {
            auto ast = compiler.getParser().setTokens(tokens).parse();
            compiler.getHandlerControl().setAST(ast).execute();
        }
        catch (const std::exception &error)
        {
            std::cerr << "Parsing error: " << error.what() << std::endl;
        }
    }

private:
    tin_compiler::LRCompiler compiler;
};

#endif