import unittest
import textwrap
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from CodeMerger.merger import CodeMerger
from CompilerDispatcher.dispatcher import CompiledFragment

class TestCodeMerger(unittest.TestCase):

    def _assert_html_equal(self, actual, expected):
        # Normalize whitespace for a more robust comparison
        normalized_actual = " ".join(actual.split())
        normalized_expected = " ".join(textwrap.dedent(expected).strip().split())
        self.assertEqual(normalized_actual, normalized_expected)

    def test_merge_as_fragment(self):
        merger = CodeMerger()
        compiled_outputs = {
            '__main__': CompiledFragment(type='main', content='<div>Main Content __P1__</div>'),
            '__P1__': CompiledFragment(type='html', content='<span>Inline</span>'),
            '__P2__': CompiledFragment(type='css', content='<style>body { color: red; }</style>'),
        }

        result = merger.merge(compiled_outputs, use_default_structure=False)

        expected = """
        <style>body { color: red; }</style>
        <div>Main Content <span>Inline</span></div>
        """
        self._assert_html_equal(result, expected)

    def test_merge_as_full_document(self):
        merger = CodeMerger()
        compiled_outputs = {
            '__main__': CompiledFragment(type='main', content='<p>Hello</p>'),
            '__P1__': CompiledFragment(type='css', content='<style>.box { width: 100px; }</style>'),
            '__P2__': CompiledFragment(type='js', content='<script>alert("test");</script>'),
        }

        result = merger.merge(compiled_outputs, use_default_structure=True)

        expected = """
        <!DOCTYPE html>
        <html>
        <head>
            <style>.box { width: 100px; }</style>
            <script>alert("test");</script>
        </head>
        <body>
            <p>Hello</p>
        </body>
        </html>
        """
        self._assert_html_equal(result, expected)

    def test_merge_with_no_head_fragments(self):
        merger = CodeMerger()
        compiled_outputs = {
            '__main__': CompiledFragment(type='main', content='<div>Main Content</div>'),
        }
        result = merger.merge(compiled_outputs, use_default_structure=True)

        expected = """
        <!DOCTYPE html>
        <html>
        <head>
        </head>
        <body>
            <div>Main Content</div>
        </body>
        </html>
        """
        self.assertNotIn('<style>', result)
        self.assertNotIn('<script>', result)
        self._assert_html_equal(result, expected)

if __name__ == '__main__':
    unittest.main()
