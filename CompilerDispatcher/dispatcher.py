from typing import Dict
from dataclasses import dataclass
from Scanner.scanner import CodeFragment
from CHTL.api import compile_chtl

@dataclass
class CompiledFragment:
    """
    Holds a compiled string and its original type for the merger.
    """
    type: str  # e.g., 'main', 'css', 'js', 'html'
    content: str

class CompilerDispatcher:
    """
    Dispatches code fragments to their respective compilers.
    """
    def __init__(self):
        # In the future, this would initialize compilers
        pass

    def dispatch(self, chtl_fragment: str, fragments: Dict[str, CodeFragment]) -> Dict[str, CompiledFragment]:
        """
        Takes fragments from the scanner and "compiles" them.
        """
        print("CompilerDispatcher: Dispatching fragments...")

        compiled_outputs: Dict[str, CompiledFragment] = {}

        # 1. Compile the main CHTL fragment using the new API
        compiled_chtl_content = compile_chtl(chtl_fragment)
        compiled_outputs['__main__'] = CompiledFragment(type='main', content=compiled_chtl_content)

        # 2. "Compile" the other fragments
        for placeholder, fragment in fragments.items():
            print(f"  - Compiling fragment for {placeholder} (type: {fragment.type})")

            compiled_content = ""
            if fragment.type == 'css':
                compiled_content = f"<style>\n{fragment.content}\n</style>"
            elif fragment.type == 'js':
                compiled_content = f"<script>\n{fragment.content}\n</script>"
            elif fragment.type == 'html':
                compiled_content = fragment.content
            else: # raw
                compiled_content = f"<!-- raw content: {fragment.content} -->"

            compiled_outputs[placeholder] = CompiledFragment(type=fragment.type, content=compiled_content)

        return compiled_outputs
