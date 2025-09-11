from CHTL.scanner.unified_scanner import UnifiedScanner
from .js_generator import JavaScriptGenerator

class CHTLJS_Compiler:
    """
    Compiler for the CHTL JS language. It takes raw script content,
    scans it for CHTL JS features, and then generates the final JavaScript code.
    """
    def __init__(self, registry: dict):
        self.registry = registry

    def compile(self, raw_script_content: str) -> str:
        """
        Compiles a string of CHTL JS code into standard JavaScript.
        """
        # Step 1: Use UnifiedScanner in 'js' mode to find and process CHTL JS blocks
        scanner = UnifiedScanner(mode='js')
        # The scanner needs access to the main registry to add new items
        scanner.registry = self.registry
        modified_script, _ = scanner.scan(raw_script_content)

        # Step 2: The generator now takes the modified script and replaces placeholders
        js_generator = JavaScriptGenerator(registry=self.registry)
        final_js = js_generator.generate(modified_script)

        return final_js
