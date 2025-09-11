from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import DocumentNode, ElementNode, AttributeNode, TextNode

class AstBuilder(CHTLVisitor):

    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        children = [self.visit(element) for element in ctx.element()]
        return DocumentNode(children=children)

    def visitElement(self, ctx:CHTLParser.ElementContext):
        tag_name = ctx.IDENTIFIER().getText()

        attributes = []
        children = []

        if ctx.children:
            for child_ctx in ctx.children:
                if isinstance(child_ctx, CHTLParser.AttributeContext):
                    attributes.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.ElementContext):
                    children.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.TextNodeContext):
                    children.append(self.visit(child_ctx))

        return ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        name = ctx.IDENTIFIER().getText()
        value = self.visit(ctx.value())
        return AttributeNode(name=name, value=value)

    def visitTextNode(self, ctx:CHTLParser.TextNodeContext):
        value = self.visit(ctx.value())
        return TextNode(value=value)

    def visitValue(self, ctx:CHTLParser.ValueContext):
        if ctx.STRING():
            # Strip quotes from string literals
            return ctx.STRING().getText()[1:-1]

        # Handle a mix of IDENTIFIER and TEXT (and other future) tokens
        parts = []
        if ctx.children:
            for child in ctx.children:
                # The children are TerminalNodeImpl, we can get their text
                parts.append(child.getText())

        if parts:
            return " ".join(parts)

        return "" # Return empty string for empty values
