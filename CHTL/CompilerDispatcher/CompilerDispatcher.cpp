#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLResolver/TemplateResolver.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTLJS/CHTLJSLexer/ChtlJsLexer.h"
#include "../CHTLJS/CHTLJSParser/ChtlJsParser.h"
#include "../CHTLJS/CHTLJSGenerator/ChtlJsGenerator.h"
#include <sstream>
#include <algorithm>

std::string CompilerDispatcher::dispatch(const std::vector<CodeSnippet>& snippets) {
    std::stringstream finalHtml;
    std::stringstream globalCss;
    std::stringstream globalJs;

    std::string chtlResult;
    for (const auto& snippet : snippets) {
        switch (snippet.type) {
            case CodeType::CHTL:
                chtlResult += compileChtl(snippet.content);
                break;
            case CodeType::CSS:
                globalCss << snippet.content;
                break;
            case CodeType::CHTL_JS: {
                ChtlJsLexer cjsLexer(snippet.content);
                auto cjsTokens = cjsLexer.tokenize();

                ChtlJsParser cjsParser(cjsTokens);
                auto cjsAst = cjsParser.parse();

                ChtlJsGenerator cjsGenerator;
                globalJs << cjsGenerator.generate(cjsAst);
                break;
            }
            case CodeType::JS:
                globalJs << snippet.content;
                break;
            case CodeType::UNKNOWN:
                break;
        }
    }

    // Inject global CSS and JS from the main CHTL compile pass
    // This is a bit of a hack. The generator should return a struct with html, css, and js.
    size_t head_pos = chtlResult.find("</head>");
    size_t body_pos = chtlResult.find("</body>");

    if (globalCss.tellp() > 0) {
        std::string styleBlock = "\n<style>\n" + globalCss.str() + "</style>\n";
        if (head_pos != std::string::npos) chtlResult.insert(head_pos, styleBlock);
        else chtlResult.insert(0, styleBlock);
    }

    if (globalJs.tellp() > 0) {
        std::string scriptBlock = "\n<script>\n" + globalJs.str() + "\n</script>\n";
        if (body_pos != std::string::npos) chtlResult.insert(body_pos, scriptBlock);
        else chtlResult += scriptBlock;
    }

    finalHtml << chtlResult;
    return finalHtml.str();
}

std::string CompilerDispatcher::compileChtl(const std::string& source) {
    if (source.empty() || std::all_of(source.begin(), source.end(), isspace)) {
        return "";
    }

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::shared_ptr<RootNode> rawAst = parser.parse();
    TemplateResolver resolver;
    std::shared_ptr<RootNode> resolvedAst = resolver.resolve(rawAst);
    Generator generator;
    return generator.generate(resolvedAst);
}
