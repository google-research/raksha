# An Authorization Logic

This directory contains a research prototype implementation of an authorization
logic. Authorization logic is a style of policy language for authorization -
deciding whether or not performing some action should be allowed. Examples of
authorization logics include
[ABLP](https://homepages.inf.ed.ac.uk/gdp/publications/Calculus_for_Access_Control.pdf),
[Binder](https://web.cs.wpi.edu/~guttman/cs564/papers/binder.pdf),
[SecPal](https://people.mpi-sws.org/~dg/teaching/lis2014/modules/authorization-1-becker07.pdf),
and [NAL](https://www.cs.cornell.edu/fbs/publications/NexusNalRationale.pdf). At
time of writing, this prototype is about the same as SecPal. A longer
description of authorization logic is given below, followed by instructions.

This authorization logic compiles programs from the source language into the
[Souffle](https://souffle-lang.github.io/index.html) implementation of Datalog.
Souffle should be installed by following
[these instructions](https://souffle-lang.github.io/install).

## Summary of Authorization Logic

### Logic Programming

Authorization logics extend some logic programming language (e.g. Datalog,
Prolog), and permit an action only when a specific predicate can be proved using
inference rules and facts written in this language. Generally, authorization
logic is paired with a reference monitor that interprets the policy language and
permits the guarded actions only when presented with a proof of the proposition
used to grant access.

### Principals and decentralization

The key feature of authorization logic is that facts are decentralized - each
fact or rule is attributed to a principal (or actor). For example,
`HR says isManager(Alice)` means that `HR` believes or claims that
`isManager(Alice)` is true. Decentralizing facts in this way ensures that a
resource owner retains control over access to resources in an environment with
mutually distrusting principals. As an example, to use the rule

```
"FileServer" says canAccess(file, requestor) :- isManager(requestor),
                                              isValidFile(file).
```

requires proving that `FileServer` says `isManager(requestor)` and
`FileServer says isValidFile(file)`.

### Delegation

Because authorization logic models beliefs held by various parties rather than a
single global truth believed by all, we need a way to export facts from one
principal to another. In authorization logic, this is done by delegation. For
example:

```
FileServer says HR canSay isManager(x).
```

means that `HR says isManager(x)` implies `FileServer says isManager(x)`. The
exact language-level construct for delegation has varied among authorization
logics, but the key point is that this gives one principal (`FileServer`) a way
of specifying when it believes claims made by another (`HR`). Delegations can
also be done conditionally by placing them on the left of a proof rule.

### Authenticating policies

When `FileServer` imports a policy statement from `HR`, we want to know that a
claim genuinely originated from `HR`. To authenticate policy statements, each
principal is also associated with an asymmetric key-pair. When a policy
statement is exported by a principal, the statement is digitally signed, the
signature is sent to the recipient along with a serialization of the policy
claim, and the recipient uses the public key to check the signature.

## Instructions

### Running a program

**TODO(b/253998272):** When the tests are migrated. We will have a command line to
work with.


### Language Guide

#### Statements, variables, and constants

Programs are sequences of assertions that are attributed to principals which we
call statements. Statements can either be _facts_ of the form

```
<principal_name> says <predicate_name>(arg_0, ..., arg_n).
```

meaning that the principal believes predicate (applied to its arguments) to be
true, or _rules_ of the form

```
<principal_name> says <predicate_name>(arg_0, ..., arg_n) :-
    <pred_0>(arg_0, ..., arg_n), ..., <pred_n>(arg_0, ..., arg_n).
```

which specify the conditions under which the principal believes the predicate on
the left side of the rule.

For example, if we have the rule:

```
"Alice" says somePred(x) :- condition(x).
```

we can combine this with the fact `"Alice" says condition(x)` to prove
`"Alice" says somePred(x)`.

When writing facts or rules, the arguments to predicates can either be constants
or variables. Constants are written in quotes whereas variables are not. For
example,

```
"Alice" says somePred("x") :- condition("x").
```

gives a rule for proving somePred for just a particular argument, `"x"`, whereas
the same rule replaced with `x` can be applied to any argument about which we
can prove `"Alice" says condition(x)`.

Principals and other kinds of arguments share the same syntax -- they can be any
sequence of lower or uppercase letters and digits, but they must begin with a
letter. For constants, the same syntax is used within the quotes.

Principal names can also be used as arguments to predicates, and quotes are used
indicate whether a principal is a constant or variable in the same way.

Multiple rules/facts stated by the same principal can be grouped together using
curly braces, as in:

```
"Alice" says {
    rule1(x) :- condition(x).
    rule2(y) :- condition1(y), condition(x).
    someFact(x).
}
```

#### Delegation

Delegation works by prepending a predicate on the left of a fact or rule with
`<principal_name> canSay`. Delegations can only appear on the left side of a
rule. As an example, the rule

```
"Alice" says "Bob" canSay pred(x) :- condition(x).
```

can be combined with `"Alice" says condition("y")` and `"Bob" says pred("y")` to
prove `Alice says pred("y")`.

#### Attributes and canActAs

Predicates can be used to represent attributes of principals or other
constants/variables using the syntax `principal_name predicate_name(<args>)`,
such as, `"Manager" hasTraining("interviewing")`. Attributes can appear in facts
or in the left or right of a rule.

Attributes can be passed from one principal to another using `canActAs`. If
`A canActAs B`, then `A` inherits all the attributes of `B`. For example,

```
"HR" says "Alice" canActAs "Manager".
```

can be combined with `"HR" says "Manager" hasTraining("interviewing")` to prove
`"HR" says "Alice" hasTraining("interviewing")`.

Combining attributes with `canActAs` gives a way to define _roles_ which group
privileges together.

#### Queries

Queries test if it is possible to prove a specific predicate given the facts and
rules. Queries can be written using the syntax:

```
<query_name> = query <principal_name> says <predicate_name>(<args>)?
```

If the program contained queries (described below), then the there will also be
a file called `<query_name>.csv` for each query. The query is true if the CSV
contains one entry and is false otherwise.
