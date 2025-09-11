#!/bin/bash
# This script packages a module directory into a .cmod file.
# Usage: ./package.sh <ModuleName>

MODULE_NAME=$1
CMOD_FILE="${MODULE_NAME}.cmod"

if [ -z "$MODULE_NAME" ]; then
    echo "Usage: ./package.sh <ModuleName>"
    exit 1
fi

if [ ! -d "$MODULE_NAME" ]; then
    echo "Error: Directory '$MODULE_NAME' not found."
    exit 1
fi

echo "Packaging ${MODULE_NAME} into ${CMOD_FILE}..."
# The -j option junks the paths, which is what we want.
# We cd into the directory so the zip file contains `src` and `info` at the root.
(cd "$MODULE_NAME" && zip -r "../${CMOD_FILE}" . -x ".*" -x "__MACOSX")

echo "Package complete."
