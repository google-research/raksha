
#[cfg(test)]
mod tests {
    use crate::souffle_interface::*;

    // This gives the name of an authorization logic program in
    // test_inputs, and a vector that relates the names of queries
    // to the boolean values they should have.
    pub struct QueryTest<'a> {
        pub filename: &'a str,
        pub query_expects: Vec<(&'a str, bool)>
            // Query_expects is a vec not a HashMap because
            // there is no hashmap! macro for constructing HashMap
            // literals. There are crates providing this macro, but
            // limiting dependencies is better for security/verification.
    }
    
    pub fn run_query_test(t: QueryTest) {
        input_to_souffle_file(&t.filename.to_string(),
            &"test_inputs".to_string(), &"test_outputs".to_string());
        run_souffle(&format!("test_outputs/{}.dl", t.filename),
                    &"test_outputs".to_string());
        for (qname, intended_result) in t.query_expects {
            let queryfile = format!("test_outputs/{}.csv", qname);
            assert!(is_file_empty(&queryfile) != intended_result);
        }
    }

    #[test]
    fn test_2() {
        run_query_test(QueryTest { 
            filename: "test2",
            query_expects: vec!(("q1", true), ("q2", false))
        });
    }


}
