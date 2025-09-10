from typing import Dict
from Scanner.scanner import CodeFragment
# The dispatcher needs to call the CHTL compiler
from chtl_compiler import compile_chtl

class CompilerDispatcher:
    """
    Dispatches code fragments to their respective compilers.
    """
    def __init__(self, use_default_structure: bool = False):
        """
        Initializes the dispatcher.
        Args:
            use_default_structure: Flag to pass to the CHTL generator.
        """
        self.use_default_structure = use_default_structure
        # In the future, this would initialize other compilers (JS, CSS)
        pass

    def dispatch(self, chtl_fragment: str, fragments: Dict[str, CodeFragment]) -> Dict[str, str]:
        """
        Takes fragments from the scanner and "compiles" them.
        """
        print("CompilerDispatcher: Dispatching fragments...")

        compiled_outputs = {}

        # 1. Compile the main CHTL fragment
        # The CHTL compiler needs to know if it's part of a larger document build
        compiled_chtl = compile_chtl(chtl_fragment, use_default_structure=self.use_default_structure)
        compiled_outputs['__main__'] = compiled_chtl

        # 2. "Compile" the other fragments
        for placeholder, fragment in fragments.items():
            print(f"  - Compiling fragment for {placeholder} (type: {fragment.type})")
            if fragment.type == 'css':
                # For now, just wrap in style tags. A real CSS compiler would go here.
                compiled_outputs[placeholder] = f"<style>\n{fragment.content}\n</style>"
            elif fragment.type == 'js':
                # For now, just wrap in script tags. A real JS/CHTL JS compiler would go here.
                compiled_outputs[placeholder] = f"<script>\n{fragment.content}\n</script>"
            elif fragment.type == 'html':
                compiled_outputs[placeholder] = fragment.content
            else:
                compiled_outputs[placeholder] = f"<!-- raw content: {fragment.content} -->"

        return compiled_outputs
