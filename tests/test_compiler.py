import pytest
from chtl_compiler.parser import parse_chtl
from chtl_compiler.generator import HtmlGenerator
import json

def test_empty_element_parsing():
    chtl_source = "br {}"
    ast = parse_chtl(chtl_source)
    # A simple check to see if it parsed as an element
    assert ast.children[0].tag == "br"
    assert not ast.children[0].children # No children

def test_end_to_end_compilation():
    chtl_source = """
        div {
            id: main;
            class: "container";

            style {
                width: 100 + 20px;
                font-size: 2 * 8em;
                opacity: 0.5;
                border: solid;
            }

            p {
                text {
                    "Hello, CHTL!"
                }
            }

            br {}
        }
    """

    expected_html = """
        <div id="main" class="container" style="width:120px;font-size:16em;opacity:0.5;border:solid">
          <p>
            Hello, CHTL!
          </p>
          <br />
        </div>
    """

    # 1. Parse
    ast = parse_chtl(chtl_source)

    # 2. Generate
    generator = HtmlGenerator()
    actual_html = generator.generate(ast)

    # 3. Compare
    normalized_actual = "".join(actual_html.split())
    normalized_expected = "".join(expected_html.split())

    assert normalized_actual == normalized_expected
