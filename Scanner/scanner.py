import re
from dataclasses import dataclass
from typing import Dict, List, Tuple

@dataclass
class CodeFragment:
    type: str
    content: str

class UnifiedScanner:
    def __init__(self, source_code: str):
        self.source_code = source_code
        self.fragments: Dict[str, CodeFragment] = {}
        self.placeholder_count = 0

    def _generate_placeholder(self) -> str:
        self.placeholder_count += 1
        return f"__UNIFIED_SCANNER_PLACEHOLDER_{self.placeholder_count}__"

    def _find_and_extract_blocks(self, block_keyword: str, fragment_type: str):
        # Regex to find the start of a block, e.g., "style {" or "script {"
        # The negative lookbehind ensures it's not part of another word.
        pattern = re.compile(f'(?<!\\w){block_keyword}\\s*\\{{')

        while True:
            match = pattern.search(self.source_code)
            if not match:
                break

            # Find the matching closing brace by counting
            content_start_pos = match.end()
            brace_level = 1
            content_end_pos = -1
            for i in range(content_start_pos, len(self.source_code)):
                if self.source_code[i] == '{':
                    brace_level += 1
                elif self.source_code[i] == '}':
                    brace_level -= 1
                    if brace_level == 0:
                        content_end_pos = i
                        break

            if content_end_pos == -1:
                # Unterminated block, stop processing
                break

            block_start_pos = match.start()
            block_end_pos = content_end_pos + 1

            content = self.source_code[content_start_pos:content_end_pos]

            placeholder = self._generate_placeholder()
            self.fragments[placeholder] = CodeFragment(type=fragment_type, content=content.strip())

            self.source_code = self.source_code[:block_start_pos] + placeholder + self.source_code[block_end_pos:]

    def _find_and_extract_origin_blocks(self):
        pattern = re.compile(r'\[Origin\]\s*(@\w+)\s*(?:\w+\s*)?\{')

        while True:
            match = pattern.search(self.source_code)
            if not match:
                break

            lang_type_str = match.group(1).lower()
            if 'html' in lang_type_str: fragment_type = 'html'
            elif 'style' in lang_type_str: fragment_type = 'css'
            elif 'javascript' in lang_type_str: fragment_type = 'js'
            else: fragment_type = 'raw'

            content_start_pos = match.end()
            brace_level = 1
            content_end_pos = -1
            for i in range(content_start_pos, len(self.source_code)):
                if self.source_code[i] == '{': brace_level += 1
                elif self.source_code[i] == '}':
                    brace_level -= 1
                    if brace_level == 0:
                        content_end_pos = i
                        break

            if content_end_pos == -1: break

            block_start_pos = match.start()
            block_end_pos = content_end_pos + 1
            content = self.source_code[content_start_pos:content_end_pos]

            placeholder = self._generate_placeholder()
            self.fragments[placeholder] = CodeFragment(type=fragment_type, content=content.strip())

            self.source_code = self.source_code[:block_start_pos] + placeholder + self.source_code[block_end_pos:]

    def scan(self) -> Tuple[str, Dict[str, CodeFragment]]:
        self._find_and_extract_origin_blocks()
        self._find_and_extract_blocks('style', 'css')
        self._find_and_extract_blocks('script', 'js')

        return self.source_code, self.fragments
