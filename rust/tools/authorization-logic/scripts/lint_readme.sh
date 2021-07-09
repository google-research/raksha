#!/bin/sh
# This script assumes it is being run from the top-level directory.
prettier --config .prettierrc.yaml README.md -w 
markdownlint -f -c .markdownlint.yaml README.md
