// Just a dummy file to force blaze to compile the new ir code.
// This won't be needed as soon as we create tests and also start using it.
#include "src/ir/derives_from_operation.h"
#include "src/ir/multi_operation.h"
#include "src/ir/operation.h"
#include "src/ir/storage.h"
#include "src/ir/tag_claim_operation.h"
#include "src/ir/sql_query_operation.h"

// Example:
// particle P1
//  .bar: reads Bar {}
//   foo: writes Foo {}
//   claim foo.a is userSelection
//   claim foo.x is derived from bar.y
//
//
// MultiOperationNode: {
// name_: "multi_op"
// inputs_: {(Bar, bar)}
// outputs_:  {(Foo, foo)}
// operations_: {
//  DerivesFromOperation: {
//    name_: "derives_from"
//    inputs_: {(Bar, bar)}
//    outputs_: {(Foo, foo)}
//    input_edges_: "multi_op.bar"
//    output_edges_: "tag_claim.foo"
//    allowed_edges_: {bar.y -> foo.x}
//  };
//  TagClaimOperation: {
//    name_: "tag_claim"
//    inputs_: {Foo}
//    outputs_: {Foo}
//    input_edges_: "derives_from.foo"
//    output_edges_: "multi_op.foo"
//    claims: {foo.a is "userSelection"}
//  };
// }
//
//
// }
