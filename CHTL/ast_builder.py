from typing import Dict, Any
from CHTL.generated.CHTLVisitor import CHTLVisitor
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, StyleUsageNode,
    ScriptNode, TemplateDefinitionNode, CustomDefinitionNode,
    UsageNode, ElementUsageNode, VarUsageNode,
    SpecializationNode, InsertStatementNode, DeleteStatementNode, SelectorNode, OriginNode,
    DirectiveNode, ImportNode, ConfigurationNode
)
from CHTL.symbol_table import SymbolTable

class AstBuilder(CHTLVisitor):
    def __init__(self, registry: Dict[str, Any], symbol_table: SymbolTable):
        self.registry = registry
        self.symbol_table = symbol_table
        self.current_namespace = 'global'
        self.current_contextual_selector = None
        super().__init__()

    def visitDocument(self, ctx:CHTLParser.DocumentContext):
        children = []
        if ctx.children:
            for child in ctx.children:
                node = self.visit(child)
                # Some nodes like Namespace are for state change and don't return an AST node
                if node is None:
                    continue

                # Definitions are added to the symbol table, not the tree directly
                if isinstance(node, (TemplateDefinitionNode, CustomDefinitionNode)):
                    self.symbol_table.define(node, namespace=self.current_namespace)
                else:
                    children.append(node)
        return DocumentNode(children=children)

    def visitDirective(self, ctx:CHTLParser.DirectiveContext):
        # Delegate to the specific directive visitor
        return self.visit(ctx.getChild(0))

    def visitImportStatement(self, ctx:CHTLParser.ImportStatementContext):
        import_type = ctx.importType().getText()
        path = ctx.path().getText()
        if path.startswith('"') or path.startswith("'"):
            path = path[1:-1]

        alias = None
        if ctx.IDENTIFIER():
            alias = ctx.IDENTIFIER().getText()

        return ImportNode(import_type=import_type, path=path, alias=alias)

    def visitNamespaceStatement(self, ctx:CHTLParser.NamespaceStatementContext):
        name = ctx.IDENTIFIER().getText()
        # This directive changes the state of the builder for all subsequent definitions
        self.current_namespace = name
        # It does not produce a node in the final AST, it's a compiler directive
        return None

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
        body = [self.visit(attr) for attr in ctx.attribute()]
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

        # --- Stage 1: Gather all potential attributes and style info ---

        user_attributes = []
        style_placeholders = []
        other_child_contexts = []

        if len(ctx.children) > 3:
            for i in range(2, len(ctx.children) - 1):
                child_ctx = ctx.getChild(i)
                if isinstance(child_ctx, CHTLParser.AttributeContext):
                    user_attributes.append(self.visit(child_ctx))
                elif isinstance(child_ctx, CHTLParser.StylePlaceholderContext):
                    style_placeholders.append(child_ctx)
                elif hasattr(child_ctx, 'accept'):
                    other_child_contexts.append(child_ctx)

        # --- Stage 2: Determine final attributes and contextual selector ---

        auto_classes = []
        auto_ids = []
        for placeholder_ctx in style_placeholders:
            block_id = placeholder_ctx.STRING().getText()[1:-1]
            style_data = self.registry.get(block_id, {})
            auto_classes.extend(style_data.get('auto_classes', []))
            auto_ids.extend(style_data.get('auto_ids', []))

        final_attributes = list(user_attributes)
        has_id = any(attr.name == 'id' for attr in user_attributes)
        has_class = any(attr.name == 'class' for attr in user_attributes)

        if auto_ids and not has_id:
            final_attributes.append(AttributeNode(name='id', value=auto_ids[0]))

        if auto_classes:
            if not has_class:
                final_attributes.append(AttributeNode(name='class', value=' '.join(sorted(list(set(auto_classes))))))
            else:
                for attr in final_attributes:
                    if attr.name == 'class':
                        existing = set(attr.value.split())
                        existing.update(auto_classes)
                        attr.value = ' '.join(sorted(list(existing)))
                        break

        # Determine the selector for '&' replacement, ID takes precedence
        new_contextual_selector = None
        final_id_attr = next((attr for attr in final_attributes if attr.name == 'id'), None)
        if final_id_attr:
            new_contextual_selector = f"#{final_id_attr.value}"
        else:
            final_class_attr = next((attr for attr in final_attributes if attr.name == 'class'), None)
            if final_class_attr:
                # Use the first class name for the context
                new_contextual_selector = f".{final_class_attr.value.split()[0]}"

        # --- Stage 3: Build the final node and its children ---

        previous_selector = self.current_contextual_selector
        self.current_contextual_selector = new_contextual_selector

        children = []
        # Visit the style placeholders first, now with the correct context
        for placeholder_ctx in style_placeholders:
            children.append(self.visit(placeholder_ctx))

        # Visit the rest of the children
        for child_ctx in other_child_contexts:
            node = self.visit(child_ctx)
            if node:
                children.append(node)

        self.current_contextual_selector = previous_selector # Restore context

        return ElementNode(tag_name=tag_name, attributes=final_attributes, children=children)

    def visitElementUsage(self, ctx:CHTLParser.ElementUsageContext):
        name = ctx.IDENTIFIER().getText()

        from_namespace = []
        if ctx.namespacePath():
            from_namespace = [ident.getText() for ident in ctx.namespacePath().IDENTIFIER()]

        specializations = []
        if ctx.LBRACE():
            for spec_body in ctx.specializationBody():
                spec_node = self.visit(spec_body)
                if spec_node:
                    specializations.append(spec_node)

        return ElementUsageNode(
            name=name,
            specializations=specializations,
            from_namespace=from_namespace,
            current_namespace=self.current_namespace
        )

    def visitSpecializationBody(self, ctx:CHTLParser.SpecializationBodyContext):
        if ctx.insertStatement():
            return self.visit(ctx.insertStatement())
        elif ctx.deleteStatement():
            return self.visit(ctx.deleteStatement())
        return None

    def visitSelector(self, ctx:CHTLParser.SelectorContext):
        tag_name = ctx.IDENTIFIER().getText()
        index = None
        if ctx.NUMBER():
            index = int(ctx.NUMBER().getText())
        return SelectorNode(tag_name=tag_name, index=index)

    def visitInsertStatement(self, ctx:CHTLParser.InsertStatementContext):
        elements = [self.visit(el) for el in ctx.element()]
        position = 'at_bottom' # Default behavior if no position is specified
        target_selector = None

        if ctx.insertPosition():
            pos_ctx = ctx.insertPosition()
            if pos_ctx.selector():
                target_selector = self.visit(pos_ctx.selector())
                position = pos_ctx.getChild(0).getText() # AFTER, BEFORE, or REPLACE
            elif pos_ctx.AT():
                # The child at index 1 will be either TOP or BOTTOM
                position = f"at_{pos_ctx.getChild(1).getText()}"

        return InsertStatementNode(elements=elements, position=position, target_selector=target_selector)

    def visitDeleteStatement(self, ctx:CHTLParser.DeleteStatementContext):
        selector = self.visit(ctx.selector())
        return DeleteStatementNode(target_selector=selector)

    def visitAttribute(self, ctx:CHTLParser.AttributeContext):
        name = ctx.IDENTIFIER().getText()
        value = None
        if ctx.value():
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
            inline_styles=style_data.get('inline', {}),
            global_rules=style_data.get('global', {}),
            style_usages=style_data.get('usages', []),
            deleted_properties=style_data.get('deleted', []),
            contextual_selector=self.current_contextual_selector
        )

    def visitScriptPlaceholder(self, ctx:CHTLParser.ScriptPlaceholderContext):
        script_id = ctx.STRING().getText()[1:-1]
        return ScriptNode(script_id=script_id)

    def visitOriginPlaceholder(self, ctx:CHTLParser.OriginPlaceholderContext):
        block_id = ctx.STRING().getText()[1:-1]
        origin_data = self.registry.get(block_id)
        if not origin_data or origin_data.get('type') != 'origin':
            return None

        return OriginNode(
            origin_type=origin_data.get('origin_type', '@Html'),
            content=origin_data.get('content', '')
        )

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
