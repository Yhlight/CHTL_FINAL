import re
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Dict, List

class FragmentType(Enum):
    CHTL = auto()
    CHTL_JS = auto()
    CSS = auto()
    JS = auto()
    UNKNOWN = auto()

@dataclass
class CodeFragment:
    type: FragmentType
    content: str
    lineno: int
    placeholders: Dict[str, str] = field(default_factory=dict)

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

            # 1. Handle entering/exiting strings
            if (char == '"' or char == "'") and (i == 0 or self.source_code[i-1] != '\\'):
                if in_string == char:
                    in_string = None
                elif in_string is None:
                    in_string = char

            # 2. Process characters only if not inside a string
            if not in_string:
                if char == '{':
                    # Check for top-level block BEFORE incrementing brace_level
                    if brace_level == 0:
                        # Walk backwards to find the keyword
                        j = i - 1
                        while j >= 0 and self.source_code[j].isspace(): j -= 1
                        word_end = j + 1
                        while j >= 0 and not self.source_code[j].isspace(): j -= 1
                        word_start = j + 1
                        keyword = self.source_code[word_start:word_end]

                        if keyword in ('style', 'script'):
                            # Found a top-level block. Add preceding CHTL.
                            preceding_content = self.source_code[current_pos:word_start]
                            if preceding_content.strip():
                                fragments.append(CodeFragment(FragmentType.CHTL, preceding_content, 0))

                            # Find matching closing brace for the block
                            block_brace_level = 1
                            k = i + 1
                            while k < len(self.source_code):
                                if self.source_code[k] == '{': block_brace_level += 1
                                elif self.source_code[k] == '}': block_brace_level -= 1
                                if block_brace_level == 0: break
                                k += 1

                            block_end_pos = k
                            block_content = self.source_code[word_start : block_end_pos + 1]

                            placeholders = {}
                            if keyword == 'script':
                                frag_type = FragmentType.CHTL_JS
                                block_content, placeholders = self._process_script_block(block_content)
                            else:
                                frag_type = FragmentType.CSS

                            fragments.append(CodeFragment(frag_type, block_content, 0, placeholders))

                            # Advance main loop past this block
                            i = block_end_pos
                            current_pos = i + 1

                    brace_level += 1 # Increment brace level after check
                elif char == '}':
                    brace_level -= 1

            i += 1

        # Add final remaining content
        remaining_content = self.source_code[current_pos:]
        if remaining_content.strip():
            fragments.append(CodeFragment(FragmentType.CHTL, remaining_content, 0))

        if not fragments and self.source_code.strip():
            fragments.append(CodeFragment(FragmentType.CHTL, self.source_code, 0))

        return fragments

    def _process_script_block(self, content: str) -> (str, Dict[str, str]):
        """
        Finds standard JS functions in a script block and replaces them with placeholders.
        Returns the modified content and a map of placeholders to original code.
        """
        # This regex is a simplified example. It won't handle all edge cases like nested functions.
        function_pattern = re.compile(r'function\s+\w+\s*\(.*?\)')
        placeholders = {}
        modified_content = content

        # Iterate backwards to not mess up indices with replacements
        for match in reversed(list(function_pattern.finditer(content))):
            open_brace_index = content.find('{', match.end())
            if open_brace_index == -1:
                continue

            brace_level = 1
            k = open_brace_index + 1
            while k < len(content):
                if content[k] == '{': brace_level += 1
                elif content[k] == '}':
                    brace_level -= 1
                    if brace_level == 0: break
                k += 1

            if brace_level != 0: continue

            close_brace_index = k

            placeholder_id = f"__JS_PLACEHOLDER_{len(placeholders)}__"
            original_code = content[match.start():close_brace_index + 1]

            placeholders[placeholder_id] = original_code
            modified_content = modified_content.replace(original_code, placeholder_id, 1)

        return modified_content, placeholders
