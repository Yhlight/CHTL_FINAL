import os
import sys
import zipfile
import argparse

def validate_cmod_structure(module_path):
    """
    Validates that the directory has the basic CMOD structure.
    """
    if not os.path.isdir(module_path):
        print(f"Error: Provided path '{module_path}' is not a directory.", file=sys.stderr)
        return False

    src_path = os.path.join(module_path, 'src')
    info_path = os.path.join(module_path, 'info')

    if not os.path.isdir(src_path):
        print(f"Error: CMOD structure invalid. Missing 'src' directory in '{module_path}'.", file=sys.stderr)
        return False

    if not os.path.isdir(info_path):
        print(f"Error: CMOD structure invalid. Missing 'info' directory in '{module_path}'.", file=sys.stderr)
        return False

    # CHTL.md requires that the info file matches the main module folder name.
    module_name = os.path.basename(os.path.normpath(module_path))
    expected_info_file = os.path.join(info_path, f"{module_name}.chtl")

    if not os.path.exists(expected_info_file):
        print(f"Warning: Expected info file '{expected_info_file}' not found.", file=sys.stderr)
        # This is a warning, not a fatal error, for now.

    return True

def create_cmod_archive(module_path, output_path=None):
    """
    Creates a .cmod archive from a given module directory.
    """
    module_path = os.path.abspath(module_path)
    module_name = os.path.basename(os.path.normpath(module_path))

    if not output_path:
        output_path = os.path.dirname(module_path)

    output_filename = os.path.join(output_path, f"{module_name}.cmod")

    if not validate_cmod_structure(module_path):
        return

    try:
        with zipfile.ZipFile(output_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
            for root, _, files in os.walk(module_path):
                for file in files:
                    file_path = os.path.join(root, file)
                    # The arcname is the path inside the zip file
                    arcname = os.path.relpath(file_path, os.path.dirname(module_path))
                    zipf.write(file_path, arcname)

        print(f"Successfully created CMOD archive at '{output_filename}'")

    except Exception as e:
        print(f"An error occurred while creating the archive: {e}", file=sys.stderr)

def main():
    parser = argparse.ArgumentParser(
        description="Package a CHTL module directory into a .cmod file."
    )
    parser.add_argument(
        "module_path",
        help="The path to the CHTL module source directory."
    )
    parser.add_argument(
        "-o", "--output",
        help="The output directory for the .cmod file. Defaults to the parent of the module directory."
    )
    args = parser.parse_args()

    create_cmod_archive(args.module_path, args.output)

if __name__ == "__main__":
    main()
