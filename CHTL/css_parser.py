import re
from CHTL.ast.nodes import StyleUsageNode

def parse_style_content(css_text: str):
    """
    Separates inline properties, global rules, @Style usages, and delete statements
    from within a CHTL style block.
    """
    # Define patterns
    comment_pattern = re.compile(r'/\*.*?\*/', re.DOTALL)
    line_comment_pattern = re.compile(r'//.*')
    delete_pattern = re.compile(r'delete\s+([^;]+);')
    style_usage_pattern = re.compile(r'@Style\s+([a-zA-Z0-9_-]+);')
    rule_pattern = re.compile(r'([^;{}]+){(.*?)}', re.DOTALL)

    # 1. Strip comments
    text = comment_pattern.sub('', css_text)
    text = line_comment_pattern.sub('', text)

    # 2. Find and extract delete statements
    deleted_properties = []
    matches = delete_pattern.findall(text)
    for prop_list_str in matches:
        props = [p.strip() for p in prop_list_str.split(',')]
        deleted_properties.extend(props)
    text = delete_pattern.sub('', text)

    # 3. Find and extract @Style usages
    style_usages = []
    matches = style_usage_pattern.findall(text)
    for name in matches:
        style_usages.append(StyleUsageNode(name=name))
    text = style_usage_pattern.sub('', text)

    # 4. Find and extract global rules
    global_rules = []
    def rule_replacer(match):
        selector = match.group(1).strip()
        properties = match.group(2).strip()
        if selector and properties:
            global_rules.append(f"{selector} {{ {properties} }}")
        return ""
    text = rule_pattern.sub(rule_replacer, text)

    # 5. The remaining text is inline properties
    inline_properties_text = text.strip()

    return inline_properties_text, global_rules, style_usages, deleted_properties
