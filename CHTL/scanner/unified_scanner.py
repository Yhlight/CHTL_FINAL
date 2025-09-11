from typing import Dict, Any, Tuple
from CHTL.css_parser import parse_style_content
import re

class UnifiedScanner:
    def __init__(self):
        self.registry: Dict[str, Any] = {}
        self._next_id = 0

    def _generate_id(self) -> str:
        self._next_id += 1
        return f"__chtl_ref_{self._next_id}"

    def scan(self, source_text: str) -> Tuple[str, Dict[str, Any]]:
        # This regex will find 'style', 'script', or '[Origin]' as whole words/tokens
        block_pattern = re.compile(r'\b(style|script)\s*\{|(\[Origin\])')

        modified_source = ""
        last_index = 0

        while True:
            match = block_pattern.search(source_text, last_index)
            if not match:
                break

            start_index = match.start()

            # Determine block type and find the opening brace
            if match.group(1): # Matched 'style' or 'script'
                block_type = match.group(1)
                brace_index = source_text.find('{', match.end(1))
            elif match.group(2): # Matched '[Origin]'
                block_type = 'origin'
                brace_index = source_text.find('{', match.end(2))
            else:
                last_index = match.end()
                continue

            if brace_index == -1:
                # Malformed block, skip
                last_index = match.end()
                continue

            # Append the text before the block
            modified_source += source_text[last_index:start_index]

            # Find the matching closing brace
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
                block_content = source_text[content_start:content_end]
                block_id = self._generate_id()

                if block_type == 'style':
                    style_data = parse_style_content(block_content)
                    self.registry[block_id] = {
                        'type': 'style',
                        'inline': style_data.get('inline', ''),
                        'global': style_data.get('global', []),
                        'usages': style_data.get('usages', []),
                        'deleted': style_data.get('deleted', []),
                        'auto_classes': style_data.get('auto_classes', []),
                        'auto_ids': style_data.get('auto_ids', [])
                    }
                    modified_source += f'__style_ref__("{block_id}");'

                elif block_type == 'script':
                    self.registry[block_id] = {
                        'type': 'script',
                        'content': block_content
                    }
                    modified_source += f'__script_ref__("{block_id}");'

                elif block_type == 'origin':
                    # Extract the origin type (e.g., @Html)
                    type_text = source_text[match.end(2):brace_index].strip()
                    # A simple regex to grab the @Type part.
                    type_match = re.match(r'(@[a-zA-Z]+)', type_text)
                    origin_type = type_match.group(1) if type_match else '@Html' # Default to Html

                    self.registry[block_id] = {
                        'type': 'origin',
                        'origin_type': origin_type,
                        'content': block_content
                    }
                    modified_source += f'__origin_ref__("{block_id}");'

                last_index = content_end + 1
            else:
                # Unterminated block, just append the keyword and move on
                modified_source += source_text[last_index:brace_index + 1]
                last_index = brace_index + 1

        modified_source += source_text[last_index:]

        return modified_source, self.registry
