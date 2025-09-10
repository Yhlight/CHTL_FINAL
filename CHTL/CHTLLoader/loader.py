import os
from typing import Dict, Optional

class Loader:
    """
    Handles loading and caching of CHTL source files.
    """
    def __init__(self):
        self.loaded_files: Dict[str, str] = {}

    def resolve_path(self, file_path: str, current_dir: str) -> Optional[str]:
        """
        Resolves the full path to an importable file.

        For now, this is a simplified implementation that only handles
        relative paths. The full implementation would search in official
        and local module directories as per the CHTL spec.
        """
        # The spec says `.` and `/` are equivalent in paths
        normalized_path_parts = file_path.replace(".", "/").split("/")

        # This doesn't correctly handle `..` or complex paths yet.
        # Let's assume simple relative paths for now.

        # A simple relative path resolver
        # os.path.join will handle the OS-specific separators
        full_path = os.path.join(current_dir, file_path)

        # The spec implies automatic .chtl extension
        if not full_path.endswith('.chtl'):
            full_path += '.chtl'

        if os.path.exists(full_path):
            return os.path.abspath(full_path)

        return None

    def load(self, file_path: str, current_dir: str = '.') -> str:
        """
        Loads a file's content, using a cache to avoid redundant reads.

        Args:
            file_path: The path to the file from the import statement.
            current_dir: The directory of the file that contains the import.

        Returns:
            The content of the file as a string.

        Raises:
            FileNotFoundError: If the file cannot be found after trying all search paths.
        """
        resolved_path = self.resolve_path(file_path, current_dir)

        if not resolved_path:
            raise FileNotFoundError(f"Could not find file '{file_path}' from directory '{current_dir}'")

        if resolved_path in self.loaded_files:
            return self.loaded_files[resolved_path]

        try:
            with open(resolved_path, 'r', encoding='utf-8') as f:
                content = f.read()
                self.loaded_files[resolved_path] = content
                return content
        except Exception as e:
            raise IOError(f"Could not read file '{resolved_path}': {e}")
