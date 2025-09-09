from dataclasses import dataclass
from enum import Enum, auto
import re

class FragmentType(Enum):
    CHTL = auto()
    CHTL_JS = auto() # Not yet used, but for future
    CSS = auto()
    JS = auto() # Not yet used, but for future
    UNKNOWN = auto()

@dataclass
class CodeFragment:
    type: FragmentType
    content: str
    lineno: int

class CHTLUnifiedScanner:
    def __init__(self, source_code: str):
        self.source_code = source_code
        self.pos = 0
        self.lineno = 1
        self.fragments: list[CodeFragment] = []
        # A simple regex to find potential block starts.
        # This is an oversimplification and will be improved.
        self.block_pattern = re.compile(r'\b(style|script)\b\s*{')

    def scan(self) -> list[CodeFragment]:
        """
        Scans the source code and splits it into language-specific fragments.
        This implementation performs a basic block-level split for style and script.
        """
        last_pos = 0

        while last_pos < len(self.source_code):
            # Find the next potential `style {` or `script {`
            match = self.block_pattern.search(self.source_code, last_pos)

            if not match:
                # No more blocks found, the rest of the file is CHTL
                if last_pos < len(self.source_code):
                    content = self.source_code[last_pos:]
                    self._add_fragment(FragmentType.CHTL, content)
                break

            # Add the CHTL content before the matched block
            start_of_block_keyword = match.start()
            if start_of_block_keyword > last_pos:
                content = self.source_code[last_pos:start_of_block_keyword]
                if content.strip():
                    self._add_fragment(FragmentType.CHTL, content)

            # The block content starts after the opening brace
            block_type_str = match.group(1)
            content_start_pos = match.end()

            # Find the matching closing brace
            brace_level = 1
            i = content_start_pos
            while i < len(self.source_code):
                if self.source_code[i] == '{':
                    brace_level += 1
                elif self.source_code[i] == '}':
                    brace_level -= 1
                    if brace_level == 0:
                        break
                i += 1

            if brace_level != 0:
                # This is an error condition (unmatched brace)
                # For now, we'll treat the rest as CHTL and stop.
                content = self.source_code[last_pos:]
                self._add_fragment(FragmentType.CHTL, content)
                break

            content_end_pos = i
            block_content = self.source_code[content_start_pos:content_end_pos]

            # For now, we simplify: style blocks are CSS, script blocks are JS.
            # CHTL.md implies style blocks can contain CHTL features and script
            # blocks contain CHTL JS. This distinction will be handled by the
            # respective compilers, which will receive the full block content.
            # The dispatcher will route based on the block type.
            # Here, we'll just label them for the dispatcher.
            fragment_type = FragmentType.CSS if block_type_str == 'style' else FragmentType.JS

            # We include the keyword and braces in the fragment so the compiler
            # has the full context.
            full_block_content = self.source_code[start_of_block_keyword:content_end_pos + 1]
            self._add_fragment(fragment_type, full_block_content)

            last_pos = content_end_pos + 1

        return self.fragments

    def _add_fragment(self, type: FragmentType, content: str):
        # A proper implementation would track lineno precisely during the scan.
        # For now, we'll use a placeholder and add only non-empty fragments.
        if content and content.strip():
            self.fragments.append(
                CodeFragment(
                    type=type,
                    content=content,
                    lineno=0 # Placeholder, to be implemented later
                )
            )

    def _get_line_number_at_pos(self, pos: int) -> int:
        return self.source_code[:pos].count('\n') + 1
