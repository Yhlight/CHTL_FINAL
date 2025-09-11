import re

def parse_style_content(css_text: str):
    """
    Separates inline properties from global rules within a CHTL style block
    using a simple state-machine parser. This is more robust than a regex
    for handling nested structures and mixed content.

    Args:
        css_text: The raw string content from a StyleNode.

    Returns:
        A tuple containing:
        - A string of inline style properties.
        - A list of strings, where each string is a full global CSS rule.
    """
    # First, strip out comments to simplify parsing.
    comment_pattern = re.compile(r'/\*.*?\*/', re.DOTALL)
    text = comment_pattern.sub('', css_text)

    inline_properties = []
    global_rules = []

    i = 0
    last_split = 0
    brace_depth = 0

    while i < len(text):
        char = text[i]
        if char == '{':
            if brace_depth == 0:
                # Found the start of a rule block. The text since the last
                # split is the selector. The text before that is an inline property.
                selector_part = text[last_split:i].strip()

                # Anything before the selector part is an inline property
                parts = selector_part.split(';')
                if len(parts) > 1:
                    inline_properties.extend([p.strip() + ';' for p in parts[:-1] if p.strip()])

                selector = parts[-1]
                last_split = i # The start of the block content

            brace_depth += 1
        elif char == '}':
            if brace_depth > 0:
                brace_depth -= 1
                if brace_depth == 0:
                    # End of a rule block
                    block_content = text[last_split + 1:i].strip()
                    full_rule = f"{selector} {{ {block_content} }}"
                    global_rules.append(full_rule)
                    last_split = i + 1
        i += 1

    # The remaining text contains only inline properties
    remaining_text = text[last_split:].strip()
    if remaining_text:
        # Split by semicolon and filter out empty strings
        props = [p.strip() for p in remaining_text.split(';') if p.strip()]
        inline_properties.extend([p + ';' for p in props])

    return " ".join(inline_properties), global_rules
