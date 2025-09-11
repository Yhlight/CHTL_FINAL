class CHTLUnifiedScanner:
    def __init__(self, source: str):
        self.source = source
        self.fragments = {}
        self.fragment_count = 0

    def scan(self) -> (str, dict):
        """
        Performs the scan using a manual loop to correctly find block boundaries.
        This version focuses on `script` blocks.
        """
        # We work on a list of characters for easier manipulation
        source_chars = list(self.source)
        i = 0
        while i < len(source_chars):
            # Check for the start of a 'script' block
            if "".join(source_chars[i:i+6]) == "script":
                # Find the opening brace
                block_start = -1
                for j in range(i + 6, len(source_chars)):
                    if source_chars[j] == '{':
                        block_start = j
                        break

                if block_start != -1:
                    # Find matching closing brace
                    brace_level = 1
                    content_start = block_start + 1
                    content_end = -1
                    for j in range(content_start, len(source_chars)):
                        if source_chars[j] == '{':
                            brace_level += 1
                        elif source_chars[j] == '}':
                            brace_level -= 1

                        if brace_level == 0:
                            content_end = j
                            break

                    if content_end != -1:
                        # Extract content and replace with placeholder
                        placeholder = self._get_placeholder("CHTLJS")
                        fragment_content = "".join(source_chars[content_start:content_end]).strip()
                        self.fragments[placeholder] = fragment_content

                        # Replace the original block with the placeholder
                        # The replacement is just the placeholder itself within the script block
                        replacement = list(f"script {{ {placeholder} }}")
                        source_chars = source_chars[:i] + replacement + source_chars[content_end+1:]

                        # Move index past the replaced block
                        i += len(replacement) -1

            i += 1

        return "".join(source_chars), self.fragments

    def _get_placeholder(self, language_prefix: str) -> str:
        placeholder = f"__{language_prefix}_FRAGMENT_{self.fragment_count}__"
        self.fragment_count += 1
        return placeholder
