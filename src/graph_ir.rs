// This file contains an IR for representing graphs that might compile from 
// Arcs manifests.
// It contains named graph nodes (which might be either computation or storage 
// as in particles or handles).
// The edges can be nested structs where different fields within the same struct 
// can be either read, written or both.

pub enum Direction {
    Input,  // reads
    Output, // writes
    InOut,  // reads + writes
}

pub enum DataType {
    BaseType,
    // BaseType represents any primitive type like int, char, etc. More
    // information than this could be added but it does not appear needed for
    // the analyses done here.
    StructType {fields: Vec<Field>}
}

pub struct Field {
    pub name: String,
    pub tpe: DataType,
    pub dir: Direction
}

pub enum NodeType {
    Computation, // executes code
    Storage      // stores data (like a handle)
}

// TODO check if directionality is already divided in
// arcs manifests. (I think it is not).
pub struct GraphNode {
    pub name: String,
    pub connections: Vec<Field>,
        // Because fields can be structs, "connections" can plausibly be a
        // single field.
    pub tpe: NodeType
}

// NOTE: correspondence to Types in arcs manifests protos:
// https://github.com/bgogul/arcs/blob/master/java/arcs/core/data/proto/manifest.proto

// I think CollectionTypes correspond roughly to structs in the Arcs 
// manifest protos,
// though I don't see where CollectionTypes (or any of the others) nest.

// Other manifest Types:
// - entitytype -- not sure what this is
// - referencetype -- presumably a pointer. I think this information is not 
// needed for these analyses
// - tuple -- I think tuples are a special case of structs, so we may not need this
// - type var -- Presumably these are for refinement types, which are not used here
// - singleton -- not sure how this differs from base types
// - CountType -- not sure what this is, but its implementation appears empty so
// perhaps it is like Unit
// - list -- can also be treated as a special case of structs
