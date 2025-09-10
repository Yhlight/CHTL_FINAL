#include "chtl/origin_embed.h"
#include <iostream>
#include <cassert>
#include <algorithm>

using namespace chtl::origin_embed;

void testOriginEmbedParser() {
    std::cout << "Testing OriginEmbedParser..." << std::endl;
    
    std::string input = R"(
[Origin] @HTML {
    <div>Hello World</div>
}

[Origin] @CSS {
    body { color: red; }
}

[Origin] @JS {
    console.log("Hello World");
}

[Origin] @Vue {
    <template>
        <div>{{ message }}</div>
    </template>
}
)";
    
    OriginEmbedParser parser(input);
    auto blocks = parser.parse();
    
    std::cout << "Parsed " << blocks.size() << " blocks" << std::endl;
    for (size_t i = 0; i < blocks.size(); ++i) {
        std::cout << "Block " << i << ": type=" << static_cast<int>(blocks[i]->type) << std::endl;
    }
    
    assert(blocks.size() == 4);
    assert(blocks[0]->type == OriginType::HTML);
    assert(blocks[1]->type == OriginType::CSS);
    assert(blocks[2]->type == OriginType::JS);
    assert(blocks[3]->type == OriginType::VUE);
    
    std::cout << "✓ OriginEmbedParser test passed" << std::endl;
}

void testOriginEmbedCompiler() {
    std::cout << "Testing OriginEmbedCompiler..." << std::endl;
    
    OriginEmbedCompiler compiler;
    
    auto htmlBlock = std::make_shared<OriginBlock>("html1", OriginType::HTML);
    htmlBlock->content = "<div>Hello World</div>";
    
    auto cssBlock = std::make_shared<OriginBlock>("css1", OriginType::CSS);
    cssBlock->content = "body { color: red; }";
    
    auto jsBlock = std::make_shared<OriginBlock>("js1", OriginType::JS);
    jsBlock->content = "console.log('Hello World');";
    
    std::vector<std::shared_ptr<OriginBlock>> blocks = {htmlBlock, cssBlock, jsBlock};
    
    std::string result = compiler.compile(blocks);
    assert(!result.empty());
    
    // Test individual compilation
    std::string htmlResult = compiler.compileHTML("<div>Test</div>");
    assert(htmlResult.find("<!DOCTYPE html>") != std::string::npos);
    
    std::string cssResult = compiler.compileCSS("body { color: red; }");
    assert(cssResult.find("<style>") != std::string::npos);
    
    std::string jsResult = compiler.compileJS("console.log('test');");
    assert(jsResult.find("<script>") != std::string::npos);
    
    std::cout << "✓ OriginEmbedCompiler test passed" << std::endl;
}

void testOriginEmbedManager() {
    std::cout << "Testing OriginEmbedManager..." << std::endl;
    
    OriginEmbedManager manager;
    
    auto htmlBlock = std::make_shared<OriginBlock>("html1", OriginType::HTML);
    htmlBlock->content = "<div>Hello World</div>";
    
    auto cssBlock = std::make_shared<OriginBlock>("css1", OriginType::CSS);
    cssBlock->content = "body { color: red; }";
    
    auto jsBlock = std::make_shared<OriginBlock>("js1", OriginType::JS);
    jsBlock->content = "console.log('Hello World');";
    
    manager.addOriginBlock(htmlBlock);
    manager.addOriginBlock(cssBlock);
    manager.addOriginBlock(jsBlock);
    
    auto retrievedBlock = manager.getOriginBlock("html1");
    assert(retrievedBlock != nullptr);
    assert(retrievedBlock->type == OriginType::HTML);
    
    // Test validation
    assert(manager.validateHTML("<div>Test</div>"));
    assert(manager.validateCSS("body { color: red; }"));
    assert(manager.validateJS("console.log('test');"));
    
    // Test custom type registration
    manager.registerCustomType("Vue", "vue-processor");
    assert(manager.getCustomTypeProcessor("Vue") == "vue-processor");
    
    auto customTypes = manager.getRegisteredCustomTypes();
    assert(customTypes.size() == 1);
    assert(customTypes[0] == "Vue");
    
    std::cout << "✓ OriginEmbedManager test passed" << std::endl;
}

void testOriginEmbedValidator() {
    std::cout << "Testing OriginEmbedValidator..." << std::endl;
    
    OriginEmbedValidator validator;
    
    auto htmlBlock = std::make_shared<OriginBlock>("html1", OriginType::HTML);
    htmlBlock->content = "<div>Hello World</div>";
    
    auto cssBlock = std::make_shared<OriginBlock>("css1", OriginType::CSS);
    cssBlock->content = "body { color: red; }";
    
    auto jsBlock = std::make_shared<OriginBlock>("js1", OriginType::JS);
    jsBlock->content = "console.log('Hello World');";
    
    std::vector<std::shared_ptr<OriginBlock>> blocks = {htmlBlock, cssBlock, jsBlock};
    validator.setOriginBlocks(blocks);
    
    // Test validation
    assert(validator.validate(*htmlBlock));
    assert(validator.validate(*cssBlock));
    assert(validator.validate(*jsBlock));
    
    // Test error handling
    auto emptyBlock = std::make_shared<OriginBlock>("", OriginType::HTML);
    assert(!validator.validate(*emptyBlock));
    
    auto errors = validator.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "✓ OriginEmbedValidator test passed" << std::endl;
}

void testOriginEmbedProcessor() {
    std::cout << "Testing OriginEmbedProcessor..." << std::endl;
    
    // Test processing functions
    std::string htmlContent = "<div>Hello World</div>";
    std::string processedHTML = OriginEmbedProcessor::processHTML(htmlContent);
    assert(processedHTML == htmlContent);
    
    std::string cssContent = "body { color: red; }";
    std::string processedCSS = OriginEmbedProcessor::processCSS(cssContent);
    assert(processedCSS == cssContent);
    
    std::string jsContent = "console.log('Hello World');";
    std::string processedJS = OriginEmbedProcessor::processJS(jsContent);
    assert(processedJS == jsContent);
    
    // Test extraction functions
    std::string extractedHTML = OriginEmbedProcessor::extractHTML(htmlContent);
    assert(extractedHTML == htmlContent);
    
    std::string extractedCSS = OriginEmbedProcessor::extractCSS(cssContent);
    assert(extractedCSS == cssContent);
    
    std::string extractedJS = OriginEmbedProcessor::extractJS(jsContent);
    assert(extractedJS == jsContent);
    
    // Test merging functions
    auto htmlBlock = std::make_shared<OriginBlock>("html1", OriginType::HTML);
    htmlBlock->content = "<div>Hello World</div>";
    
    auto cssBlock = std::make_shared<OriginBlock>("css1", OriginType::CSS);
    cssBlock->content = "body { color: red; }";
    
    auto jsBlock = std::make_shared<OriginBlock>("js1", OriginType::JS);
    jsBlock->content = "console.log('Hello World');";
    
    std::vector<std::shared_ptr<OriginBlock>> blocks = {htmlBlock, cssBlock, jsBlock};
    
    std::string merged = OriginEmbedProcessor::mergeOriginBlocks(blocks);
    assert(!merged.empty());
    
    std::string mergedHTML = OriginEmbedProcessor::mergeHTMLBlocks(blocks);
    assert(mergedHTML.find("<div>Hello World</div>") != std::string::npos);
    
    std::string mergedCSS = OriginEmbedProcessor::mergeCSSBlocks(blocks);
    assert(mergedCSS.find("body { color: red; }") != std::string::npos);
    
    std::string mergedJS = OriginEmbedProcessor::mergeJSBlocks(blocks);
    assert(mergedJS.find("console.log('Hello World');") != std::string::npos);
    
    std::cout << "✓ OriginEmbedProcessor test passed" << std::endl;
}

void testOriginEmbedSupport() {
    std::cout << "Testing OriginEmbedSupport..." << std::endl;
    
    // Test supported types
    auto types = OriginEmbedSupport::getSupportedTypes();
    assert(types.size() == 7);
    assert(std::find(types.begin(), types.end(), std::string("HTML")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("CSS")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("JS")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("Vue")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("React")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("Angular")) != types.end());
    assert(std::find(types.begin(), types.end(), std::string("Custom")) != types.end());
    
    // Test type checking
    assert(OriginEmbedSupport::isSupportedType("HTML"));
    assert(OriginEmbedSupport::isSupportedType("CSS"));
    assert(OriginEmbedSupport::isSupportedType("JS"));
    assert(OriginEmbedSupport::isSupportedType("Vue"));
    assert(OriginEmbedSupport::isSupportedType("React"));
    assert(OriginEmbedSupport::isSupportedType("Angular"));
    assert(OriginEmbedSupport::isSupportedType("Custom"));
    assert(!OriginEmbedSupport::isSupportedType("Unknown"));
    
    // Test type descriptions
    std::string htmlDesc = OriginEmbedSupport::getTypeDescription("HTML");
    assert(htmlDesc == "HTML markup language");
    
    std::string cssDesc = OriginEmbedSupport::getTypeDescription("CSS");
    assert(cssDesc == "CSS stylesheet language");
    
    std::string jsDesc = OriginEmbedSupport::getTypeDescription("JS");
    assert(jsDesc == "JavaScript programming language");
    
    // Test type features
    auto htmlFeatures = OriginEmbedSupport::getTypeFeatures("HTML");
    assert(htmlFeatures.size() == 4);
    assert(std::find(htmlFeatures.begin(), htmlFeatures.end(), std::string("Markup")) != htmlFeatures.end());
    assert(std::find(htmlFeatures.begin(), htmlFeatures.end(), std::string("Elements")) != htmlFeatures.end());
    assert(std::find(htmlFeatures.begin(), htmlFeatures.end(), std::string("Attributes")) != htmlFeatures.end());
    assert(std::find(htmlFeatures.begin(), htmlFeatures.end(), std::string("Structure")) != htmlFeatures.end());
    
    // Test code generation
    std::string originBlock = OriginEmbedSupport::generateOriginBlock("HTML", "<div>Test</div>");
    assert(originBlock.find("[Origin] @{ HTML <div>Test</div> }") != std::string::npos);
    
    std::string originImport = OriginEmbedSupport::generateOriginImport("HTML");
    assert(originImport.find("import { HTML } from 'origin-embed';") != std::string::npos);
    
    std::string originUsage = OriginEmbedSupport::generateOriginUsage("HTML", "<div>Test</div>");
    assert(originUsage.find("// Using HTML origin embedding") != std::string::npos);
    
    std::cout << "✓ OriginEmbedSupport test passed" << std::endl;
}

void testOriginEmbedIntegration() {
    std::cout << "Testing OriginEmbedIntegration..." << std::endl;
    
    // Test complete workflow
    std::string input = R"(
[Origin] @HTML {
    <div class="container">
        <h1>Hello World</h1>
        <p>This is a test</p>
    </div>
}

[Origin] @CSS {
    .container {
        max-width: 800px;
        margin: 0 auto;
        padding: 20px;
    }
    
    h1 {
        color: #333;
        font-size: 2em;
    }
}

[Origin] @JS {
    document.addEventListener('DOMContentLoaded', function() {
        console.log('Page loaded');
    });
}
)";
    
    // Parse
    OriginEmbedParser parser(input);
    auto blocks = parser.parse();
    assert(blocks.size() == 3);
    
    // Compile
    OriginEmbedCompiler compiler;
    std::string result = compiler.compile(blocks);
    assert(!result.empty());
    
    // Validate
    OriginEmbedValidator validator;
    validator.setOriginBlocks(blocks);
    
    for (const auto& block : blocks) {
        assert(validator.validate(*block));
    }
    
    // Process
    std::string processedHTML = OriginEmbedProcessor::processHTML(blocks[0]->content);
    assert(processedHTML.find("Hello World") != std::string::npos);
    
    std::string processedCSS = OriginEmbedProcessor::processCSS(blocks[1]->content);
    assert(processedCSS.find(".container") != std::string::npos);
    
    std::string processedJS = OriginEmbedProcessor::processJS(blocks[2]->content);
    assert(processedJS.find("DOMContentLoaded") != std::string::npos);
    
    // Merge
    std::string merged = OriginEmbedProcessor::mergeOriginBlocks(blocks);
    assert(merged.find("Hello World") != std::string::npos);
    assert(merged.find(".container") != std::string::npos);
    assert(merged.find("DOMContentLoaded") != std::string::npos);
    
    std::cout << "✓ OriginEmbedIntegration test passed" << std::endl;
}

int main() {
    std::cout << "Starting Origin Embed System tests..." << std::endl;
    
    try {
        testOriginEmbedParser();
        testOriginEmbedCompiler();
        testOriginEmbedManager();
        testOriginEmbedValidator();
        testOriginEmbedProcessor();
        testOriginEmbedSupport();
        testOriginEmbedIntegration();
        
        std::cout << "\n✅ All Origin Embed System tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}