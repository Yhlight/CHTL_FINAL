import re
import sys

class CHTLScanner:
    def __init__(self, source):
        self.source = source
        self.fragments = []
        self.cursor = 0
        self.chtl_js_keywords = ["Listen", "Delegate", "Animate", "Router", "Vir", "printMylove", "iNeverAway"]

    def scan(self):
        while self.cursor < len(self.source):
            self.scan_source()
        return self.fragments

    def scan_source(self):
        last_pos = self.cursor

        try:
            script_keyword_pos = self.source.index("script", self.cursor)
            opening_brace_pos = self.source.index('{', script_keyword_pos)
        except ValueError:
            # No more script blocks
            if last_pos < len(self.source):
                self.fragments.append(("CHTL", self.source[last_pos:]))
            self.cursor = len(self.source)
            return

        # Add the CHTL part before the script block, including "script {"
        self.fragments.append(("CHTL", self.source[last_pos : opening_brace_pos + 1]))
        self.cursor = opening_brace_pos + 1

        # Find matching closing brace
        brace_count = 1
        scan_pos = self.cursor
        while scan_pos < len(self.source) and brace_count > 0:
            if self.source[scan_pos] == '{':
                brace_count += 1
            elif self.source[scan_pos] == '}':
                brace_count -= 1
            scan_pos += 1

        if brace_count == 0:
            block_end = scan_pos - 1
            self.scan_script_content(block_end)
            self.fragments.append(("CHTL", self.source[block_end : block_end + 1])) # The closing brace
            self.cursor = block_end + 1
        else:
            # Unmatched brace
            self.fragments.append(("CHTL", self.source[self.cursor:]))
            self.cursor = len(self.source)

    def scan_script_content(self, block_end):
        # This is a simplified direct scanner, not using the placeholder model for clarity
        while self.cursor < block_end:
            # Find the next CHTL JS construct
            next_mustache_match = re.search(r"{{", self.source[self.cursor:block_end])
            next_mustache_pos = (self.cursor + next_mustache_match.start()) if next_mustache_match else float('inf')

            next_keyword_pos = float('inf')
            for keyword in self.chtl_js_keywords:
                try:
                    pos = self.source.index(keyword, self.cursor, block_end)
                    # Basic standalone word check
                    if (pos == 0 or not self.source[pos-1].isalnum()) and \
                       (pos + len(keyword) == len(self.source) or not self.source[pos + len(keyword)].isalnum()):
                        next_keyword_pos = min(next_keyword_pos, pos)
                except ValueError:
                    continue

            next_construct_pos = min(next_mustache_pos, next_keyword_pos)

            if next_construct_pos == float('inf'):
                # No more constructs, rest is JS
                if self.cursor < block_end:
                    self.fragments.append(("JS", self.source[self.cursor:block_end]))
                self.cursor = block_end
                break

            # JS before the construct
            if next_construct_pos > self.cursor:
                self.fragments.append(("JS", self.source[self.cursor:next_construct_pos]))

            self.cursor = next_construct_pos

            # Handle the construct
            if self.cursor == next_mustache_pos:
                end_mustache_pos = self.source.find("}}", self.cursor, block_end)
                if end_mustache_pos != -1:
                    after_mustache = end_mustache_pos + 2

                    # Check for ->Listen
                    listen_match = re.match(r"\s*->\s*Listen\s*{", self.source[after_mustache:block_end])
                    if listen_match:
                        # Find matching brace for listen
                        brace_count = 1
                        scan_pos = after_mustache + listen_match.end()
                        while scan_pos < block_end and brace_count > 0:
                            if self.source[scan_pos] == '{': brace_count += 1
                            elif self.source[scan_pos] == '}': brace_count -= 1
                            scan_pos += 1

                        if brace_count == 0:
                            self.fragments.append(("CHTL_JS", self.source[self.cursor:scan_pos]))
                            self.cursor = scan_pos
                            continue

                    # Fallback to simple mustache
                    self.fragments.append(("CHTL_JS", self.source[self.cursor:after_mustache]))
                    self.cursor = after_mustache
                else:
                    self.cursor += 2 # Skip unterminated {{
            else: # Keyword
                keyword_len = 0
                for kw in self.chtl_js_keywords:
                    if self.source.startswith(kw, self.cursor):
                        keyword_len = len(kw)
                        break

                match = re.match(r"\s*{", self.source[self.cursor + keyword_len : block_end])
                if match:
                    brace_count = 1
                    scan_pos = self.cursor + keyword_len + match.end()
                    while scan_pos < block_end and brace_count > 0:
                        if self.source[scan_pos] == '{': brace_count += 1
                        elif self.source[scan_pos] == '}': brace_count -= 1
                        scan_pos += 1
                    if brace_count == 0:
                        self.fragments.append(("CHTL_JS", self.source[self.cursor:scan_pos]))
                        self.cursor = scan_pos
                    else:
                        self.cursor += keyword_len
                else:
                    self.cursor += keyword_len


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python scanner.py <filename>")
        sys.exit(1)

    filepath = sys.argv[1]
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            source_code = f.read()
    except FileNotFoundError:
        print(f"Error: File not found at {filepath}")
        sys.exit(1)

    scanner = CHTLScanner(source_code)
    fragments = scanner.scan()

    print("--- Scan Results ---")
    for i, (frag_type, content) in enumerate(fragments):
        print(f"Fragment {i+1}:")
        print(f"  Type: {frag_type}")
        print(f"  Content: [[[[\n{content}\n]]]]")
        print("--------------------")
