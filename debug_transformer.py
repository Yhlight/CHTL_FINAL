import unittest
import copy
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, TextNode, StyleNode, CssPropertyNode,
    CustomUsageNode, TemplateDefinitionNode
)
from CHTL.CHTLTransformer.transformer import ASTTransformer

# --- Test Data Setup ---
# Manually create the AST that the parser would produce for our failing case.

# 1. The Template Definition
template_content = [
    ElementNode(tag_name='div', children=[
        ElementNode(tag_name='p', children=[TextNode(value='hello')])
    ])
]

# 2. The Customization Rule Body
override_rule = ElementNode(tag_name='div', children=[
    ElementNode(tag_name='p', children=[
        StyleNode(children=[CssPropertyNode(name='color', value='red')])
    ])
])

# 3. The AST representing the file content
ast = DocumentNode(children=[
    CustomUsageNode(template_type='Element', name='MyBox', body=[override_rule])
])

# 4. The Compilation Context
context = CompilationContext()
context.add_element_template("MyBox", template_content)


# --- Run the Transformer ---
print("--- AST Before Transformation ---")
import json
class AstEncoder(json.JSONEncoder):
    def default(self, o):
        return {k: v for k, v in vars(o).items() if k != 'parent'}
print(json.dumps(ast, cls=AstEncoder, indent=2))


transformer = ASTTransformer(ast, context)
transformed_ast = transformer.transform()


print("\n--- AST After Transformation ---")
print(json.dumps(transformed_ast, cls=AstEncoder, indent=2))

# --- Verification ---
print("\n--- Verification ---")
final_div = transformed_ast.children[0]
final_p = final_div.children[0]
style_node = next((c for c in final_p.children if isinstance(c, StyleNode)), None)

if style_node:
    print("SUCCESS: StyleNode found in 'p' element.")
    print(f"Style content: {style_node.children[0].name}: {style_node.children[0].value}")
else:
    print("FAILURE: StyleNode NOT found in 'p' element.")
