from dataclasses import dataclass
from enum import Enum, auto
import re

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
                    frag_type = FragmentType.CSS if keyword == 'style' else FragmentType.JS
                    fragments.append(CodeFragment(frag_type, block_content, 0))

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
