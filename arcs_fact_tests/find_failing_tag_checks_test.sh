#!/bin/bash
set -euo pipefail
SOUFFLE_BIN="$1"
FACTS_DIR=$(dirname "$2")
DL_FILE="$3"
EXPECTED_OUTPUT_FILE="$4"
if ! "$SOUFFLE_BIN" --fact-dir="$FACTS_DIR" "$DL_FILE" --output-dir=- | diff "$EXPECTED_OUTPUT_FILE" -; then
  echo >&2 "FAIL: expected output differs from observed output for tag checks."
  exit 1
fi

