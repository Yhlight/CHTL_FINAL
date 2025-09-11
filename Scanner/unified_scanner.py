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
        local_fragments = {}
        output_parts = []
        cursor = 0

        block_start_regex = re.compile(r'\s*(listen|animate)\s*\{')

        while cursor < len(fragment):
            match = block_start_regex.search(fragment, cursor)
            if not match:
                output_parts.append(fragment[cursor:])
                break

            output_parts.append(fragment[cursor:match.start()])
            block_type = match.group(1)

            open_brace_pos = match.end() - 1
            brace_level = 1
            content_start = open_brace_pos + 1
            content_end = -1

            for i in range(content_start, len(fragment)):
                if fragment[i] == '{': brace_level += 1
                elif fragment[i] == '}': brace_level -= 1
                if brace_level == 0:
                    content_end = i
                    break

            if content_end == -1:
                output_parts.append(fragment[match.start():])
                break

            inner_content = fragment[content_start:content_end]

            if block_type == 'listen':
                js_code = inner_content.strip()
                if not js_code:
                    output_parts.append(match.group(0) + inner_content + '}')
                else:
                    placeholder = self._get_placeholder("JS")
                    local_fragments[placeholder] = js_code
                    output_parts.append(f'{match.group(0).rstrip()} {placeholder} }}')

            elif block_type == 'animate':
                try:
                    callback_key_pos = inner_content.index("callback")
                    colon_pos = inner_content.index(":", callback_key_pos)
                    val_start_pos = colon_pos + 1

                    # Simplified logic: Assume callback is the last property
                    js_code = inner_content[val_start_pos:].strip()

                    if not js_code:
                        output_parts.append(match.group(0) + inner_content + '}')
                    else:
                        placeholder = self._get_placeholder("JS")
                        local_fragments[placeholder] = js_code

                        # Reconstruct the animate block content carefully
                        pre_callback_part = inner_content[:val_start_pos]
                        modified_inner_content = pre_callback_part + " " + placeholder
                        output_parts.append(f'animate {{ {modified_inner_content} }}')

                except ValueError:
                    output_parts.append(match.group(0) + inner_content + '}')

            cursor = content_end + 1

        return "".join(output_parts), local_fragments
