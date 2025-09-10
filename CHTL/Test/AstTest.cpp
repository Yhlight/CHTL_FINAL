#include "AstTest.h"

namespace CHTL {
namespace Test {

std::unique_ptr<TestSuite> AstTest::createTestSuite() {
    auto suite = std::make_unique<TestSuite>("AstTest");
    
    // 节点基本功能测试
    suite->addTestCase(std::make_unique<TestCase>("testBaseNode", "测试BaseNode基本功能", testBaseNode));
    suite->addTestCase(std::make_unique<TestCase>("testElementNode", "测试ElementNode功能", testElementNode));
    suite->addTestCase(std::make_unique<TestCase>("testTextNode", "测试TextNode功能", testTextNode));
    suite->addTestCase(std::make_unique<TestCase>("testCommentNode", "测试CommentNode功能", testCommentNode));
    suite->addTestCase(std::make_unique<TestCase>("testStyleNode", "测试StyleNode功能", testStyleNode));
    suite->addTestCase(std::make_unique<TestCase>("testScriptNode", "测试ScriptNode功能", testScriptNode));
    suite->addTestCase(std::make_unique<TestCase>("testTemplateNode", "测试TemplateNode功能", testTemplateNode));
    suite->addTestCase(std::make_unique<TestCase>("testCustomNode", "测试CustomNode功能", testCustomNode));
    suite->addTestCase(std::make_unique<TestCase>("testOriginNode", "测试OriginNode功能", testOriginNode));
    suite->addTestCase(std::make_unique<TestCase>("testImportNode", "测试ImportNode功能", testImportNode));
    suite->addTestCase(std::make_unique<TestCase>("testNamespaceNode", "测试NamespaceNode功能", testNamespaceNode));
    suite->addTestCase(std::make_unique<TestCase>("testConstraintNode", "测试ConstraintNode功能", testConstraintNode));
    suite->addTestCase(std::make_unique<TestCase>("testConfigurationNode", "测试ConfigurationNode功能", testConfigurationNode));
    suite->addTestCase(std::make_unique<TestCase>("testUseNode", "测试UseNode功能", testUseNode));
    
    // 解析器测试
    suite->addTestCase(std::make_unique<TestCase>("testParserBasic", "测试解析器基本功能", testParserBasic));
    suite->addTestCase(std::make_unique<TestCase>("testParserElement", "测试解析器元素解析功能", testParserElement));
    suite->addTestCase(std::make_unique<TestCase>("testParserStyle", "测试解析器样式解析功能", testParserStyle));
    suite->addTestCase(std::make_unique<TestCase>("testParserScript", "测试解析器脚本解析功能", testParserScript));
    suite->addTestCase(std::make_unique<TestCase>("testParserTemplate", "测试解析器模板解析功能", testParserTemplate));
    suite->addTestCase(std::make_unique<TestCase>("testParserCustom", "测试解析器自定义解析功能", testParserCustom));
    suite->addTestCase(std::make_unique<TestCase>("testParserOrigin", "测试解析器原始嵌入解析功能", testParserOrigin));
    suite->addTestCase(std::make_unique<TestCase>("testParserImport", "测试解析器导入解析功能", testParserImport));
    suite->addTestCase(std::make_unique<TestCase>("testParserNamespace", "测试解析器命名空间解析功能", testParserNamespace));
    suite->addTestCase(std::make_unique<TestCase>("testParserConstraint", "测试解析器约束解析功能", testParserConstraint));
    suite->addTestCase(std::make_unique<TestCase>("testParserConfiguration", "测试解析器配置解析功能", testParserConfiguration));
    suite->addTestCase(std::make_unique<TestCase>("testParserUse", "测试解析器use解析功能", testParserUse));
    suite->addTestCase(std::make_unique<TestCase>("testParserComplex", "测试解析器复杂结构解析功能", testParserComplex));
    
    return suite;
}

// 节点基本功能测试实现
void AstTest::testBaseNode() {
    BaseNode node(NodeType::ELEMENT);
    CHTL_ASSERT_TRUE(node.getNodeType() == NodeType::ELEMENT);
    CHTL_ASSERT_EQUAL("", node.getValue());
    CHTL_ASSERT_EQUAL("0", std::to_string(node.getLine()));
    CHTL_ASSERT_EQUAL("0", std::to_string(node.getColumn()));
    
    node.setValue("test");
    CHTL_ASSERT_EQUAL("test", node.getValue());
    
    node.setPosition(10, 20, 100);
    CHTL_ASSERT_EQUAL("10", std::to_string(node.getLine()));
    CHTL_ASSERT_EQUAL("20", std::to_string(node.getColumn()));
}

void AstTest::testElementNode() {
    ElementNode element("div");
    CHTL_ASSERT_EQUAL("div", element.getTagName());
    CHTL_ASSERT_TRUE(element.getNodeType() == NodeType::ELEMENT);
    
    element.addAttribute("class", "container");
    element.addAttribute("id", "main");
    
    CHTL_ASSERT_EQUAL("container", element.getAttribute("class"));
    CHTL_ASSERT_EQUAL("main", element.getAttribute("id"));
    CHTL_ASSERT_TRUE(element.hasAttribute("class"));
    CHTL_ASSERT_FALSE(element.hasAttribute("nonexistent"));
    
    auto attributes = element.getAttributes();
    CHTL_ASSERT_EQUAL("2", std::to_string(attributes.size()));
}

void AstTest::testTextNode() {
    TextNode text("Hello World");
    CHTL_ASSERT_EQUAL("Hello World", text.getText());
    CHTL_ASSERT_TRUE(text.getType() == NodeType::TEXT);
    
    text.setText("Updated text");
    CHTL_ASSERT_EQUAL("Updated text", text.getText());
}

void AstTest::testCommentNode() {
    CommentNode comment("This is a comment");
    CHTL_ASSERT_EQUAL("This is a comment", comment.getComment());
    CHTL_ASSERT_TRUE(comment.getType() == NodeType::COMMENT);
    
    comment.setComment("Updated comment");
    CHTL_ASSERT_EQUAL("Updated comment", comment.getComment());
}

void AstTest::testStyleNode() {
    StyleNode style("color: red; font-size: 16px;");
    CHTL_ASSERT_EQUAL("color: red; font-size: 16px;", style.getStyle());
    CHTL_ASSERT_TRUE(style.getType() == NodeType::STYLE);
    
    style.setStyle("background: blue;");
    CHTL_ASSERT_EQUAL("background: blue;", style.getStyle());
}

void AstTest::testScriptNode() {
    ScriptNode script("console.log('Hello');");
    CHTL_ASSERT_EQUAL("console.log('Hello');", script.getScript());
    CHTL_ASSERT_TRUE(script.getType() == NodeType::SCRIPT);
    
    script.setScript("alert('World');");
    CHTL_ASSERT_EQUAL("alert('World');", script.getScript());
}

void AstTest::testTemplateNode() {
    TemplateNode template("MyTemplate");
    CHTL_ASSERT_EQUAL("MyTemplate", template.getTemplateName());
    CHTL_ASSERT_TRUE(template.getType() == NodeType::TEMPLATE);
    
    template.setTemplateName("UpdatedTemplate");
    CHTL_ASSERT_EQUAL("UpdatedTemplate", template.getTemplateName());
}

void AstTest::testCustomNode() {
    CustomNode custom("MyCustom");
    CHTL_ASSERT_EQUAL("MyCustom", custom.getCustomName());
    CHTL_ASSERT_TRUE(custom.getType() == NodeType::CUSTOM);
    
    custom.setCustomName("UpdatedCustom");
    CHTL_ASSERT_EQUAL("UpdatedCustom", custom.getCustomName());
}

void AstTest::testOriginNode() {
    OriginNode origin("Html", "content");
    CHTL_ASSERT_EQUAL("Html", origin.getOriginType());
    CHTL_ASSERT_EQUAL("content", origin.getContent());
    CHTL_ASSERT_TRUE(origin.getType() == NodeType::ORIGIN);
    
    origin.setOriginType("Style");
    origin.setContent("updated content");
    CHTL_ASSERT_EQUAL("Style", origin.getOriginType());
    CHTL_ASSERT_EQUAL("updated content", origin.getContent());
}

void AstTest::testImportNode() {
    ImportNode import("module", "path/to/module");
    CHTL_ASSERT_EQUAL("module", import.getModuleName());
    CHTL_ASSERT_EQUAL("path/to/module", import.getPath());
    CHTL_ASSERT_TRUE(import.getType() == NodeType::IMPORT);
    
    import.setModuleName("updatedModule");
    import.setPath("new/path");
    CHTL_ASSERT_EQUAL("updatedModule", import.getModuleName());
    CHTL_ASSERT_EQUAL("new/path", import.getPath());
}

void AstTest::testNamespaceNode() {
    NamespaceNode namespace("MyNamespace");
    CHTL_ASSERT_EQUAL("MyNamespace", namespace.getNamespaceName());
    CHTL_ASSERT_TRUE(namespace.getType() == NodeType::NAMESPACE);
    
    namespace.setNamespaceName("UpdatedNamespace");
    CHTL_ASSERT_EQUAL("UpdatedNamespace", namespace.getNamespaceName());
}

void AstTest::testConstraintNode() {
    ConstraintNode constraint("except", "div");
    CHTL_ASSERT_EQUAL("except", constraint.getConstraintType());
    CHTL_ASSERT_EQUAL("div", constraint.getTarget());
    CHTL_ASSERT_TRUE(constraint.getType() == NodeType::CONSTRAINT);
    
    constraint.setConstraintType("precise");
    constraint.setTarget("span");
    CHTL_ASSERT_EQUAL("precise", constraint.getConstraintType());
    CHTL_ASSERT_EQUAL("span", constraint.getTarget());
}

void AstTest::testConfigurationNode() {
    ConfigurationNode config("MyConfig");
    CHTL_ASSERT_EQUAL("MyConfig", config.getConfigName());
    CHTL_ASSERT_TRUE(config.getType() == NodeType::CONFIGURATION);
    
    config.setConfigName("UpdatedConfig");
    CHTL_ASSERT_EQUAL("UpdatedConfig", config.getConfigName());
}

void AstTest::testUseNode() {
    UseNode use("html5");
    CHTL_ASSERT_EQUAL("html5", use.getUseType());
    CHTL_ASSERT_TRUE(use.getType() == NodeType::USE);
    
    use.setUseType("custom");
    CHTL_ASSERT_EQUAL("custom", use.getUseType());
}

// 解析器测试实现
void AstTest::testParserBasic() {
    CHTLParser parser;
    std::string source = "div: Hello World";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
    CHTL_ASSERT_TRUE(ast->getType() == NodeType::ELEMENT);
}

void AstTest::testParserElement() {
    CHTLParser parser;
    std::string source = "div.class#id: Text content";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
    
    auto element = dynamic_cast<ElementNode*>(ast.get());
    CHTL_ASSERT_NOT_NULL(element);
    CHTL_ASSERT_EQUAL("div", element->getTagName());
}

void AstTest::testParserStyle() {
    CHTLParser parser;
    std::string source = "div { color: red; font-size: 16px; }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserScript() {
    CHTLParser parser;
    std::string source = "script: console.log('Hello World');";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserTemplate() {
    CHTLParser parser;
    std::string source = "[Template] MyTemplate { div: Content }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserCustom() {
    CHTLParser parser;
    std::string source = "[Custom] MyCustom { div: Custom content }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserOrigin() {
    CHTLParser parser;
    std::string source = "[Origin] @Html <div>Raw HTML</div>";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserImport() {
    CHTLParser parser;
    std::string source = "[Import] MyModule from 'path/to/module'";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserNamespace() {
    CHTLParser parser;
    std::string source = "[Namespace] MyNamespace { div: Content }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserConstraint() {
    CHTLParser parser;
    std::string source = "[Constraint] except div { span: Content }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserConfiguration() {
    CHTLParser parser;
    std::string source = "[Configuration] MyConfig { INDEX_INITIAL_COUNT: 10 }";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserUse() {
    CHTLParser parser;
    std::string source = "use html5";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

void AstTest::testParserComplex() {
    CHTLParser parser;
    std::string source = R"(
[Template] MyTemplate {
    div.container {
        class: "main"
        h1: "Title"
        p: "Content"
    }
}

[Custom] MyCustom {
    div: "Custom content"
}

use html5
    )";
    
    auto ast = parser.parse(source);
    CHTL_ASSERT_NOT_NULL(ast.get());
}

} // namespace Test
} // namespace CHTL