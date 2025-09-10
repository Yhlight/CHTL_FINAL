import re
from dataclasses import dataclass
from enum import Enum, auto

class FragmentType(Enum):
    CHTL = auto()
    CHTL_JS = auto()
    CSS = auto()
    JS = auto()
    UNKNOWN = auto()

from typing import Dict, List

@dataclass
class CodeFragment:
    type: FragmentType
    content: str
    lineno: int
    placeholders: Dict[str, str] = None

class CHTLUnifiedScanner:
    def __init__(self, source_code: str):
        self.source_code = source_code

    def scan(self) -> list[CodeFragment]:
        fragments = []
        current_pos = 0
        brace_level = 0
        in_string = None

        i = 0
        while i < len(self.source_code):
            char = self.source_code[i]

            # Before anything else, check if we are at the start of a potential top-level block
            if char == '{' and brace_level == 0:
                # Walk backwards to find the keyword
                j = i - 1
                while j >= 0 and self.source_code[j].isspace():
                    j -= 1
                word_end = j + 1
                while j >= 0 and not self.source_code[j].isspace():
                    j -= 1
                word_start = j + 1

                keyword = self.source_code[word_start:word_end]

                if keyword in ('style', 'script'):
                    # Found a top-level block. Add the preceding CHTL content.
                    preceding_content = self.source_code[current_pos:word_start]
                    if preceding_content.strip():
                        fragments.append(CodeFragment(FragmentType.CHTL, preceding_content, 0))

                    # Now, find the end of this block by matching braces
                    block_start_pos = i
                    block_brace_level = 1
                    k = i + 1
                    while k < len(self.source_code):
                        if self.source_code[k] == '{': block_brace_level += 1
                        elif self.source_code[k] == '}': block_brace_level -= 1
                        if block_brace_level == 0: break
                        k += 1

                    block_end_pos = k
                    block_content = self.source_code[word_start : block_end_pos + 1]

                    placeholders = None
                    if keyword == 'script':
                        frag_type = FragmentType.CHTL_JS
                        block_content, placeholders = self._process_script_block(block_content)
                    else:
                        frag_type = FragmentType.CSS

                    fragments.append(CodeFragment(frag_type, block_content, 0, placeholders))

                    # Move main cursor past this block
                    i = block_end_pos
                    current_pos = i + 1
                    # Continue to the next character in the main loop
                    i += 1
                    continue

            # If not a top-level block, just do normal brace counting and string checking
            if (char == '"' or char == "'") and (i == 0 or self.source_code[i-1] != '\\'):
                if in_string == char:
                    in_string = None
                elif in_string is None:
                    in_string = char

            if not in_string:
                if char == '{':
                    brace_level += 1
                elif char == '}':
                    brace_level -= 1

            i += 1

        # Add any final remaining CHTL content
        remaining_content = self.source_code[current_pos:]
        if remaining_content.strip():
            fragments.append(CodeFragment(FragmentType.CHTL, remaining_content, 0))

        # If the file was just one big CHTL block (or empty), handle that.
        if not fragments and self.source_code.strip():
            fragments.append(CodeFragment(FragmentType.CHTL, self.source_code, 0))

        return fragments

    def _process_script_block(self, content: str) -> (str, Dict[str, str]):
        """
        Finds standard JS functions in a script block and replaces them with placeholders.
        Returns the modified content and a map of placeholders to original code.
        """
        # This regex is a simplified example. It won't handle all edge cases.
        function_pattern = re.compile(r'function\s+\w+\s*\(.*?\)\s*\{[\s\S]*?\}')
        placeholders = {}
        modified_content = content

        matches = list(function_pattern.finditer(content))

        # Iterate backwards to not mess up indices with replacements
        for i, match in enumerate(reversed(matches)):
            placeholder_id = f"__JS_PLACEHOLDER_{len(matches) - 1 - i}__"
            original_code = match.group(0)

            placeholders[placeholder_id] = original_code

            # Replace the function in the content string
            start, end = match.span()
            modified_content = modified_content[:start] + placeholder_id + modified_content[end:]

        return modified_content, placeholders
