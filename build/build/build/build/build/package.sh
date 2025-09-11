#!/bin/bash
MODULE_NAME=$1
CMOD_FILE="${MODULE_NAME}.cmod"
(cd "$MODULE_NAME" && zip -r "../${CMOD_FILE}" . -x ".*" -x "__MACOSX")
