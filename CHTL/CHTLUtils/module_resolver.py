import os
from typing import Optional

class ModuleResolver:
    def __init__(self, official_module_dir: str, current_file_path: str):
        """
        Initializes the ModuleResolver.

        :param official_module_dir: The path to the compiler's official 'module' directory.
        :param current_file_path: The path of the CHTL file currently being compiled.
        """
        self.official_module_dir = os.path.abspath(official_module_dir)
        self.current_file_dir = os.path.dirname(os.path.abspath(current_file_path)) if current_file_path else os.getcwd()

        self.search_paths = [
            os.path.join(self.current_file_dir, 'module'),
            self.current_file_dir,
        ]
        # Prepend official dir if it's different and exists
        if os.path.isdir(self.official_module_dir) and self.official_module_dir not in self.search_paths:
            self.search_paths.insert(0, self.official_module_dir)


    def resolve(self, import_path: str) -> Optional[str]:
        """
        Resolves an import path to a full file path based on CHTL's search rules.

        :param import_path: The path from the [Import] statement (e.g., "Chtholly", "utils/helpers.chtl").
        :return: The absolute path to the module file, or None if not found.
        """

        # Case 1: Absolute path
        if os.path.isabs(import_path):
            if os.path.exists(import_path):
                return import_path
            return None

        # Case 2: Official module prefix
        if import_path.startswith("chtl::"):
            path_part = import_path.replace("chtl::", "")
            return self._search_in_paths([self.official_module_dir], path_part)

        # Case 3: Relative or bare import
        return self._search_in_paths(self.search_paths, import_path)

    def _search_in_paths(self, paths: list[str], name: str) -> Optional[str]:
        """
        Searches for a module name within a given list of directory paths.
        """

        # If the name includes a file extension, just look for that file.
        if '.' in os.path.basename(name):
            for path in paths:
                full_path = os.path.join(path, name)
                if os.path.exists(full_path):
                    return os.path.abspath(full_path)
            return None

        # If no extension, search for .cmod then .chtl
        for path in paths:
            # Check for .cmod first
            cmod_path = os.path.join(path, f"{name}.cmod")
            if os.path.exists(cmod_path):
                return os.path.abspath(cmod_path)

            # Then check for .chtl
            chtl_path = os.path.join(path, f"{name}.chtl")
            if os.path.exists(chtl_path):
                return os.path.abspath(chtl_path)

        return None
