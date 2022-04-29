#!/bin/bash

# A simple script to test the generate_datalog_program command line.
ROOT_DIR=$TEST_SRCDIR/$TEST_WORKSPACE/src/frontends/arcs/xform_to_datalog
CMD=$ROOT_DIR/generate_datalog_program

AUTH_FILE=$ROOT_DIR/testdata/ok_claim_propagates.auth
MANIFEST_FILE=$ROOT_DIR/testdata/ok_claim_propagates_proto.binarypb
DATALOG_FILE=$ROOT_DIR/testdata/ok_claim_propagates.dl
GENERATED_DATALOG_FILE=`mktemp`

$CMD --auth_logic_file=$AUTH_FILE --manifest_proto=$MANIFEST_FILE \
  --datalog_file=$GENERATED_DATALOG_FILE --overwrite

# Return the result of comparing generated and golden file.
diff $GENERATED_DATALOG_FILE $DATALOG_FILE
