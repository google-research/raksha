// This is a lower-level graph IR that might compile from graph_ir. It 
// simplifies the interfaces between nodes that includes structs by
// flattening the structs and separating paths (unfolded complete names of
// elements of structs) to locations that are read/written) into lists of 
// inputs/outputs.

// As an example of how this might work, Imagine we have a GraphIR instance that looks like
//
// GraphNode {
//      name: "someNode"
//      tpe: Computation
//      connections: vec![
//          Field {
//              name: "foo",
//              dir: Input,
//              tpe: StructType {
//                  Field {name: "a", dir: Input, tpe: BaseType}
//                  Field {name: "b", dir: Input, tpe: BaseType}
//              }
//          }
//          Field {
//              name: "bar"
//              dir: Output,
//              tpe: BaseType
//          }
//      ]
// }
// 
//
// This would translate to
//
// LowGraphNode {
//      name: "SomeNode",
//      tpe: Computation,
//      inputs: vec!["foo.a", "foo.b"],
//      outptus: vec!["bar"]
// }
pub enum LowGraphType {
    Computation,
    Storage
}

pub struct LowGraphNode {
    pub name: String,
    pub tpe: LowGraphType,
    pub inputs: Vec<String>, // names of paths that are read
    pub outputs: Vec<String>, // names of paths that are written
    // nodes that are both read and written appear in both inputs and outputs
}
