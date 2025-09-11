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
                processed_content, js_fragments = self._scan_script_fragment(script_content)
                self.fragments.update(js_fragments)
                self.output_source.append(processed_content)
                self.output_source.append(self.source[content_end])
                self.current_pos = content_end + 1
            else:
                self.output_source.append(self.source[open_brace_pos+1:])
                self.current_pos = len(self.source)

        return "".join(self.output_source), self.fragments

    def scan_script_fragment(self, fragment: str) -> (str, dict):
        local_fragments = {}
        # Manual scan for `listen` blocks
        try:
            listen_pos = fragment.index("listen")
            open_brace_pos = fragment.index('{', listen_pos)
        except ValueError:
            return fragment, local_fragments

        # Extract content inside listen { ... }
        brace_level = 1
        content_start = open_brace_pos + 1
        content_end = -1
        for i in range(content_start, len(fragment)):
            if fragment[i] == '{': brace_level += 1
            elif fragment[i] == '}': brace_level -= 1
            if brace_level == 0:
                content_end = i
                break

        if content_end == -1: return fragment, local_fragments # Unterminated

        listen_content = fragment[content_start:content_end]

        # Now, process the content to replace JS callbacks
        # This is a simplified parser for `key: value, key2: value2`
        modified_content = []
        i = 0
        while i < len(listen_content):
            try:
                colon_pos = listen_content.index(':', i)
                modified_content.append(listen_content[i:colon_pos+1])

                # Find end of callback (comma or end of block)
                brace_level = 0
                callback_start = colon_pos + 1
                callback_end = len(listen_content)
                for j in range(callback_start, len(listen_content)):
                    char = listen_content[j]
                    if char == '{': brace_level += 1
                    elif char == '}': brace_level -= 1
                    elif char in [',', ';'] and brace_level == 0:
                        callback_end = j
                        break

                js_code = listen_content[callback_start:callback_end].strip()
                placeholder = self._get_placeholder("JS")
                local_fragments[placeholder] = js_code
                modified_content.append(f" {placeholder}")
                if callback_end < len(listen_content):
                    modified_content.append(listen_content[callback_end]) # Append the comma/semicolon

                i = callback_end + 1
            except ValueError:
                modified_content.append(listen_content[i:])
                break

        processed_content = "".join(modified_content)
        final_fragment = fragment.replace(listen_content, processed_content)
        return final_fragment, local_fragments
