import pytest
from chtl_compiler.parser import parse_chtl
from chtl_compiler.generator import HtmlGenerator

def test_minimal_end_to_end_compilation():
    chtl_source = """
        div {
            id: main;
            class: "container";

            style {
                color: red;
                border: "1px solid black";
            }

            p {
                text: "Hello, CHTL!"
            }

            br {}
        }
    """

    expected_html = """
        <div id="main" class="container" style="color:red;border:1px solid black">
          <p>
            Hello, CHTL!
          </p>
          <br />
        </div>
    """

    ast = parse_chtl(chtl_source)
    generator = HtmlGenerator(ast)
    actual_html = generator.generate()

    normalized_actual = "".join(actual_html.split())
    normalized_expected = "".join(expected_html.split())

    assert normalized_actual == normalized_expected
