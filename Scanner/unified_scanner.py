import re

class CHTLUnifiedScanner:
    def __init__(self):
        self.fragments = {}
        self.fragment_count = 0

    def _get_placeholder(self, language_prefix: str) -> str:
        placeholder = f"__{language_prefix}_FRAGMENT_{self.fragment_count}__"
        self.fragment_count += 1
        return placeholder

    def scan(self, source: str) -> (str, dict):
        # This top-level scan finds `script` blocks and passes their content
        # to the fragment scanner. This logic is sound.
        self.source = source
        self.current_pos = 0
        self.output_source = []
        self.fragments = {}
        self.fragment_count = 0

        while self.current_pos < len(self.source):
            try:
                script_keyword_pos = self.source.index("script", self.current_pos)
                open_brace_pos = self.source.index('{', script_keyword_pos)
            except ValueError:
                self.output_source.append(self.source[self.current_pos:])
                break

            self.output_source.append(self.source[self.current_pos:open_brace_pos+1])

            brace_level = 1
            content_start = open_brace_pos + 1
            content_end = -1
            for i in range(content_start, len(self.source)):
                if self.source[i] == '{': brace_level += 1
                elif self.source[i] == '}': brace_level -= 1
                if brace_level == 0:
                    content_end = i
                    break

            if content_end != -1:
                script_content = self.source[content_start:content_end]
                processed_content, js_fragments = self.scan_script_fragment(script_content)
                self.fragments.update(js_fragments)
                self.output_source.append(processed_content)
                self.output_source.append(self.source[content_end])
                self.current_pos = content_end + 1
            else:
                self.output_source.append(self.source[open_brace_pos+1:])
                self.current_pos = len(self.source)

        return "".join(self.output_source), self.fragments

    def scan_script_fragment(self, fragment: str) -> (str, dict):
        # This function now uses a generic approach to find and replace JS callbacks
        # in any CHTL JS block that contains them.

        local_fragments = {}
        modified_fragment = fragment

        # A robust regex to find any JS function literal, including arrow functions.
        # It handles simple nested braces but is not fully recursive. It looks for
        # `function(...) { ... }` or `(...) => { ... }`.
        # This is a pragmatic simplification. A full JS parser would be overkill.
        function_regex = re.compile(
            r'(function\s*\([^)]*\)\s*\{((?:[^{}]|{[^{}]*})*)\}|'  # Regular function
            r'\([^)]*\)\s*=>\s*\{((?:[^{}]|{[^{}]*})*)\})'          # Arrow function with block body
        )

        def replacer(match):
            js_code = match.group(0)
            placeholder = self._get_placeholder("JS")
            local_fragments[placeholder] = js_code
            return placeholder

        modified_fragment = function_regex.sub(replacer, fragment)

        return modified_fragment, local_fragments
