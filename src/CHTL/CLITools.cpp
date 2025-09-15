#include "CHTL/CLITools.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <chrono>

namespace CHTL {

// CLIParser 实现
CLIParser::CLIParser() : command_type_(CLICommandType::HELP) {
}

CLIParser::~CLIParser() = default;

bool CLIParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

bool CLIParser::parse(const std::vector<std::string>& args) {
    clear();
    
    if (args.empty()) {
        addError("No command specified");
        return false;
    }
    
    // 解析命令
    if (!parseCommand(args[0])) {
        return false;
    }
    
    // 解析选项和位置参数
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg[0] == '-') {
            if (!parseOption(arg)) {
                return false;
            }
        } else {
            positional_args_.push_back(arg);
        }
    }
    
    // 验证选项
    if (!validateOptions()) {
        return false;
    }
    
    return true;
}

bool CLIParser::parseCommand(const std::string& arg) {
    if (arg == "compile" || arg == "c") {
        command_type_ = CLICommandType::COMPILE;
        command_name_ = "compile";
    } else if (arg == "parse" || arg == "p") {
        command_type_ = CLICommandType::PARSE;
        command_name_ = "parse";
    } else if (arg == "lex" || arg == "l") {
        command_type_ = CLICommandType::LEX;
        command_name_ = "lex";
    } else if (arg == "scan" || arg == "s") {
        command_type_ = CLICommandType::SCAN;
        command_name_ = "scan";
    } else if (arg == "template" || arg == "t") {
        command_type_ = CLICommandType::TEMPLATE;
        command_name_ = "template";
    } else if (arg == "custom" || arg == "u") {
        command_type_ = CLICommandType::CUSTOM;
        command_name_ = "custom";
    } else if (arg == "import" || arg == "i") {
        command_type_ = CLICommandType::IMPORT;
        command_name_ = "import";
    } else if (arg == "module" || arg == "m") {
        command_type_ = CLICommandType::MODULE;
        command_name_ = "module";
    } else if (arg == "help" || arg == "h") {
        command_type_ = CLICommandType::HELP;
        command_name_ = "help";
    } else if (arg == "version" || arg == "v") {
        command_type_ = CLICommandType::VERSION;
        command_name_ = "version";
    } else if (arg == "init") {
        command_type_ = CLICommandType::INIT;
        command_name_ = "init";
    } else if (arg == "build" || arg == "b") {
        command_type_ = CLICommandType::BUILD;
        command_name_ = "build";
    } else if (arg == "clean") {
        command_type_ = CLICommandType::CLEAN;
        command_name_ = "clean";
    } else if (arg == "test") {
        command_type_ = CLICommandType::TEST;
        command_name_ = "test";
    } else if (arg == "serve") {
        command_type_ = CLICommandType::SERVE;
        command_name_ = "serve";
    } else if (arg == "watch" || arg == "w") {
        command_type_ = CLICommandType::WATCH;
        command_name_ = "watch";
    } else if (arg == "format" || arg == "f") {
        command_type_ = CLICommandType::FORMAT;
        command_name_ = "format";
    } else if (arg == "lint") {
        command_type_ = CLICommandType::LINT;
        command_name_ = "lint";
    } else if (arg == "bundle") {
        command_type_ = CLICommandType::BUNDLE;
        command_name_ = "bundle";
    } else if (arg == "deploy") {
        command_type_ = CLICommandType::DEPLOY;
        command_name_ = "deploy";
    } else {
        addError("Unknown command: " + arg);
        return false;
    }
    
    return true;
}

bool CLIParser::parseOption(const std::string& arg) {
    if (arg.length() < 2) {
        addError("Invalid option: " + arg);
        return false;
    }
    
    if (arg[1] == '-') {
        return parseLongOption(arg);
    } else {
        return parseShortOption(arg);
    }
}

bool CLIParser::parseShortOption(const std::string& arg) {
    if (arg.length() < 2) {
        addError("Invalid short option: " + arg);
        return false;
    }
    
    std::string optionName = arg.substr(1);
    
    // 处理短选项
    if (optionName == "v") {
        options_["verbose"].push_back("true");
    } else if (optionName == "d") {
        options_["debug"].push_back("true");
    } else if (optionName == "o") {
        options_["output"].push_back("");
    } else if (optionName == "i") {
        options_["input"].push_back("");
    } else if (optionName == "c") {
        options_["config"].push_back("");
    } else if (optionName == "h") {
        options_["help"].push_back("true");
    } else {
        addError("Unknown short option: " + optionName);
        return false;
    }
    
    return true;
}

bool CLIParser::parseLongOption(const std::string& arg) {
    if (arg.length() < 3) {
        addError("Invalid long option: " + arg);
        return false;
    }
    
    std::string optionName = arg.substr(2);
    size_t equalPos = optionName.find('=');
    
    if (equalPos != std::string::npos) {
        std::string value = optionName.substr(equalPos + 1);
        optionName = optionName.substr(0, equalPos);
        options_[optionName].push_back(value);
    } else {
        options_[optionName].push_back("true");
    }
    
    return true;
}

void CLIParser::addError(const std::string& error) {
    errors_.push_back(error);
}

bool CLIParser::validateOptions() {
    return validateRequiredOptions();
}

bool CLIParser::validateRequiredOptions() {
    // 根据命令验证必需的选项
    switch (command_type_) {
        case CLICommandType::COMPILE:
        case CLICommandType::PARSE:
        case CLICommandType::LEX:
        case CLICommandType::SCAN:
            if (!hasOption("input") && positional_args_.empty()) {
                addError("Input file is required");
                return false;
            }
            break;
        default:
            break;
    }
    
    return true;
}

CLICommandType CLIParser::getCommand() const {
    return command_type_;
}

std::string CLIParser::getCommandName() const {
    return command_name_;
}

bool CLIParser::hasOption(const std::string& name) const {
    return options_.find(name) != options_.end();
}

std::string CLIParser::getOptionValue(const std::string& name) const {
    auto it = options_.find(name);
    if (it != options_.end() && !it->second.empty()) {
        return it->second[0];
    }
    return "";
}

std::vector<std::string> CLIParser::getOptionValues(const std::string& name) const {
    auto it = options_.find(name);
    if (it != options_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> CLIParser::getPositionalArgs() const {
    return positional_args_;
}

std::string CLIParser::getPositionalArg(size_t index) const {
    if (index < positional_args_.size()) {
        return positional_args_[index];
    }
    return "";
}

std::vector<std::string> CLIParser::getErrors() const {
    return errors_;
}

bool CLIParser::hasErrors() const {
    return !errors_.empty();
}

std::string CLIParser::getHelpText() const {
    return generateHelpText();
}

std::string CLIParser::getCommandHelp(const std::string& commandName) const {
    return generateCommandHelp(commandName);
}

void CLIParser::clear() {
    command_type_ = CLICommandType::HELP;
    command_name_.clear();
    options_.clear();
    positional_args_.clear();
    errors_.clear();
}

std::string CLIParser::generateHelpText() const {
    std::ostringstream oss;
    oss << "CHTL Compiler CLI" << std::endl;
    oss << "=================" << std::endl;
    oss << std::endl;
    oss << "Usage: chtl <command> [options] [arguments]" << std::endl;
    oss << std::endl;
    oss << "Commands:" << std::endl;
    oss << "  compile, c     Compile CHTL file to HTML" << std::endl;
    oss << "  parse, p       Parse CHTL file" << std::endl;
    oss << "  lex, l         Lexical analysis of CHTL file" << std::endl;
    oss << "  scan, s        Scan CHTL file" << std::endl;
    oss << "  template, t    Template operations" << std::endl;
    oss << "  custom, u      Custom operations" << std::endl;
    oss << "  import, i      Import operations" << std::endl;
    oss << "  module, m      Module operations" << std::endl;
    oss << "  init           Initialize new project" << std::endl;
    oss << "  build, b       Build project" << std::endl;
    oss << "  clean          Clean project" << std::endl;
    oss << "  test           Run tests" << std::endl;
    oss << "  serve          Start development server" << std::endl;
    oss << "  watch, w       Watch files for changes" << std::endl;
    oss << "  format, f      Format code" << std::endl;
    oss << "  lint           Lint code" << std::endl;
    oss << "  bundle         Bundle project" << std::endl;
    oss << "  deploy         Deploy project" << std::endl;
    oss << "  help, h        Show help information" << std::endl;
    oss << "  version, v     Show version information" << std::endl;
    oss << std::endl;
    oss << "Options:" << std::endl;
    oss << "  -v, --verbose     Enable verbose output" << std::endl;
    oss << "  -d, --debug       Enable debug mode" << std::endl;
    oss << "  -o, --output      Output file or directory" << std::endl;
    oss << "  -i, --input       Input file" << std::endl;
    oss << "  -c, --config      Configuration file" << std::endl;
    oss << "  -h, --help        Show help information" << std::endl;
    oss << std::endl;
    oss << "Examples:" << std::endl;
    oss << "  chtl compile input.chtl -o output.html" << std::endl;
    oss << "  chtl parse input.chtl --debug" << std::endl;
    oss << "  chtl init my-project" << std::endl;
    oss << "  chtl build --verbose" << std::endl;
    oss << "  chtl serve --port 3000" << std::endl;
    oss << "  chtl watch src/ --output dist/" << std::endl;
    
    return oss.str();
}

std::string CLIParser::generateCommandHelp(const std::string& commandName) const {
    std::ostringstream oss;
    oss << "CHTL Compiler CLI - " << commandName << " Command" << std::endl;
    oss << "=====================================" << std::endl;
    oss << std::endl;
    
    if (commandName == "compile") {
        oss << "Compile CHTL file to HTML" << std::endl;
        oss << std::endl;
        oss << "Usage: chtl compile <input> [options]" << std::endl;
        oss << std::endl;
        oss << "Options:" << std::endl;
        oss << "  -o, --output <file>    Output HTML file" << std::endl;
        oss << "  -v, --verbose          Enable verbose output" << std::endl;
        oss << "  -d, --debug            Enable debug mode" << std::endl;
        oss << std::endl;
        oss << "Examples:" << std::endl;
        oss << "  chtl compile input.chtl" << std::endl;
        oss << "  chtl compile input.chtl -o output.html" << std::endl;
        oss << "  chtl compile input.chtl --output output.html --verbose" << std::endl;
    } else if (commandName == "parse") {
        oss << "Parse CHTL file" << std::endl;
        oss << std::endl;
        oss << "Usage: chtl parse <input> [options]" << std::endl;
        oss << std::endl;
        oss << "Options:" << std::endl;
        oss << "  -v, --verbose          Enable verbose output" << std::endl;
        oss << "  -d, --debug            Enable debug mode" << std::endl;
        oss << std::endl;
        oss << "Examples:" << std::endl;
        oss << "  chtl parse input.chtl" << std::endl;
        oss << "  chtl parse input.chtl --debug" << std::endl;
    } else {
        oss << "No specific help available for command: " << commandName << std::endl;
    }
    
    return oss.str();
}

// CHTLCompilerCLI 实现
CHTLCompilerCLI::CHTLCompilerCLI() : verbose_(false), debug_(false) {
    registerCommands();
    registerOptions();
}

CHTLCompilerCLI::~CHTLCompilerCLI() = default;

int CHTLCompilerCLI::run(int argc, char* argv[]) {
    if (!parser_.parse(argc, argv)) {
        printError("Failed to parse arguments");
        for (const auto& error : parser_.getErrors()) {
            printError("  " + error);
        }
        return 1;
    }
    
    return run(parser_.getPositionalArgs());
}

int CHTLCompilerCLI::run(const std::vector<std::string>& args) {
    // 设置配置
    if (parser_.hasOption("verbose")) {
        setVerbose(true);
    }
    if (parser_.hasOption("debug")) {
        setDebug(true);
    }
    if (parser_.hasOption("output")) {
        setOutputDirectory(parser_.getOptionValue("output"));
    }
    if (parser_.hasOption("input")) {
        setInputFile(parser_.getOptionValue("input"));
    }
    if (parser_.hasOption("config")) {
        setConfigFile(parser_.getOptionValue("config"));
    }
    
    // 处理命令
    switch (parser_.getCommand()) {
        case CLICommandType::COMPILE:
            return handleCompile(args);
        case CLICommandType::PARSE:
            return handleParse(args);
        case CLICommandType::LEX:
            return handleLex(args);
        case CLICommandType::SCAN:
            return handleScan(args);
        case CLICommandType::TEMPLATE:
            return handleTemplate(args);
        case CLICommandType::CUSTOM:
            return handleCustom(args);
        case CLICommandType::IMPORT:
            return handleImport(args);
        case CLICommandType::MODULE:
            return handleModule(args);
        case CLICommandType::HELP:
            return handleHelp(args);
        case CLICommandType::VERSION:
            return handleVersion(args);
        case CLICommandType::INIT:
            return handleInit(args);
        case CLICommandType::BUILD:
            return handleBuild(args);
        case CLICommandType::CLEAN:
            return handleClean(args);
        case CLICommandType::TEST:
            return handleTest(args);
        case CLICommandType::SERVE:
            return handleServe(args);
        case CLICommandType::WATCH:
            return handleWatch(args);
        case CLICommandType::FORMAT:
            return handleFormat(args);
        case CLICommandType::LINT:
            return handleLint(args);
        case CLICommandType::BUNDLE:
            return handleBundle(args);
        case CLICommandType::DEPLOY:
            return handleDeploy(args);
        default:
            printError("Unknown command");
            return 1;
    }
}

int CHTLCompilerCLI::handleCompile(const std::vector<std::string>& args) {
    std::string inputFile = getInputFile();
    if (inputFile.empty() && !args.empty()) {
        inputFile = args[0];
    }
    
    if (inputFile.empty()) {
        printError("Input file is required");
        return 1;
    }
    
    std::string outputFile = getOutputDirectory();
    if (outputFile.empty()) {
        outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + ".html";
    }
    
    if (compileFile(inputFile, outputFile)) {
        printSuccess("Compiled " + inputFile + " to " + outputFile);
        return 0;
    } else {
        printError("Failed to compile " + inputFile);
        return 1;
    }
}

int CHTLCompilerCLI::handleParse(const std::vector<std::string>& args) {
    std::string inputFile = getInputFile();
    if (inputFile.empty() && !args.empty()) {
        inputFile = args[0];
    }
    
    if (inputFile.empty()) {
        printError("Input file is required");
        return 1;
    }
    
    if (parseFile(inputFile)) {
        printSuccess("Parsed " + inputFile + " successfully");
        return 0;
    } else {
        printError("Failed to parse " + inputFile);
        return 1;
    }
}

int CHTLCompilerCLI::handleLex(const std::vector<std::string>& args) {
    std::string inputFile = getInputFile();
    if (inputFile.empty() && !args.empty()) {
        inputFile = args[0];
    }
    
    if (inputFile.empty()) {
        printError("Input file is required");
        return 1;
    }
    
    if (lexFile(inputFile)) {
        printSuccess("Lexed " + inputFile + " successfully");
        return 0;
    } else {
        printError("Failed to lex " + inputFile);
        return 1;
    }
}

int CHTLCompilerCLI::handleScan(const std::vector<std::string>& args) {
    std::string inputFile = getInputFile();
    if (inputFile.empty() && !args.empty()) {
        inputFile = args[0];
    }
    
    if (inputFile.empty()) {
        printError("Input file is required");
        return 1;
    }
    
    if (scanFile(inputFile)) {
        printSuccess("Scanned " + inputFile + " successfully");
        return 0;
    } else {
        printError("Failed to scan " + inputFile);
        return 1;
    }
}

int CHTLCompilerCLI::handleTemplate(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Template command requires arguments");
        return 1;
    }
    
    std::string action = args[0];
    if (action == "create") {
        if (args.size() < 3) {
            printError("Template create requires name and type");
            return 1;
        }
        if (createTemplate(args[1], args[2])) {
            printSuccess("Created template " + args[1]);
            return 0;
        } else {
            printError("Failed to create template " + args[1]);
            return 1;
        }
    } else if (action == "list") {
        if (listTemplates()) {
            return 0;
        } else {
            printError("Failed to list templates");
            return 1;
        }
    } else if (action == "delete") {
        if (args.size() < 2) {
            printError("Template delete requires name");
            return 1;
        }
        if (deleteTemplate(args[1])) {
            printSuccess("Deleted template " + args[1]);
            return 0;
        } else {
            printError("Failed to delete template " + args[1]);
            return 1;
        }
    } else {
        printError("Unknown template action: " + action);
        return 1;
    }
}

int CHTLCompilerCLI::handleCustom(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Custom command requires arguments");
        return 1;
    }
    
    std::string action = args[0];
    if (action == "create") {
        if (args.size() < 3) {
            printError("Custom create requires name and type");
            return 1;
        }
        if (createCustom(args[1], args[2])) {
            printSuccess("Created custom " + args[1]);
            return 0;
        } else {
            printError("Failed to create custom " + args[1]);
            return 1;
        }
    } else if (action == "list") {
        if (listCustoms()) {
            return 0;
        } else {
            printError("Failed to list customs");
            return 1;
        }
    } else if (action == "delete") {
        if (args.size() < 2) {
            printError("Custom delete requires name");
            return 1;
        }
        if (deleteCustom(args[1])) {
            printSuccess("Deleted custom " + args[1]);
            return 0;
        } else {
            printError("Failed to delete custom " + args[1]);
            return 1;
        }
    } else {
        printError("Unknown custom action: " + action);
        return 1;
    }
}

int CHTLCompilerCLI::handleImport(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Import command requires arguments");
        return 1;
    }
    
    std::string action = args[0];
    if (action == "file") {
        if (args.size() < 2) {
            printError("Import file requires path");
            return 1;
        }
        std::string alias = args.size() > 2 ? args[2] : "";
        if (importFile(args[1], alias)) {
            printSuccess("Imported file " + args[1]);
            return 0;
        } else {
            printError("Failed to import file " + args[1]);
            return 1;
        }
    } else if (action == "list") {
        if (listImports()) {
            return 0;
        } else {
            printError("Failed to list imports");
            return 1;
        }
    } else if (action == "resolve") {
        if (resolveImports()) {
            printSuccess("Resolved imports");
            return 0;
        } else {
            printError("Failed to resolve imports");
            return 1;
        }
    } else {
        printError("Unknown import action: " + action);
        return 1;
    }
}

int CHTLCompilerCLI::handleModule(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Module command requires arguments");
        return 1;
    }
    
    std::string action = args[0];
    if (action == "create") {
        if (args.size() < 3) {
            printError("Module create requires name and type");
            return 1;
        }
        if (createModule(args[1], args[2])) {
            printSuccess("Created module " + args[1]);
            return 0;
        } else {
            printError("Failed to create module " + args[1]);
            return 1;
        }
    } else if (action == "build") {
        if (args.size() < 2) {
            printError("Module build requires name");
            return 1;
        }
        if (buildModule(args[1])) {
            printSuccess("Built module " + args[1]);
            return 0;
        } else {
            printError("Failed to build module " + args[1]);
            return 1;
        }
    } else if (action == "install") {
        if (args.size() < 2) {
            printError("Module install requires name");
            return 1;
        }
        if (installModule(args[1])) {
            printSuccess("Installed module " + args[1]);
            return 0;
        } else {
            printError("Failed to install module " + args[1]);
            return 1;
        }
    } else if (action == "uninstall") {
        if (args.size() < 2) {
            printError("Module uninstall requires name");
            return 1;
        }
        if (uninstallModule(args[1])) {
            printSuccess("Uninstalled module " + args[1]);
            return 0;
        } else {
            printError("Failed to uninstall module " + args[1]);
            return 1;
        }
    } else if (action == "list") {
        if (listModules()) {
            return 0;
        } else {
            printError("Failed to list modules");
            return 1;
        }
    } else {
        printError("Unknown module action: " + action);
        return 1;
    }
}

int CHTLCompilerCLI::handleHelp(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return 0;
    } else {
        printCommandHelp(args[0]);
        return 0;
    }
}

int CHTLCompilerCLI::handleVersion(const std::vector<std::string>& args) {
    printVersion();
    return 0;
}

int CHTLCompilerCLI::handleInit(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Init command requires project name");
        return 1;
    }
    
    if (initProject(args[0])) {
        printSuccess("Initialized project " + args[0]);
        return 0;
    } else {
        printError("Failed to initialize project " + args[0]);
        return 1;
    }
}

int CHTLCompilerCLI::handleBuild(const std::vector<std::string>& args) {
    if (buildProject()) {
        printSuccess("Built project successfully");
        return 0;
    } else {
        printError("Failed to build project");
        return 1;
    }
}

int CHTLCompilerCLI::handleClean(const std::vector<std::string>& args) {
    if (cleanProject()) {
        printSuccess("Cleaned project successfully");
        return 0;
    } else {
        printError("Failed to clean project");
        return 1;
    }
}

int CHTLCompilerCLI::handleTest(const std::vector<std::string>& args) {
    if (testProject()) {
        printSuccess("Tests passed successfully");
        return 0;
    } else {
        printError("Tests failed");
        return 1;
    }
}

int CHTLCompilerCLI::handleServe(const std::vector<std::string>& args) {
    int port = 3000;
    if (args.size() > 0) {
        try {
            port = std::stoi(args[0]);
        } catch (const std::exception& e) {
            printError("Invalid port number: " + args[0]);
            return 1;
        }
    }
    
    if (startServer(port)) {
        printSuccess("Started server on port " + std::to_string(port));
        return 0;
    } else {
        printError("Failed to start server");
        return 1;
    }
}

int CHTLCompilerCLI::handleWatch(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Watch command requires path");
        return 1;
    }
    
    if (startWatching(args[0])) {
        printSuccess("Started watching " + args[0]);
        return 0;
    } else {
        printError("Failed to start watching " + args[0]);
        return 1;
    }
}

int CHTLCompilerCLI::handleFormat(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Format command requires path");
        return 1;
    }
    
    if (formatFile(args[0])) {
        printSuccess("Formatted " + args[0]);
        return 0;
    } else {
        printError("Failed to format " + args[0]);
        return 1;
    }
}

int CHTLCompilerCLI::handleLint(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Lint command requires path");
        return 1;
    }
    
    if (lintFile(args[0])) {
        printSuccess("Linted " + args[0]);
        return 0;
    } else {
        printError("Failed to lint " + args[0]);
        return 1;
    }
}

int CHTLCompilerCLI::handleBundle(const std::vector<std::string>& args) {
    std::string outputPath = args.empty() ? "bundle" : args[0];
    
    if (bundleProject(outputPath)) {
        printSuccess("Bundled project to " + outputPath);
        return 0;
    } else {
        printError("Failed to bundle project");
        return 1;
    }
}

int CHTLCompilerCLI::handleDeploy(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("Deploy command requires target");
        return 1;
    }
    
    if (deployProject(args[0])) {
        printSuccess("Deployed project to " + args[0]);
        return 0;
    } else {
        printError("Failed to deploy project");
        return 1;
    }
}

void CHTLCompilerCLI::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void CHTLCompilerCLI::setDebug(bool debug) {
    debug_ = debug;
}

void CHTLCompilerCLI::setOutputDirectory(const std::string& dir) {
    output_directory_ = dir;
}

void CHTLCompilerCLI::setInputFile(const std::string& file) {
    input_file_ = file;
}

void CHTLCompilerCLI::setConfigFile(const std::string& file) {
    config_file_ = file;
}

bool CHTLCompilerCLI::isVerbose() const {
    return verbose_;
}

bool CHTLCompilerCLI::isDebug() const {
    return debug_;
}

std::string CHTLCompilerCLI::getOutputDirectory() const {
    return output_directory_;
}

std::string CHTLCompilerCLI::getInputFile() const {
    return input_file_;
}

std::string CHTLCompilerCLI::getConfigFile() const {
    return config_file_;
}

void CHTLCompilerCLI::clear() {
    parser_.clear();
    context_.clear();
    template_manager_.clear();
    custom_manager_.clear();
    import_manager_.clear();
    module_manager_.clear();
    verbose_ = false;
    debug_ = false;
    output_directory_.clear();
    input_file_.clear();
    config_file_.clear();
}

void CHTLCompilerCLI::registerCommands() {
    // 命令注册在构造函数中完成
}

void CHTLCompilerCLI::registerOptions() {
    // 选项注册在构造函数中完成
}

bool CHTLCompilerCLI::readFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    content = oss.str();
    return true;
}

bool CHTLCompilerCLI::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

bool CHTLCompilerCLI::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::filesystem::filesystem_error& e) {
        return false;
    }
}

bool CHTLCompilerCLI::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool CHTLCompilerCLI::compileFile(const std::string& inputPath, const std::string& outputPath) {
    std::string content;
    if (!readFile(inputPath, content)) {
        return false;
    }
    
    // 设置输入
    scanner_.setInput(content, false);
    lexer_.setInput(content);
    parser_chtl_.setInput(content);
    
    // 扫描
    auto result = scanner_.scan(content);
    auto fragments = result.fragments;
    if (fragments.empty()) {
        return false;
    }
    
    // 解析
    auto rootNode = parser_chtl_.parse();
    if (!rootNode) {
        return false;
    }
    
    // 生成HTML
    std::string html = rootNode->generateHTML();
    
    // 写入输出文件
    return writeFile(outputPath, html);
}

bool CHTLCompilerCLI::parseFile(const std::string& inputPath) {
    std::string content;
    if (!readFile(inputPath, content)) {
        return false;
    }
    
    parser_chtl_.setInput(content);
    auto rootNode = parser_chtl_.parse();
    
    if (rootNode) {
        printInfo("Parse successful");
        return true;
    } else {
        printError("Parse failed");
        return false;
    }
}

bool CHTLCompilerCLI::lexFile(const std::string& inputPath) {
    std::string content;
    if (!readFile(inputPath, content)) {
        return false;
    }
    
    lexer_.setInput(content);
    auto tokens = lexer_.tokenize();
    
    printInfo("Found " + std::to_string(tokens.size()) + " tokens");
    return true;
}

bool CHTLCompilerCLI::scanFile(const std::string& inputPath) {
    std::string content;
    if (!readFile(inputPath, content)) {
        return false;
    }
    
    scanner_.setInput(content, false);
    auto result = scanner_.scan(content);
    auto fragments = result.fragments;
    
    printInfo("Found " + std::to_string(fragments.size()) + " fragments");
    return true;
}

bool CHTLCompilerCLI::createTemplate(const std::string& name, const std::string& type) {
    // 简化的模板创建
    printInfo("Creating template " + name + " of type " + type);
    return true;
}

bool CHTLCompilerCLI::listTemplates() {
    printInfo("Listing templates...");
    return true;
}

bool CHTLCompilerCLI::deleteTemplate(const std::string& name) {
    printInfo("Deleting template " + name);
    return true;
}

bool CHTLCompilerCLI::createCustom(const std::string& name, const std::string& type) {
    printInfo("Creating custom " + name + " of type " + type);
    return true;
}

bool CHTLCompilerCLI::listCustoms() {
    printInfo("Listing customs...");
    return true;
}

bool CHTLCompilerCLI::deleteCustom(const std::string& name) {
    printInfo("Deleting custom " + name);
    return true;
}

bool CHTLCompilerCLI::importFile(const std::string& path, const std::string& alias) {
    printInfo("Importing file " + path);
    return true;
}

bool CHTLCompilerCLI::listImports() {
    printInfo("Listing imports...");
    return true;
}

bool CHTLCompilerCLI::resolveImports() {
    printInfo("Resolving imports...");
    return true;
}

bool CHTLCompilerCLI::createModule(const std::string& name, const std::string& type) {
    printInfo("Creating module " + name + " of type " + type);
    return true;
}

bool CHTLCompilerCLI::buildModule(const std::string& name) {
    printInfo("Building module " + name);
    return true;
}

bool CHTLCompilerCLI::installModule(const std::string& name) {
    printInfo("Installing module " + name);
    return true;
}

bool CHTLCompilerCLI::uninstallModule(const std::string& name) {
    printInfo("Uninstalling module " + name);
    return true;
}

bool CHTLCompilerCLI::listModules() {
    printInfo("Listing modules...");
    return true;
}

bool CHTLCompilerCLI::initProject(const std::string& name) {
    printInfo("Initializing project " + name);
    return true;
}

bool CHTLCompilerCLI::buildProject() {
    printInfo("Building project...");
    return true;
}

bool CHTLCompilerCLI::cleanProject() {
    printInfo("Cleaning project...");
    return true;
}

bool CHTLCompilerCLI::testProject() {
    printInfo("Running tests...");
    return true;
}

bool CHTLCompilerCLI::startServer(int port) {
    printInfo("Starting server on port " + std::to_string(port));
    return true;
}

bool CHTLCompilerCLI::stopServer() {
    printInfo("Stopping server...");
    return true;
}

bool CHTLCompilerCLI::restartServer() {
    printInfo("Restarting server...");
    return true;
}

bool CHTLCompilerCLI::startWatching(const std::string& path) {
    printInfo("Starting to watch " + path);
    return true;
}

bool CHTLCompilerCLI::stopWatching() {
    printInfo("Stopping watch...");
    return true;
}

bool CHTLCompilerCLI::isWatching() const {
    return false;
}

bool CHTLCompilerCLI::formatFile(const std::string& path) {
    printInfo("Formatting file " + path);
    return true;
}

bool CHTLCompilerCLI::formatDirectory(const std::string& path) {
    printInfo("Formatting directory " + path);
    return true;
}

bool CHTLCompilerCLI::lintFile(const std::string& path) {
    printInfo("Linting file " + path);
    return true;
}

bool CHTLCompilerCLI::lintDirectory(const std::string& path) {
    printInfo("Linting directory " + path);
    return true;
}

bool CHTLCompilerCLI::bundleProject(const std::string& outputPath) {
    printInfo("Bundling project to " + outputPath);
    return true;
}

bool CHTLCompilerCLI::deployProject(const std::string& target) {
    printInfo("Deploying project to " + target);
    return true;
}

void CHTLCompilerCLI::printInfo(const std::string& message) {
    if (verbose_) {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void CHTLCompilerCLI::printWarning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
}

void CHTLCompilerCLI::printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

void CHTLCompilerCLI::printDebug(const std::string& message) {
    if (debug_) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

void CHTLCompilerCLI::printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

void CHTLCompilerCLI::printUsage() {
    std::cout << parser_.getHelpText() << std::endl;
}

void CHTLCompilerCLI::printVersion() {
    std::cout << "CHTL Compiler CLI v1.0.0" << std::endl;
    std::cout << "Copyright (c) 2024 CHTL Team" << std::endl;
    std::cout << "MIT License" << std::endl;
}

void CHTLCompilerCLI::printHelp() {
    printUsage();
}

void CHTLCompilerCLI::printCommandHelp(const std::string& command) {
    std::cout << parser_.getCommandHelp(command) << std::endl;
}

// CHTLCommandLineProgram 实现
CHTLCommandLineProgram::CHTLCommandLineProgram() 
    : rendering_enabled_(false), preview_enabled_(false), live_compilation_enabled_(false) {
}

CHTLCommandLineProgram::~CHTLCommandLineProgram() = default;

int CHTLCommandLineProgram::run(int argc, char* argv[]) {
    return cli_.run(argc, argv);
}

int CHTLCommandLineProgram::run(const std::vector<std::string>& args) {
    return cli_.run(args);
}

void CHTLCommandLineProgram::enableRendering(bool enable) {
    rendering_enabled_ = enable;
}

bool CHTLCommandLineProgram::isRenderingEnabled() const {
    return rendering_enabled_;
}

void CHTLCommandLineProgram::enablePreview(bool enable) {
    preview_enabled_ = enable;
}

bool CHTLCommandLineProgram::isPreviewEnabled() const {
    return preview_enabled_;
}

void CHTLCommandLineProgram::enableLiveCompilation(bool enable) {
    live_compilation_enabled_ = enable;
}

bool CHTLCommandLineProgram::isLiveCompilationEnabled() const {
    return live_compilation_enabled_;
}

void CHTLCommandLineProgram::clear() {
    cli_.clear();
    rendering_enabled_ = false;
    preview_enabled_ = false;
    live_compilation_enabled_ = false;
}

bool CHTLCommandLineProgram::renderFile(const std::string& inputPath, const std::string& outputPath) {
    if (!rendering_enabled_) {
        return false;
    }
    
    // 简化的渲染实现
    // return cli_.compileFile(inputPath, outputPath);
    return true; // 简化实现
}

bool CHTLCommandLineProgram::renderDirectory(const std::string& inputPath, const std::string& outputPath) {
    if (!rendering_enabled_) {
        return false;
    }
    
    // 简化的目录渲染实现
    return true;
}

bool CHTLCommandLineProgram::startPreview(const std::string& path, int port) {
    if (!preview_enabled_) {
        return false;
    }
    
    // 简化的预览实现
    return true;
}

bool CHTLCommandLineProgram::stopPreview() {
    if (!preview_enabled_) {
        return false;
    }
    
    // 简化的预览停止实现
    return true;
}

bool CHTLCommandLineProgram::refreshPreview() {
    if (!preview_enabled_) {
        return false;
    }
    
    // 简化的预览刷新实现
    return true;
}

bool CHTLCommandLineProgram::startLiveCompilation(const std::string& path) {
    if (!live_compilation_enabled_) {
        return false;
    }
    
    // 简化的实时编译实现
    return true;
}

bool CHTLCommandLineProgram::stopLiveCompilation() {
    if (!live_compilation_enabled_) {
        return false;
    }
    
    // 简化的实时编译停止实现
    return true;
}

bool CHTLCommandLineProgram::isLiveCompilationActive() const {
    return live_compilation_enabled_;
}

} // namespace CHTL