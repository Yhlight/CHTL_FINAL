#ifndef CHTL_AST_TEST_H
#define CHTL_AST_TEST_H

#include "TestFramework.h"
#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/CommentNode.h"
#include "../CHTL/CHTLNode/StyleNode.h"
#include "../CHTL/CHTLNode/ScriptNode.h"
#include "../CHTL/CHTLNode/TemplateNode.h"
#include "../CHTL/CHTLNode/CustomNode.h"
#include "../CHTL/CHTLNode/OriginNode.h"
#include "../CHTL/CHTLNode/ImportNode.h"
#include "../CHTL/CHTLNode/NamespaceNode.h"
#include "../CHTL/CHTLNode/ConstraintNode.h"
#include "../CHTL/CHTLNode/ConfigurationNode.h"
#include "../CHTL/CHTLNode/UseNode.h"
#include "../CHTL/CHTLParser/CHTLParser.h"

namespace CHTL {
namespace Test {

class AstTest {
public:
    static std::unique_ptr<TestSuite> createTestSuite();
    
private:
    static void testBaseNode();
    static void testElementNode();
    static void testTextNode();
    static void testCommentNode();
    static void testStyleNode();
    static void testScriptNode();
    static void testTemplateNode();
    static void testCustomNode();
    static void testOriginNode();
    static void testImportNode();
    static void testNamespaceNode();
    static void testConstraintNode();
    static void testConfigurationNode();
    static void testUseNode();
    static void testParserBasic();
    static void testParserElement();
    static void testParserStyle();
    static void testParserScript();
    static void testParserTemplate();
    static void testParserCustom();
    static void testParserOrigin();
    static void testParserImport();
    static void testParserNamespace();
    static void testParserConstraint();
    static void testParserConfiguration();
    static void testParserUse();
    static void testParserComplex();
};

} // namespace Test
} // namespace CHTL

#endif // CHTL_AST_TEST_H