import re
from typing import List, Dict, Optional, Tuple

class ConfigPreParser:
    """
    A simple, fast pre-parser to find/extract/remove [Configuration] blocks
    and `use @Config` statements from a raw CHTL source string.
    This runs before the main lexer.
    """
    def __init__(self, source_code: str):
        self.source_code = source_code
        self.use_pattern = re.compile(r'^\s*use\s+@Config\s+([a-zA-Z0-9_]+)\s*;', re.MULTILINE)
        self.config_pattern = re.compile(r'\[\s*Configuration\s*\]\s*(?:@Config\s+([a-zA-Z0-9_]+))?')

    def extract_configs(self) -> Tuple[List[str], Dict[str, str], Optional[str], str]:
        """
        Finds all config blocks and the 'use' statement.
        Returns: (unnamed_configs, named_configs, used_config_name, cleaned_source)
        """
        unnamed_configs = []
        named_configs = {}
        used_config_name = None
        cleaned_source = self.source_code

        # 1. Find and remove the 'use' statement first.
        use_match = self.use_pattern.search(cleaned_source)
        if use_match:
            used_config_name = use_match.group(1)
            # Remove the 'use' statement from the source
            cleaned_source = cleaned_source.replace(use_match.group(0), '', 1)

        # 2. Find all configuration blocks
        search_pos = 0
        while True:
            match = self.config_pattern.search(cleaned_source, search_pos)
            if not match:
                break

            start_index = match.start()
            config_name = match.group(1)

            open_brace_index = cleaned_source.find('{', match.end())
            if open_brace_index == -1:
                search_pos = match.end()
                continue

            brace_level = 1
            i = open_brace_index + 1
            while i < len(cleaned_source):
                if cleaned_source[i] == '{': brace_level += 1
                elif cleaned_source[i] == '}':
                    brace_level -= 1
                    if brace_level == 0: break
                i += 1

            if brace_level != 0:
                search_pos = open_brace_index + 1
                continue

            close_brace_index = i
            content = cleaned_source[open_brace_index + 1 : close_brace_index]

            if config_name:
                named_configs[config_name] = content
            else:
                unnamed_configs.append(content)

            # Remove the full block for the next iteration of finding its text
            # and for the final cleaned source
            full_block_text = cleaned_source[start_index : close_brace_index + 1]
            cleaned_source = cleaned_source.replace(full_block_text, '', 1)

            # Start searching from the beginning of the cleaned source
            search_pos = 0

        return unnamed_configs, named_configs, used_config_name, cleaned_source.strip()
