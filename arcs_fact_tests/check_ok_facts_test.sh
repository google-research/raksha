#!/bin/bash

FIND_FAILING_TAG_CHECKS_BIN="$1"
FACTS_DIR="$2"
if ! "${FIND_FAILING_TAG_CHECKS_BIN}" "${FACTS_DIR}"; then
  echo >&2 "Found failing tag checks"
  exit 1
fi
exit 0
