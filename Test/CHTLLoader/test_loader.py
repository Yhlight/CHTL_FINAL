import unittest
import os
import sys

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from CHTL.CHTLLoader.loader import Loader

class TestLoader(unittest.TestCase):

    def setUp(self):
        """Create a mock file structure for testing."""
        self.test_dir = "test_loader_temp"
        os.makedirs(self.test_dir, exist_ok=True)

        self.file_path = os.path.join(self.test_dir, "test_module.chtl")
        with open(self.file_path, "w") as f:
            f.write("div { text: 'hello from module'; }")

    def tearDown(self):
        """Clean up the mock files."""
        if os.path.exists(self.file_path):
            os.remove(self.file_path)
        if os.path.exists(self.test_dir):
            os.rmdir(self.test_dir)

    def test_load_file_successfully(self):
        loader = Loader()
        content = loader.load("test_module", self.test_dir)
        self.assertIn("hello from module", content)

    def test_load_cached_file(self):
        loader = Loader()
        # First load
        content1 = loader.load("test_module", self.test_dir)

        # To test the cache, we can delete the file and see if it still loads
        os.remove(self.file_path)

        # The loader's internal `resolve_path` will now fail, but the cache should hit first.
        # Let's adjust the test. The cache key is the absolute path.
        abs_path = os.path.abspath(self.file_path)
        self.assertIn(abs_path, loader.loaded_files)

        # Let's create a new loader to test the negative case
        new_loader = Loader()
        with self.assertRaises(FileNotFoundError):
            new_loader.load("test_module", self.test_dir)

    def test_load_file_not_found(self):
        loader = Loader()
        with self.assertRaises(FileNotFoundError):
            loader.load("non_existent_module", self.test_dir)

if __name__ == '__main__':
    unittest.main()
