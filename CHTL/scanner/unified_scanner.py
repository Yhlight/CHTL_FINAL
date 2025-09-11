from typing import Dict, Any, Tuple
from CHTL.css_parser import parse_style_content
import re

class UnifiedScanner:
    def __init__(self, mode: str = 'chtl'):
        self.registry: Dict[str, Any] = {}
        self._next_id = 0
        self.mode = mode

    def _generate_id(self) -> str:
        self._next_id += 1
        return f"__chtl_ref_{self._next_id}"

    def _parse_listen_block(self, content: str) -> Dict[str, str]:
        handlers = {}
        # This regex is a simplification. It will fail on nested commas/braces in handlers.
        # A full parser for this would be needed for 100% robustness.
        handler_pattern = re.compile(r'([a-zA-Z0-9_]+)\s*:\s*((?:[^{}]|\{[^{}]*\})*?)(?=,\s*[a-zA-Z0-9_]+\s*:|$)', re.DOTALL)
        for match in handler_pattern.finditer(content):
            event_name = match.group(1).strip()
            handler_code = match.group(2).strip().removesuffix(',').strip()
            if event_name and handler_code:
                handlers[event_name] = handler_code
        return handlers

    def _parse_delegate_block(self, content: str) -> Dict:
        """Parses the content of a delegate block."""
        data = {'handlers': {}}
        # Find the target: property
        target_match = re.search(r'target\s*:\s*(.*?)(?=,|$)', content, re.DOTALL)
        if target_match:
            data['target_selectors'] = target_match.group(1).strip()

        # Find event handlers
        handler_pattern = re.compile(r'([a-zA-Z0-9_]+)\s*:\s*((?:[^{}]|\{[^{}]*\})*?)(?=,|$)', re.DOTALL)
        for match in handler_pattern.finditer(content):
            event_name = match.group(1).strip()
            if event_name == 'target': continue # Skip the target property
            handler_code = match.group(2).strip().removesuffix(',').strip()
            if event_name and handler_code:
                data['handlers'][event_name] = handler_code
        return data


    def scan(self, source_text: str) -> Tuple[str, Dict[str, Any]]:
        if self.mode == 'js':
            return self._scan_js(source_text)
        return self._scan_chtl(source_text)

    def _scan_js(self, source_text: str) -> Tuple[str, Dict[str, Any]]:
        # First, replace simple selectors as they don't have nested structures
        def replace_selector(match):
            block_id = self._generate_id()
            self.registry[block_id] = {'type': 'selector', 'selector': match.group(1).strip()}
            return f"__selector_ref__(\"{block_id}\")"

        text = re.sub(r'\{\{(.*?)\}\}', replace_selector, source_text)

        # Now, manually scan for listen/delegate/animate blocks to handle nested braces correctly
        modified_source = ""
        last_index = 0
        js_block_pattern = re.compile(r'([a-zA-Z0-9_$.]+)\s*->\s*(listen|delegate)\s*\{|\b(animate)\s*\{')

        while True:
            match = js_block_pattern.search(text, last_index)
            if not match:
                break

            start_index = match.start()
            modified_source += text[last_index:start_index]

            if match.group(3): # animate
                block_type = match.group(3)
                target_obj = None # animate is not a method call
                brace_index = text.find('{', match.end(3))
            else: # listen or delegate
                target_obj = match.group(1).strip()
                block_type = match.group(2).strip()
                brace_index = text.find('{', match.end(2))

            brace_depth = 1
            content_start = brace_index + 1
            content_end = -1
            for j in range(content_start, len(text)):
                if text[j] == '{': brace_depth += 1
                elif text[j] == '}':
                    brace_depth -= 1
                    if brace_depth == 0: content_end = j; break

            if content_end != -1:
                block_content = text[content_start:content_end]
                block_id = self._generate_id()

                if block_type == 'listen':
                    handlers = self._parse_listen_block(block_content)
                    self.registry[block_id] = {'type': 'listen', 'target': target_obj, 'handlers': handlers}
                    modified_source += f"__listen_ref__(\"{block_id}\");"
                elif block_type == 'delegate':
                    delegate_data = self._parse_delegate_block(block_content)
                    self.registry[block_id] = {'type': 'delegate', 'parent_target': target_obj, **delegate_data}
                    modified_source += f"__delegate_ref__(\"{block_id}\");"
                elif block_type == 'animate':
                    animate_data = self._parse_animate_block(block_content)
                    self.registry[block_id] = {'type': 'animate', **animate_data}
                    modified_source += f"__animate_ref__(\"{block_id}\")"

                last_index = content_end + 1
            else:
                modified_source += text[last_index:match.end()]
                last_index = match.end()

        modified_source += text[last_index:]
        return modified_source, self.registry


    def _parse_animate_block(self, content: str) -> Dict:
        """
        Parses the content of an animate block using regex to extract key parts.
        This is not a full JS parser but is designed to handle the specific
        structure of `keyframes`, `options`, and `when`.
        """
        data = {}
        # This regex is non-greedy and assumes balanced brackets/braces.
        keyframes_match = re.search(r'keyframes\s*:\s*(\[.*?\])', content, re.DOTALL)
        if keyframes_match:
            data['keyframes'] = keyframes_match.group(1)

        # This regex is non-greedy and assumes balanced brackets/braces.
        options_match = re.search(r'options\s*:\s*(\{.*?\})', content, re.DOTALL)
        if options_match:
            data['options'] = options_match.group(1)

        # Extracts the value of 'when', which should be a string literal.
        when_match = re.search(r'when\s*:\s*([\'"](.*?_?)[\'"])', content, re.DOTALL)
        if when_match:
            # We want the inner value, without the quotes.
            data['when'] = when_match.group(2)

        # Basic validation
        if 'keyframes' not in data or 'options' not in data:
            # You could raise an error here for malformed animate blocks
            print(f"Warning: Malformed animate block content: {content}")

        return data

    def _scan_chtl(self, source_text: str) -> Tuple[str, Dict[str, Any]]:
        block_pattern = re.compile(r'\b(style|script)\s*\{|(\[Origin\])')
        modified_source = ""
        last_index = 0

        while True:
            match = block_pattern.search(source_text, last_index)
            if not match: break
            start_index = match.start()
            modified_source += source_text[last_index:start_index]

            if match.group(1):
                block_type = match.group(1)
                brace_index = source_text.find('{', match.end(1))
            elif match.group(2):
                block_type = 'origin'
                brace_index = source_text.find('{', match.end(2))
            else:
                last_index = match.end(); continue

            if brace_index == -1: last_index = match.end(); continue

            brace_depth = 1
            content_start = brace_index + 1
            content_end = -1
            for j in range(content_start, len(source_text)):
                if source_text[j] == '{': brace_depth += 1
                elif source_text[j] == '}':
                    brace_depth -= 1
                    if brace_depth == 0: content_end = j; break

            if content_end != -1:
                block_content = source_text[content_start:content_end]
                block_id = self._generate_id()

                if block_type == 'style':
                    self.registry[block_id] = { 'type': 'style', **parse_style_content(block_content) }
                    modified_source += f'__style_ref__("{block_id}");'
                elif block_type == 'script':
                    self.registry[block_id] = { 'type': 'script', 'content': block_content }
                    modified_source += f'__script_ref__("{block_id}");'
                elif block_type == 'origin':
                    type_text = source_text[match.end(2):brace_index].strip()
                    type_match = re.match(r'(@[a-zA-Z]+)', type_text)
                    origin_type = type_match.group(1) if type_match else '@Html'
                    self.registry[block_id] = { 'type': 'origin', 'origin_type': origin_type, 'content': block_content }
                    modified_source += f'__origin_ref__("{block_id}");'
                last_index = content_end + 1
            else:
                modified_source += source_text[last_index:brace_index + 1]
                last_index = brace_index + 1

        modified_source += source_text[last_index:]
        return modified_source, self.registry
