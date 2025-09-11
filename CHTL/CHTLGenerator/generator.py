import re
import html
from CHTL.CHTLNode import nodes
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLLoader.file_loader import FileLoader

# Import the entire CHTL JS pipeline
from Scanner.unified_scanner import CHTLUnifiedScanner
from CHTL_JS.CHTLJSLexer.lexer import CHTLJSLexer
from CHTL_JS.CHTLJSParser.parser import CHTLJSParser
from CHTL_JS.CHTLJSGenerator.generator import CHTLJSGenerator

class Generator:
    def __init__(self):
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}
        self.templates = {}
        self.global_styles = []
        self.global_scripts = []
        self.ast_root = None
        self.file_loader = FileLoader()

    def generate(self, ast: nodes.ProgramNode) -> str:
        self.ast_root = ast
        self.templates = {}
        self.global_styles = []
        self.global_scripts = []
        self._collect_templates(ast)
        self._process_imports(ast)

        # Generate the main HTML structure
        html_output = self._visit(ast)

        # Inject collected styles
        if self.global_styles:
            style_content = "\n".join(sorted(self.global_styles))
            style_block = f"<style>\n{style_content}\n</style>"
            if "</head>" in html_output.lower():
                html_output = re.sub(r'</head>', f'{style_block}\n</head>', html_output, flags=re.IGNORECASE, count=1)
            else:
                html_output = style_block + "\n" + html_output

        # Inject collected scripts
        if self.global_scripts:
            script_content = "\n".join(self.global_scripts)
            script_block = f"<script>\n{script_content}\n</script>"
            if "</body>" in html_output.lower():
                 html_output = re.sub(r'</body>', f'{script_block}\n</body>', html_output, flags=re.IGNORECASE, count=1)
            else:
                html_output += "\n" + script_block

        return html_output


    def _process_imports(self, node):
        if isinstance(node, nodes.ImportNode) and node.import_type == "Chtl":
            source = self.file_loader.read(node.path)
            if source:
                tokens = Lexer(source).scan_tokens()
                self._collect_templates(Parser(tokens).parse())
        if hasattr(node, 'children'):
            for child in node.children:
                if child: self._process_imports(child)

    def _collect_templates(self, node):
        if isinstance(node, nodes.TemplateDefinitionNode):
            if node.template_type not in self.templates: self.templates[node.template_type] = {}
            self.templates[node.template_type][node.name] = node
        if hasattr(node, 'children'):
            for child in node.children:
                if child: self._collect_templates(child)

    def _visit(self, node, parent_element=None, selector_context=""):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n, pe, sc: "")
        return visitor(node, parent_element, selector_context)

    def _visit_programnode(self, node, parent_element, selector_context):
        children_to_render = [c for c in node.children if not isinstance(c, (nodes.TemplateDefinitionNode, nodes.ImportNode, nodes.CustomDefinitionNode))]
        return "".join(self._visit(child) for child in children_to_render)

    def _visit_elementnode(self, node, parent_element, selector_context):
        if node.tag_name.lower() == 'script':
            script_content_node = next((c for c in node.children if isinstance(c, nodes.TextNode)), None)
            if script_content_node:
                raw_script = script_content_node.value

                scanner = CHTLUnifiedScanner()
                processed_script, js_fragments = scanner.scan_script_fragment(raw_script)

                js_lexer = CHTLJSLexer(processed_script)
                js_tokens = js_lexer.scan_tokens()
                js_parser = CHTLJSParser(js_tokens)
                js_ast = js_parser.parse()

                js_generator = CHTLJSGenerator(js_fragments)
                final_js = js_generator.generate(js_ast)
                self.global_scripts.append(final_js)

            return ""

        style_node = next((c for c in node.children if isinstance(c, nodes.StyleNode)), None)
        if style_node:
            for child in style_node.children:
                if isinstance(child, nodes.StyleSelectorRuleNode):
                    selector = "".join(t.lexeme for t in child.selector_tokens)
                    if selector.startswith('.'):
                        class_attr = next((a for a in node.attributes if a.name == 'class'), None)
                        if not class_attr: node.attributes.append(nodes.AttributeNode(name='class', value=selector[1:]))
                        elif selector[1:] not in class_attr.value.split(): class_attr.value += f" {selector[1:]}"
                    elif selector.startswith('#'):
                        id_attr = next((a for a in node.attributes if a.name == 'id'), None)
                        if not id_attr: node.attributes.append(nodes.AttributeNode(name='id', value=selector[1:]))
                        else: id_attr.value = selector[1:]

        html_str = f"<{node.tag_name}"
        for attr in sorted(node.attributes, key=lambda x: x.name):
            html_str += f' {attr.name}="{html.escape(attr.value)}"'

        if style_node:
            all_props = [p for p in style_node.children if isinstance(p, nodes.StylePropertyNode)]
            template_usages = [u for u in style_node.children if isinstance(u, nodes.TemplateUsageNode)]
            for usage in template_usages:
                template_def = self.templates.get(usage.template_type, {}).get(usage.name)
                if template_def: all_props.extend(template_def.body)
            if all_props:
                style_strings = [f"{p.name}: {self._evaluate_expression(p.value_expression, node)}" for p in sorted(all_props, key=lambda x: x.name)]
                html_str += f' style="{html.escape("; ".join(style_strings))}"'

        if node.tag_name in self.self_closing_tags and not any(not isinstance(c, nodes.StyleNode) for c in node.children):
            return html_str + " />"
        html_str += ">"

        id_attr = next((a for a in node.attributes if a.name == 'id'), None)
        class_attr = next((a for a in node.attributes if a.name == 'class'), None)
        my_selector = f"#{id_attr.value}" if id_attr else f".{class_attr.value.split()[0]}" if class_attr else ""

        for child in node.children:
            if not isinstance(child, nodes.StyleNode):
                html_str += self._visit(child, node, my_selector)
        if style_node: self._visit_stylenode(style_node, node, my_selector)
        html_str += f"</{node.tag_name}>"
        return html_str

    def _visit_stylenode(self, node, parent_element, selector_context):
        last_full_selector = selector_context
        all_children = []
        for child in node.children:
            if isinstance(child, nodes.TemplateUsageNode):
                 template_def = self.templates.get(child.template_type, {}).get(child.name)
                 if template_def: all_children.extend(template_def.body)
            else: all_children.append(child)
        for child in all_children:
            if isinstance(child, nodes.StyleSelectorRuleNode):
                raw_selector = "".join(t.lexeme for t in child.selector_tokens)
                final_selector = (last_full_selector + raw_selector[1:]) if raw_selector.startswith('&') else f"{selector_context} {raw_selector}".strip()
                last_full_selector = final_selector
                prop_strings = [f"  {p.name}: {self._evaluate_expression(p.value_expression, parent_element)};" for p in sorted(child.properties, key=lambda x: x.name)]
                self.global_styles.append(f"{final_selector} {{\n" + "\n".join(prop_strings) + "\n}")

    def _visit_textnode(self, node, parent_element, selector_context):
        if parent_element and parent_element.tag_name.lower() == 'script':
            return node.value
        return html.escape(node.value.strip().strip('"\''))

    def _visit_originnode(self, node, parent_element, selector_context): return node.content

    def _evaluate_expression(self, node, current_element):
        value, unit = self._eval_recursive(node, current_element)
        if isinstance(value, float) and value == int(value): value = int(value)
        return f"{value}{unit or ''}"

    def _eval_recursive(self, node, current_element):
        if isinstance(node, nodes.LiteralNode):
            val_str = str(node.value)
            # This regex captures a number (int or float) and an optional unit.
            match = re.match(r'^(-?\d+\.?\d*)\s*(.*)$', val_str.strip())
            if match:
                number_part, unit_part = match.groups()
                return (float(number_part), unit_part or None)
            return (node.value, None) # It's just a string like 'solid' or 'blue'

        if isinstance(node, nodes.ValueWithUnitNode): return (node.value, node.unit)

        if isinstance(node, nodes.ReferenceNode):
            target_element = self._find_node(node.selector_tokens) if node.selector_tokens else current_element
            if not target_element: return (node.property_name, None)
            style_node = next((c for c in target_element.children if isinstance(c, nodes.StyleNode)), None)
            if not style_node: return (0, None)
            prop_node = next((p for p in style_node.children if isinstance(p, nodes.StylePropertyNode) and p.name == node.property_name), None)
            if not prop_node: return (0, None)
            return self._eval_recursive(prop_node.value_expression, target_element)

        if isinstance(node, nodes.ConditionalNode):
            cond_val, _ = self._eval_recursive(node.condition, current_element)
            if isinstance(cond_val, (int,float)): cond_val = cond_val != 0
            return self._eval_recursive(node.true_branch if cond_val else node.false_branch, current_element)

        if isinstance(node, nodes.BinaryOpNode):
            left_val, left_unit = self._eval_recursive(node.left, current_element)
            right_val, right_unit = self._eval_recursive(node.right, current_element)

            if left_unit and right_unit and left_unit != right_unit:
                return ("(error: incompatible units)", None)
            unit = left_unit or right_unit

            op = node.op.type
            if op == TokenType.PLUS: res = left_val + right_val
            elif op == TokenType.MINUS: res = left_val - right_val
            elif op == TokenType.STAR: res = left_val * right_val
            elif op == TokenType.SLASH: res = left_val / right_val if right_val != 0 else 0
            elif op == TokenType.STAR_STAR: res = left_val ** right_val
            elif op == TokenType.PERCENT: res = left_val % right_val if right_val != 0 else 0
            elif op == TokenType.GREATER: res = left_val > right_val
            elif op == TokenType.GREATER_EQUAL: res = left_val >= right_val
            elif op == TokenType.LESS: res = left_val < right_val
            elif op == TokenType.LESS_EQUAL: res = left_val <= right_val
            elif op == TokenType.EQUAL_EQUAL: res = left_val == right_val
            elif op == TokenType.BANG_EQUAL: res = left_val != right_val
            elif op == TokenType.AMPERSAND_AMPERSAND: res = left_val and right_val
            elif op == TokenType.PIPE_PIPE: res = left_val or right_val
            else: res = 0
            return (res, unit)
        return (None, None)

    def _find_node(self, selector_tokens: list) -> nodes.ElementNode:
        if not selector_tokens: return None
        selector_type = selector_tokens[0].type
        selector_name = selector_tokens[1].lexeme if len(selector_tokens) > 1 else ""
        def traverse(root):
            if isinstance(root, nodes.ElementNode):
                if selector_type == TokenType.HASH:
                    id_attr = next((a for a in root.attributes if a.name == 'id'), None)
                    if id_attr and id_attr.value == selector_name: return root
                elif selector_type == TokenType.DOT:
                    class_attr = next((a for a in node.attributes if a.name == 'class'), None)
                    if class_attr and selector_name in class_attr.value.split(): return root
            if hasattr(root, 'children'):
                for child in root.children:
                    if child and (found := traverse(child)): return found
            return None
        return traverse(self.ast_root)
