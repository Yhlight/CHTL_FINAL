import os

class FileLoader:
    def __init__(self, base_path=""):
        self.base_path = base_path

    def read(self, file_path: str) -> str:
        """Reads the content of a file relative to the base path."""
        try:
            # For now, we only support relative paths from the execution directory
            # A more robust loader would handle various pathing strategies.
            with open(file_path, 'r', encoding='utf-8') as f:
                return f.read()
        except FileNotFoundError:
            print(f"Warning: Could not find file to import at '{file_path}'")
            return None
        except Exception as e:
            print(f"Warning: Error reading file '{file_path}': {e}")
            return None
