from CHTL.CHTLNode import BaseNode, ElementNode, TextNode, StyleNode
import html

class Generator:
    """
    The HTML Generator for the CHTL language. It takes an AST and produces
    an HTML string.
    """
    def __init__(self):
        # A set of HTML tags that are self-closing.
        self.self_closing_tags = {"br", "hr", "img", "input", "link", "meta"}
        # This list will store CssRule objects collected from the AST.
        self.global_css_rules = []

    def generate(self, ast_root: ElementNode) -> str:
        """
        The main entry point for generating HTML from the AST.
        It orchestrates the collection of global styles and the assembly of the final HTML document.
        """
        if not isinstance(ast_root, ElementNode) or ast_root.tag_name != "root":
            raise ValueError("AST root must be a root ElementNode.")

        # 1. Clear state from any previous runs and collect styles from the new AST.
        self.global_css_rules = []
        self._collect_global_styles(ast_root)

        # 2. Generate the body HTML from the AST's children.
        body_html = "".join(self._generate_node(child) for child in ast_root.children)

        # 3. Generate the global CSS string from the collected rules.
        css_string = self._generate_global_css_string()

        # 4. Assemble the final, complete HTML document.
        return f"""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CHTL Document</title>
    <style>
{css_string}
    </style>
</head>
<body>
{body_html}
</body>
</html>"""

    def _generate_global_css_string(self) -> str:
        """Formats the collected CssRule objects into a single, formatted CSS string."""
        if not self.global_css_rules:
            return ""

        rule_strings = []
        for rule in self.global_css_rules:
            prop_string = "\n".join([f"        {key}: {value};" for key, value in rule.properties.items()])
            rule_strings.append(f"    {rule.selector} {{\n{prop_string}\n    }}")

        return "\n".join(rule_strings)

    def _collect_global_styles(self, node: BaseNode):
        """
        Performs a depth-first traversal of the AST to find all StyleNodes
        and collect their CSS rules into the generator's state.
        """
        if isinstance(node, StyleNode):
            self.global_css_rules.extend(node.rules)

        # Recursively traverse children, if any.
        if hasattr(node, 'children'):
            for child in node.children:
                self._collect_global_styles(child)

    def _generate_node(self, node: BaseNode) -> str:
        """Dispatches to the correct generation method based on node type."""
        if isinstance(node, ElementNode):
            return self._generate_element_node(node)
        elif isinstance(node, TextNode):
            return self._generate_text_node(node)
        # StyleNodes are now handled in separate passes, so they produce no direct output here.
        elif isinstance(node, StyleNode):
            return ""
        else:
            return ""

    def _generate_attributes(self, node: ElementNode) -> str:
        """Generates the HTML attribute string from a node's attributes dict."""
        if not node.attributes:
            return ""

        parts = []
        for key, value in node.attributes.items():
            # Escape the attribute value to handle special characters like " and >
            escaped_value = html.escape(str(value), quote=True)
            parts.append(f'{key}="{escaped_value}"')

        # Return the parts joined by spaces, with a leading space
        return " " + " ".join(parts)

    def _generate_element_node(self, node: ElementNode) -> str:
        """Generates an HTML element from an ElementNode, processing inline styles and attributes."""
        tag_name = node.tag_name.lower()

        # Find any StyleNode children and process them for inline styles and auto-attributes.
        inline_styles = {}
        auto_classes = set()
        auto_ids = []
        for child in node.children:
            if isinstance(child, StyleNode):
                inline_styles.update(child.properties)
                for c in child.auto_classes_to_add:
                    auto_classes.add(c)
                auto_ids.extend(child.auto_ids_to_add)

        # Merge auto-classes with existing classes
        if auto_classes:
            existing_classes = set(node.attributes.get("class", "").split())
            all_classes = existing_classes.union(auto_classes)
            if all_classes:
                node.attributes["class"] = " ".join(sorted(list(all_classes)))

        # Handle auto-ids, giving precedence to an existing id
        if auto_ids and "id" not in node.attributes:
            node.attributes["id"] = auto_ids[0]

        # Merge inline styles with existing style attribute
        if inline_styles:
            style_string = "; ".join(f"{key}: {value}" for key, value in inline_styles.items())
            existing_style = node.attributes.get("style", "")
            if existing_style and not existing_style.strip().endswith(';'):
                existing_style += ";"
            final_style = f"{existing_style} {style_string}".strip()
            node.attributes["style"] = final_style

        # Generate the attribute string now that all styles and auto-attributes have been merged.
        attr_string = self._generate_attributes(node)

        if tag_name in self.self_closing_tags:
            return f"<{tag_name}{attr_string}>"

        # Generate children's HTML, excluding StyleNodes which have already been processed.
        children_html = "".join(
            self._generate_node(child) for child in node.children if not isinstance(child, StyleNode)
        )

        return f"<{tag_name}{attr_string}>{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        """Generates text content from a TextNode."""
        # Escape the text to prevent HTML injection vulnerabilities.
        return html.escape(node.value)
