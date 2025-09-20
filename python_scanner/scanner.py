from dataclasses import dataclass
from typing import List
import re

@dataclass
class CodeFragment:
    """Represents a snippet of code with a specific type."""
    type: str
    content: str

class UnifiedScanner:
    """
    Scans a source file and separates it into fragments of different types
    (e.g., CHTL, JS, CHTL_JS).
    """
    def __init__(self, source_code: str):
        self.source_code = source_code
        self.fragments: List[CodeFragment] = []
        self.cursor = 0

    def scan(self) -> List[CodeFragment]:
        """
        Performs the scanning of the source code.
        """
        while self.cursor < len(self.source_code):
            # Use a regex that is less greedy and finds the next script block
            script_match = re.search(r"script\s*{", self.source_code[self.cursor:])

            if not script_match:
                # No more script blocks, the rest is CHTL
                self._add_fragment("CHTL", self.source_code[self.cursor:])
                break

            # Add the CHTL part before the script block
            pre_script_content = self.source_code[self.cursor : self.cursor + script_match.start()]
            self._add_fragment("CHTL", pre_script_content)

            # Find the end of the script block
            script_content_start = self.cursor + script_match.end()
            script_content_end = self._find_matching_brace(script_content_start)

            if script_content_end == -1:
                # Unmatched brace, treat rest of file as CHTL and stop
                self._add_fragment("CHTL", self.source_code[self.cursor:])
                break

            # Extract and scan the script content
            script_content = self.source_code[script_content_start:script_content_end]
            self._scan_script_block(script_content)

            # Move cursor past the script block
            self.cursor = script_content_end + 1

        return self.fragments

    def _scan_script_block(self, script_content: str):
        """
        Scans the content of a script block to separate JS and CHTL_JS.

        This is a simplified implementation that demonstrates the basic "black box"
        placeholder mechanism described in CHTL.md. It currently identifies
        CHTL_JS code only by the '{{...}}' pattern.

        A full implementation would require a more sophisticated parser to recognize
        all CHTL_JS keywords (e.g., Listen, Animate, Vir) and their respective
        syntactical structures (e.g., balanced braces for blocks).
        """
        local_cursor = 0
        while local_cursor < len(script_content):
            start_match = script_content.find('{{', local_cursor)

            if start_match == -1:
                # No more CHTL_JS, the rest is JS
                self._add_fragment("JS", script_content[local_cursor:])
                break

            # Add the JS part before the '{{'
            self._add_fragment("JS", script_content[local_cursor:start_match])

            end_match = script_content.find('}}', start_match + 2)

            if end_match == -1:
                # Unmatched '{{', treat rest of script as JS and bail
                self._add_fragment("JS", script_content[start_match:])
                break

            end_pos = end_match + 2
            self._add_fragment("CHTL_JS", script_content[start_match:end_pos])
            local_cursor = end_pos

    def _find_matching_brace(self, start_pos: int) -> int:
        """Finds the matching closing brace for an opening brace at start_pos - 1."""
        brace_level = 1
        for i in range(start_pos, len(self.source_code)):
            if self.source_code[i] == '{':
                brace_level += 1
            elif self.source_code[i] == '}':
                brace_level -= 1

            if brace_level == 0:
                return i
        return -1 # Not found

    def _add_fragment(self, type: str, content: str):
        """
        Adds a new fragment if the content is not empty or does not consist
        solely of whitespace.
        """
        if content and not content.isspace():
            self.fragments.append(CodeFragment(type=type, content=content))
