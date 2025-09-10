import pytest
from chtl_compiler.compiler import ChtlCompiler

@pytest.fixture
def compiler():
    return ChtlCompiler()

def normalize_html(html: str) -> str:
    """Removes insignificant whitespace to make comparisons easier."""
    return "".join(html.split())

def test_simple_element(compiler):
    chtl = """
        div {
            id: main;
            text: "Hello";
        }
    """
    expected_html = '<div id="main">Hello</div>'
    compiled_html = compiler.compile(chtl)
    assert normalize_html(compiled_html) == normalize_html(expected_html)

def test_nested_elements(compiler):
    chtl = """
        html {
            body {
                p {
                    text: "Nested paragraph.";
                }
            }
        }
    """
    expected_html = '<html><body><p>Nested paragraph.</p></body></html>'
    compiled_html = compiler.compile(chtl)
    assert normalize_html(compiled_html) == normalize_html(expected_html)

def test_text_element(compiler):
    chtl = """
        text {
            "Just some text."
        }
    """
    expected_html = 'Just some text.'
    compiled_html = compiler.compile(chtl)
    assert compiled_html == expected_html # Whitespace is significant here

def test_unquoted_values(compiler):
    chtl = """
        div {
            class: my-class;
            id: my-id-123;
        }
    """
    expected_html = '<div class="my-class" id="my-id-123"></div>'
    compiled_html = compiler.compile(chtl)
    assert normalize_html(compiled_html) == normalize_html(expected_html)

def test_self_closing_tag(compiler):
    chtl = """
        img {
            src: "image.jpg";
            alt: "My Image";
        }
    """
    expected_html = '<img src="image.jpg" alt="My Image">'
    compiled_html = compiler.compile(chtl)
    assert normalize_html(compiled_html) == normalize_html(expected_html)

def test_style_block(compiler):
    chtl = """
        div {
            style {
                width: 100px;
                color: red;
                border: 1px solid black;
            }
            id: styled-div;
        }
    """
    expected_html = '<div id="styled-div" style="width: 100px; color: red; border: 1px solid black;"></div>'
    compiled_html = compiler.compile(chtl)
    assert normalize_html(compiled_html) == normalize_html(expected_html)
