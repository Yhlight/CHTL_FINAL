from typing import Dict, List
from CompilerDispatcher.dispatcher import CompiledFragment

class CodeMerger:
    """
    Merges compiled code fragments into a single final output,
    handling the final document structure.
    """
    def merge(self, compiled_outputs: Dict[str, CompiledFragment], use_default_structure: bool = False) -> str:
        """
        Takes the compiled outputs and assembles the final HTML.
        """
        if '__main__' not in compiled_outputs:
            raise ValueError("Main CHTL output not found in compiled outputs.")

        main_content = compiled_outputs.pop('__main__').content

        head_fragments: List[str] = []
        inline_fragments: Dict[str, str] = {}

        # Separate fragments for head or inline substitution
        for placeholder, fragment in compiled_outputs.items():
            if fragment.type in ('css', 'js'):
                head_fragments.append(fragment.content)
            else: # html, raw
                inline_fragments[placeholder] = fragment.content

        # Substitute all inline placeholders first
        for placeholder, content in inline_fragments.items():
            main_content = main_content.replace(placeholder, content)

        # If we don't need the default structure, just combine the parts
        if not use_default_structure:
            all_head_content = "\n".join(head_fragments)
            return f"{all_head_content}\n{main_content}".strip()

        # Otherwise, build the full HTML document
        # Note: The CHTL fragment might contain a <title> tag, which is fine to be in the body,
        # but a more advanced merger could extract it and move it to the head.
        # For now, this is a robust implementation.

        head_content_str = "\n".join(f"    {line}" for fragment in head_fragments for line in fragment.splitlines())

        # Indent the body content
        body_content_str = "\n".join(f"    {line}" for line in main_content.splitlines())

        final_html = f"""<!DOCTYPE html>
<html>
<head>
{head_content_str}
</head>
<body>
{body_content_str}
</body>
</html>"""

        return final_html
