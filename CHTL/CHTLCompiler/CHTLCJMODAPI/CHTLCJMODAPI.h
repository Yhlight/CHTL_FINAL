#ifndef CHTLCJMODAPI_H
#define CHTLCJMODAPI_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// Syntax analysis types
enum class SyntaxType {
    Object,
    Function,
    Array,
    String,
    Number,
    Boolean,
    Null,
    Undefined,
    CHTLJSFunction
};

// Syntax analyzer
class CHTLSyntax {
public:
    CHTLSyntax();
    ~CHTLSyntax();
    
    // Syntax analysis
    SyntaxType analyze(const std::string& code) const;
    bool isObject(const std::string& code) const;
    bool isFunction(const std::string& code) const;
    bool isArray(const std::string& code) const;
    bool isCHTLJSFunction(const std::string& code) const;
    
    // Code validation
    bool isValidSyntax(const std::string& code) const;
    std::vector<std::string> getSyntaxErrors(const std::string& code) const;
    
    // Code parsing
    std::map<std::string, std::string> parseObject(const std::string& code) const;
    std::vector<std::string> parseArray(const std::string& code) const;
    std::string parseFunction(const std::string& code) const;
    
private:
    // Helper methods
    bool isBalanced(const std::string& code, char open, char close) const;
    std::string trimWhitespace(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& code) const;
    bool isNumeric(const std::string& str) const;
    bool isString(const std::string& str) const;
    bool isBoolean(const std::string& str) const;
    bool isNull(const std::string& str) const;
    bool isUndefined(const std::string& str) const;
};

// Argument types
enum class ArgType {
    String,
    Number,
    Boolean,
    Object,
    Array,
    Function,
    CHTLJSFunction,
    Placeholder
};

// Argument structure
struct CHTLArg {
    ArgType type;
    std::string value;
    std::map<std::string, std::string> properties;
    std::vector<CHTLArg> children;
    bool is_placeholder;
    std::string placeholder_type;
    
    CHTLArg() : type(ArgType::String), is_placeholder(false) {}
    CHTLArg(ArgType t, const std::string& v = "") : type(t), value(v), is_placeholder(false) {}
};

// Argument processor
class CHTLArgProcessor {
public:
    CHTLArgProcessor();
    ~CHTLArgProcessor();
    
    // Argument binding
    CHTLArg bind(const std::string& name, const CHTLArg& value);
    CHTLArg bind(const std::string& name, const std::string& value);
    CHTLArg bind(const std::string& name, int value);
    CHTLArg bind(const std::string& name, bool value);
    
    // Argument filling
    CHTLArg fillValue(const CHTLArg& arg, const std::map<std::string, CHTLArg>& context);
    std::string fillValue(const std::string& template_str, const std::map<std::string, CHTLArg>& context);
    
    // Argument transformation
    CHTLArg transform(const CHTLArg& arg, const std::function<CHTLArg(const CHTLArg&)>& transformer);
    std::vector<CHTLArg> transformArray(const std::vector<CHTLArg>& args, const std::function<CHTLArg(const CHTLArg&)>& transformer);
    
    // Placeholder handling
    CHTLArg createPlaceholder(const std::string& type);
    bool isPlaceholder(const CHTLArg& arg) const;
    std::string getPlaceholderType(const CHTLArg& arg) const;
    
    // Argument validation
    bool validateArg(const CHTLArg& arg) const;
    std::vector<std::string> getArgErrors(const CHTLArg& arg) const;
    
private:
    std::map<std::string, CHTLArg> bound_args;
    
    // Helper methods
    std::string replacePlaceholders(const std::string& str, const std::map<std::string, CHTLArg>& context) const;
    CHTLArg processPlaceholder(const std::string& placeholder, const std::map<std::string, CHTLArg>& context) const;
    bool isValidPlaceholder(const std::string& placeholder) const;
    std::string extractPlaceholderType(const std::string& placeholder) const;
};

// CJMOD Scanner
class CHTLCJMODScanner {
public:
    CHTLCJMODScanner();
    ~CHTLCJMODScanner();
    
    // Scanning operations
    std::vector<std::string> scan(const std::string& code) const;
    std::vector<std::string> scanFile(const std::string& path) const;
    std::vector<std::string> scanDirectory(const std::string& directory) const;
    
    // Pattern matching
    std::vector<std::string> findPatterns(const std::string& code, const std::string& pattern) const;
    std::vector<std::string> findImports(const std::string& code) const;
    std::vector<std::string> findExports(const std::string& code) const;
    std::vector<std::string> findFunctions(const std::string& code) const;
    
    // Code analysis
    std::map<std::string, std::string> analyzeCode(const std::string& code) const;
    std::vector<std::string> getDependencies(const std::string& code) const;
    std::vector<std::string> getExports(const std::string& code) const;
    
    // File operations
    bool isValidFile(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    std::vector<std::string> listFiles(const std::string& directory) const;
    
private:
    // Helper methods
    std::vector<std::string> tokenize(const std::string& code) const;
    bool matchesPattern(const std::string& line, const std::string& pattern) const;
    std::string extractImport(const std::string& line) const;
    std::string extractExport(const std::string& line) const;
    std::string extractFunction(const std::string& line) const;
    bool isCJMODFile(const std::string& path) const;
};

// CJMOD Generator
class CHTLCJMODGenerator {
public:
    CHTLCJMODGenerator();
    ~CHTLCJMODGenerator();
    
    // Code generation
    std::string generateModule(const std::string& name, const std::map<std::string, std::string>& exports) const;
    std::string generateImport(const std::string& module, const std::string& name) const;
    std::string generateExport(const std::string& name, const std::string& value) const;
    std::string generateFunction(const std::string& name, const std::string& body) const;
    
    // Template generation
    std::string generateTemplate(const std::string& template_name, const std::map<std::string, std::string>& params) const;
    std::string generateCHTLJSFunction(const std::string& name, const std::string& body) const;
    
    // Code formatting
    std::string formatCode(const std::string& code) const;
    std::string minifyCode(const std::string& code) const;
    std::string beautifyCode(const std::string& code) const;
    
    // Export operations
    std::string exportResult(const std::string& code, const std::string& format = "js") const;
    std::string exportToFile(const std::string& code, const std::string& path) const;
    std::string exportToModule(const std::string& code, const std::string& module_name) const;
    
private:
    // Helper methods
    std::string indentCode(const std::string& code, int level = 0) const;
    std::string removeComments(const std::string& code) const;
    std::string removeWhitespace(const std::string& code) const;
    std::string validateCode(const std::string& code) const;
    std::string escapeString(const std::string& str) const;
};

// Atom argument types
enum class AtomArgType {
    Dollar,        // $
    DollarQuestion, // $?
    DollarExclamation, // $!
    DollarUnderscore, // $_
    Ellipsis       // ...
};

// Atom argument processor
class CHTLAtomArg {
public:
    CHTLAtomArg();
    ~CHTLAtomArg();
    
    // Atom argument creation
    CHTLArg createDollarArg() const;
    CHTLArg createDollarQuestionArg() const;
    CHTLArg createDollarExclamationArg() const;
    CHTLArg createDollarUnderscoreArg() const;
    CHTLArg createEllipsisArg() const;
    
    // Atom argument processing
    CHTLArg processAtomArg(const std::string& atom, const std::map<std::string, CHTLArg>& context) const;
    std::vector<CHTLArg> processAtomArgs(const std::vector<std::string>& atoms, const std::map<std::string, CHTLArg>& context) const;
    
    // Atom argument validation
    bool isValidAtom(const std::string& atom) const;
    AtomArgType getAtomType(const std::string& atom) const;
    
    // Placeholder operations
    std::string generatePlaceholder(const AtomArgType& type) const;
    std::string generatePlaceholder(const std::string& atom) const;
    
private:
    // Helper methods
    CHTLArg createPlaceholderArg(const AtomArgType& type) const;
    std::string atomTypeToString(const AtomArgType& type) const;
    AtomArgType stringToAtomType(const std::string& str) const;
};

// CHTL JS Function
class CHTLCHTLJSFunction {
public:
    CHTLCHTLJSFunction();
    ~CHTLCHTLJSFunction();
    
    // Function creation
    static CHTLCHTLJSFunction createCHTLJSFunction(const std::string& name, const std::string& body);
    static CHTLCHTLJSFunction createCHTLJSFunction(const std::string& name, const std::string& body, const std::vector<std::string>& params);
    
    // Function operations
    std::string getName() const;
    std::string getBody() const;
    std::vector<std::string> getParameters() const;
    std::string call(const std::vector<CHTLArg>& args) const;
    std::string call(const std::map<std::string, CHTLArg>& args) const;
    
    // Function binding
    CHTLCHTLJSFunction bind(const std::string& param, const CHTLArg& value) const;
    CHTLCHTLJSFunction bind(const std::map<std::string, CHTLArg>& bindings) const;
    CHTLCHTLJSFunction bindVirtualObject(const std::string& object_name) const;
    
    // Function validation
    bool isValid() const;
    std::vector<std::string> getErrors() const;
    bool hasParameter(const std::string& param) const;
    
    // Function utilities
    std::string generateCode() const;
    std::string generateCall(const std::vector<std::string>& args) const;
    std::string generateDeclaration() const;
    
private:
    std::string name;
    std::string body;
    std::vector<std::string> parameters;
    std::map<std::string, CHTLArg> bindings;
    
    // Helper methods
    std::string processBody(const std::string& body, const std::map<std::string, CHTLArg>& context) const;
    std::string replaceParameters(const std::string& body, const std::map<std::string, CHTLArg>& context) const;
    bool validateParameters(const std::vector<std::string>& params) const;
    std::string sanitizeName(const std::string& name) const;
};

// CJMOD API Manager
class CHTLCJMODAPIManager {
public:
    CHTLCJMODAPIManager();
    ~CHTLCJMODAPIManager();
    
    // API initialization
    bool initialize();
    void cleanup();
    
    // Component access
    CHTLSyntax& getSyntax();
    CHTLArgProcessor& getArgProcessor();
    CHTLCJMODScanner& getScanner();
    CHTLCJMODGenerator& getGenerator();
    CHTLAtomArg& getAtomArg();
    
    // API operations
    std::string processCode(const std::string& code) const;
    std::string processFile(const std::string& path) const;
    std::string processModule(const std::string& module_name) const;
    
    // API utilities
    bool isAPIReady() const;
    std::vector<std::string> getAPIVersion() const;
    std::map<std::string, std::string> getAPIInfo() const;
    
private:
    std::unique_ptr<CHTLSyntax> syntax;
    std::unique_ptr<CHTLArgProcessor> arg_processor;
    std::unique_ptr<CHTLCJMODScanner> scanner;
    std::unique_ptr<CHTLCJMODGenerator> generator;
    std::unique_ptr<CHTLAtomArg> atom_arg;
    
    bool initialized;
    
    // Helper methods
    bool initializeComponents();
    void cleanupComponents();
    bool validateAPI() const;
};

} // namespace CHTL

#endif // CHTLCJMODAPI_H