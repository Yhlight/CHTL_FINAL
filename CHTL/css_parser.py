import re
from CHTL.ast.nodes import StyleUsageNode

def parse_style_content(css_text: str):
    """
    Separates inline properties, global rules, and @Style usages
    from within a CHTL style block.
    """
    # Pattern for C-style block comments
    block_comment_pattern = re.compile(r'/\*.*?\*/', re.DOTALL)
    # Pattern for C++-style line comments
    line_comment_pattern = re.compile(r'//.*')
    # Pattern for @Style usage, e.g., "@Style MyStyles;"
    style_usage_pattern = re.compile(r'@Style\s+([a-zA-Z0-9_-]+);')
    # Pattern for global rule blocks
    rule_pattern = re.compile(r'([^;{}]+){(.*?)}', re.DOTALL)

    # 1. Strip all comments first
    text = block_comment_pattern.sub('', css_text)
    text = line_comment_pattern.sub('', text)

    # 2. Find and extract @Style usages
    style_usages = []
    matches = style_usage_pattern.findall(text)
    for name in matches:
        style_usages.append(StyleUsageNode(name=name))
    # Remove the usage statements from the text
    text = style_usage_pattern.sub('', text)

    # 3. Find and extract global rules
    global_rules = []
    def rule_replacer(match):
        selector = match.group(1).strip()
        properties = match.group(2).strip()
        if selector and properties:
            global_rules.append(f"{selector} {{ {properties} }}")
        return ""
    text = rule_pattern.sub(rule_replacer, text)

    # 4. The remaining text is inline properties
    inline_properties_text = text.strip()

    return inline_properties_text, global_rules, style_usages
