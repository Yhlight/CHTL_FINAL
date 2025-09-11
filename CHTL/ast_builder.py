from typing import Dict, Any
from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    ScriptNode, TemplateDefinitionNode, CustomDefinitionNode,
    UsageNode, ElementUsageNode, VarUsageNode,
    SpecializationNode, InsertStatementNode, DeleteStatementNode,
    DirectiveNode, ImportNode, NamespaceNode, ConfigurationNode
)
from CHTL.symbol_table import SymbolTable

class AstBuilder(CHTLVisitor):
    def __init__(self, registry: Dict[str, Any], symbol_table: SymbolTable):
        self.registry = registry
        self.symbol_table = symbol_table
        self.current_namespace = 'global'
        super().__init__()

    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        children = []
        if ctx.children:
            for child in ctx.children:
                if isinstance(child, (CHTLParser.DirectiveContext, CHTLParser.DefinitionContext, CHTLParser.ElementContext)):
                    node = self.visit(child)
                    if isinstance(node, (TemplateDefinitionNode, CustomDefinitionNode)):
                        self.symbol_table.define(node, namespace=self.current_namespace)
                    elif isinstance(node, (NamespaceNode, ImportNode, ConfigurationNode)):
                        # These are processed by the driver, not kept in the main tree
                        # We still need to visit them to populate symbol table etc.
                        children.append(node) # Add them to the tree for now
                    elif node:
                        children.append(node)
        return DocumentNode(children=children)

    def visitDirective(self, ctx:CHTLParser.DirectiveContext):
        # Delegate to the specific directive visitor
        return self.visit(ctx.getChild(0))

    def visitImportStatement(self, ctx:CHTLParser.ImportStatementContext):
        # Simplified: just get the path for now
        path_node = ctx.getChild(3)
        path = path_node.getText()
        if path.startswith('"') or path.startswith("'"):
            path = path[1:-1]
        return ImportNode(import_type='Chtl', path=path)

    def visitNamespaceStatement(self, ctx:CHTLParser.NamespaceStatementContext):
        # Simplified version
        name = ctx.IDENTIFIER().getText()
        return NamespaceNode(name=name, body=[])

    def visitConfigurationStatement(self, ctx:CHTLParser.ConfigurationStatementContext):
        name = ctx.IDENTIFIER().getText() if ctx.IDENTIFIER() else None
        settings = [self.visit(attr) for attr in ctx.attribute()]
        return ConfigurationNode(name=name, settings=settings)


    def visitDefinition(self, ctx:CHTLParser.DefinitionContext):
        node_data = self.visit(ctx.getChild(1))
        def_type, name, body = node_data
        if ctx.TEMPLATE():
            return TemplateDefinitionNode(def_type=def_type, name=name, body=body)
        elif ctx.CUSTOM():
            return CustomDefinitionNode(def_type=def_type, name=name, body=body)
        return None

    def visitStyleTemplate(self, ctx:CHTLParser.StyleTemplateContext):
        name = ctx.IDENTIFIER().getText()
        start = ctx.LBRACE().symbol.stop + 1
        stop = ctx.RBRACE().symbol.start - 1
        content = ""
        if start <= stop:
            content = ctx.start.getInputStream().getText(start, stop).strip()
        body = [TextNode(value=content)]
        return ('Style', name, body)

    def visitElementTemplate(self, ctx:CHTLParser.ElementTemplateContext):
        name = ctx.IDENTIFIER().getText()
        body = [self.visit(el) for el in ctx.element()]
        return ('Element', name, body)

    def visitVarTemplate(self, ctx:CHTLParser.VarTemplateContext):
        name = ctx.IDENTIFIER().getText()
        body = [self.visit(attr) for attr in ctx.attribute()]
        return ('Var', name, body)

    def visitElement(self, ctx:CHTLParser.ElementContext):
        tag_name = ctx.IDENTIFIER().getText()
        attributes = []
        children = []
        # The children of the element are inside the LBRACE and RBRACE.
        # The actual content starts at index 2.
        if len(ctx.children) > 3: # IDENTIFIER LBRACE ... RBRACE
            for i in range(2, len(ctx.children) - 1):
                child_ctx = ctx.getChild(i)
                # It's possible to get a TerminalNode here (e.g. for a SEMI), so we check the type
                if not hasattr(child_ctx, 'accept'):
                    continue
                node = self.visit(child_ctx)

                if isinstance(node, AttributeNode):
                    attributes.append(node)
                elif node:
                    children.append(node)
        return ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def visitElementUsage(self, ctx:CHTLParser.ElementUsageContext):
        name = ctx.IDENTIFIER().getText()
        # This rule was simplified in the grammar to debug a parser issue.
        # The body and from_namespace parts are temporarily removed.
        return ElementUsageNode(name=name, specializations=[], from_namespace=[])

    def visitSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        if ctx.insertStatement():
            return self.visit(ctx.insertStatement())
        elif ctx.deleteStatement():
            return self.visit(ctx.deleteStatement())
        return None

    def visitInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        elements = [self.visit(el) for el in ctx.element()]
        return InsertStatementNode(elements=elements)

    def visitDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
        tag_name = ctx.IDENTIFIER().getText()
        return DeleteStatementNode(tag_name=tag_name)

    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        name = ctx.IDENTIFIER().getText()
        value = self.visit(ctx.value())
        return AttributeNode(name=name, value=value)

    def visitTextNode(self, ctx:CHTLParser.TextNodeContext):
        value = self.visit(ctx.value())
        return TextNode(value=value)

    def visitStylePlaceholder(self, ctx:CHTLParser.StylePlaceholderContext):
        block_id = ctx.STRING().getText()[1:-1]
        style_data = self.registry.get(block_id)
        if not style_data or style_data.get('type') != 'style':
            return None
        return StyleNode(
            raw_content="",
            inline_styles=style_data.get('inline', ''),
            global_rules=style_data.get('global', []),
            style_usages=style_data.get('usages', []),
            deleted_properties=style_data.get('deleted', [])
        )

    def visitScriptPlaceholder(self, ctx:CHTLParser.ScriptPlaceholderContext):
        script_id = ctx.STRING().getText()[1:-1]
        return ScriptNode(script_id=script_id)

    def visitValue(self, ctx:CHTLParser.ValueContext):
        parts = [self.visit(child) for child in ctx.children]
        if len(parts) == 1 and isinstance(parts[0], VarUsageNode):
            return parts[0]
        str_parts = [str(p) if not isinstance(p, VarUsageNode) else f"{p.template_name}({p.var_name})" for p in parts]
        return "".join(str_parts)

    def visitValuePart(self, ctx:CHTLParser.ValuePartContext):
        if ctx.STRING():
            return ctx.STRING().getText()[1:-1]
        elif ctx.varUsage():
            return self.visit(ctx.varUsage())
        else:
            return ctx.getChild(0).getText()

    def visitVarUsage(self, ctx:CHTLParser.VarUsageContext):
        template_name = ctx.IDENTIFIER(0).getText()
        var_name = ctx.IDENTIFIER(1).getText()
        override_value = None
        if ctx.value():
            override_value = self.visit(ctx.value())
        return VarUsageNode(template_name=template_name, var_name=var_name, override_value=override_value)
