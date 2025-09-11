import re
from typing import Dict
from CHTL.ast.nodes import DocumentNode, ElementNode, AttributeNode, TextNode, StyleNode, ScriptNode
from CHTL.css_expr.evaluator import ExpressionEvaluator

class HtmlGenerator:
    def __init__(self, registry: Dict):
        self.registry = registry

    def visit(self, node):
        """The main entry point for the visitor."""
        return node.accept(self)

    def _find_nodes(self, node, node_type):
        """Recursively finds all nodes of a specific type in the AST."""
        if isinstance(node, node_type):
            yield node

        children = []
        if hasattr(node, 'children'):
            children.extend(node.children)
        if hasattr(node, 'body'):
            children.extend(node.body)

        for child in children:
            yield from self._find_nodes(child, node_type)

    def _evaluate_properties(self, properties_dict: Dict) -> Dict[str, str]:
        """
        Evaluates a dictionary of {prop_name: ExprAST} into {prop_name: string_value}.
        Handles self-referential properties with a multi-pass approach.
        """
        evaluated_props = {}
        evaluator = ExpressionEvaluator(context=evaluated_props)

        props_to_eval = properties_dict.copy()
        for _ in range(len(props_to_eval) + 1):
            unevaluated = {}
            for name, expr_ast in props_to_eval.items():
                try:
                    evaluated_props[name] = evaluator.evaluate(expr_ast)
                except (KeyError, TypeError, ValueError):
                    unevaluated[name] = expr_ast

            if not unevaluated:
                break

            if len(unevaluated) == len(props_to_eval):
                break # Circular dependency or unresolvable reference

            props_to_eval = unevaluated

        for name in props_to_eval:
            evaluated_props[name] = "EVAL_ERROR"

        return evaluated_props

    def visit_documentnode(self, node: DocumentNode):
        """Visits the root document node and constructs the full HTML page."""
        all_style_nodes = list(self._find_nodes(node, StyleNode))
        all_script_nodes = list(self._find_nodes(node, ScriptNode))
        head_content, script_content = "", ""

        all_global_rules_str = []
        for style_node in all_style_nodes:
            for selector, properties in style_node.global_rules.items():
                final_selector = selector
                if style_node.contextual_selector and '&' in selector:
                    final_selector = re.sub(r'&', style_node.contextual_selector, selector)

                evaluated_props = self._evaluate_properties(properties)
                props_str = " ".join([f"{name}: {value};" for name, value in evaluated_props.items()])
                all_global_rules_str.append(f"  {final_selector} {{ {props_str} }}")

        if all_global_rules_str:
            unique_rules = sorted(list(set(all_global_rules_str)))
            head_content = f"<style>\n{''.join(unique_rules)}\n</style>"

        all_script_ids = {s.script_id for s in all_script_nodes}
        if all_script_ids:
            all_js = [self.registry.get(sid)['content'] for sid in all_script_ids if self.registry.get(sid)]
            if all_js:
                script_content = f"<script>\n{''.join(all_js)}\n</script>"

        body_content = "".join(self.visit(child) for child in node.children)

        return f"<!DOCTYPE html>\n<html>\n<head>\n{head_content}\n</head>\n<body>\n{body_content}\n{script_content}\n</body>\n</html>"

    def visit_elementnode(self, node: ElementNode):
        inline_properties = {}
        for child in node.children:
            if isinstance(child, StyleNode):
                inline_properties.update(child.inline_styles)

        evaluated_styles = self._evaluate_properties(inline_properties)
        style_str = "; ".join([f"{name}: {value}" for name, value in evaluated_styles.items() if value != 'EVAL_ERROR'])

        attrs = [self.visit(attr) for attr in node.attributes]
        if style_str:
            attrs.append(f'style="{style_str}"')

        attrs_str = f" {' '.join(attrs)}" if attrs else ""
        children_html = "".join(self.visit(child) for child in node.children if not isinstance(child, StyleNode))

        return f"<{node.tag_name}{attrs_str}>{children_html}</{node.tag_name}>"

    def visit_attributenode(self, node: AttributeNode):
        return f'{node.name}="{node.value}"'

    def visit_textnode(self, node: TextNode):
        return node.value

    def visit_stylenode(self, node: StyleNode):
        return ""

    def visit_scriptnode(self, node: ScriptNode):
        return ""

    def visit_originnode(self, node):
        """Visits an origin node and returns its raw content."""
        return node.content

    def visit_default(self, node):
        raise TypeError(f"No visit_{type(node).__name__.lower()} method for node type {type(node).__name__}")
