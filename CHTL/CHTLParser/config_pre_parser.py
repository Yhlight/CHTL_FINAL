import re

class ConfigPreParser:
    """
    A simple, fast pre-parser to find, extract, and remove [Configuration]
    blocks from a raw CHTL source string. This runs before the main lexer.
    """
    def __init__(self, source_code: str):
        self.source_code = source_code

    def extract_configs(self) -> (list[str], str):
        """
        Finds all top-level [Configuration] blocks, returns their inner
        content, and returns the source code with the blocks removed.
        """
        config_contents = []
        cleaned_source = self.source_code

        search_pos = 0
        while True:
            # Find the start of a potential block in the *original* source
            start_index = self.source_code.find('[Configuration]', search_pos)
            if start_index == -1:
                break

            open_brace_index = self.source_code.find('{', start_index)
            if open_brace_index == -1:
                search_pos = start_index + 1
                continue

            brace_level = 1
            i = open_brace_index + 1
            while i < len(self.source_code):
                if self.source_code[i] == '{':
                    brace_level += 1
                elif self.source_code[i] == '}':
                    brace_level -= 1
                    if brace_level == 0:
                        break
                i += 1

            if brace_level != 0:
                search_pos = open_brace_index + 1
                continue

            close_brace_index = i

            # Extract the content and the full block text
            content = self.source_code[open_brace_index + 1 : close_brace_index]
            full_block_text = self.source_code[start_index : close_brace_index + 1]

            config_contents.append(content)

            # Remove the block from the source code for the main parser
            cleaned_source = cleaned_source.replace(full_block_text, '', 1)

            search_pos = close_brace_index + 1

        return config_contents, cleaned_source
