from typing import Dict, Any, Tuple
from CHTL.css_parser import parse_style_content
import re

class UnifiedScanner:
    """
    Pre-processes raw CHTL source code to separate different language fragments
    (like style blocks) before the main ANTLR parser runs.
    """
    def __init__(self):
        """
        Initializes the scanner.
        """
        self.registry: Dict[str, Any] = {}
        self._next_id = 0

    def _generate_id(self) -> str:
        """Generates a unique ID for a replaced block."""
        self._next_id += 1
        return f"__chtl_ref_{self._next_id}"

    def scan(self, source_text: str) -> Tuple[str, Dict[str, Any]]:
        """
        Scans the source text, extracts complex blocks (like style blocks),
        replaces them with placeholders, and returns the modified source
        along with a registry of the extracted data.
        """
        modified_source = ""
        last_index = 0

        i = 0
        while i < len(source_text):
            match = re.search(r'\bstyle\s*{', source_text[i:])
            if not match:
                break

            start_index = i + match.start()
            brace_index = i + match.end() - 1

            brace_depth = 1
            content_start = brace_index + 1
            content_end = -1
            for j in range(content_start, len(source_text)):
                if source_text[j] == '{':
                    brace_depth += 1
                elif source_text[j] == '}':
                    brace_depth -= 1
                    if brace_depth == 0:
                        content_end = j
                        break

            if content_end != -1:
                # Append the text before the style block
                modified_source += source_text[last_index:start_index]

                block_content = source_text[content_start:content_end]
                block_id = self._generate_id()

                inline_styles, global_rules = parse_style_content(block_content)

                self.registry[block_id] = {
                    'type': 'style',
                    'inline': inline_styles,
                    'global': global_rules
                }

                # Append the placeholder to the modified source
                modified_source += f'__style_ref__("{block_id}");'

                # Update indices
                i = content_end + 1
                last_index = i
            else:
                i = brace_index + 1

        # Append any remaining text after the last style block
        modified_source += source_text[last_index:]

        return modified_source, self.registry
