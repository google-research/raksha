# Strategies for Getting Labelchecking in Datalog to Work

For all of these approaches, the label check without downgrades can be 
specified roughly as:
 - Compute the join over the labels of the inputs Li, compute the meet over the 
   labels of the outputs Lo, show that Li flowsTo Lo.
or
 - For each input `x` with label `Li_x`, and each output `y` with label `Lo_y`, 
   show that `Li_x` flowsTo `Lo_y`

To add downgrades, we need a second rule for proving that a field has a label 
that is lower in the lattice order than its original one. It should only be 
possible to do a downgrade when the owner of a labelElement gives permission 
using authorization logic. This rule should work roughly as follows
- using the normal rule for labelchecking a field, we proved it has `l_old`
- we proved principal `p` owns `l_old`
- we proved `p` says candDowngrad(`l_old`, `l_new`)

## Labels using Algebraic Datatypes

An algebraic datatype represents labels with constructors that represent atomic 
labels and joins over labels. A problem with this approach is that it 
necessitates conversion into conjunctive or disjunctive normal form (this is 
equivalent to converting boolean expressions to CNF/DNF). It is possible to
write a relation that converts to CNF/DNF in prolog, but it is unclear if this
is doable in datalog.

TODO: why need normal form?

## Hypercube Labels

TODO explain what this is.

This gives all the conventional lablechecking relations and operators, but runs 
into performance issues because the check eagerly computes the label for all 
possible hypercubes that have the given dimensions.

A way to continue down this path might be to implement a new solver or to try 
iterate on the labelcheck so that it is not eagerly computed. Maybe there is a 
way to leverage on-demand DFA (or take inspiration from it) to make hypercube 
labels more efficient in a bottom-up solver

## Labels as Sets Using Membership Relation

Labels are sets and we use the lattice over sets using the subset relation as 
the order operator. Essentially, isMember(elt, Set) is true when elt is a
member of Set. 

### Find a Counterexample

The idea is to avoid computing joins or meets over either the input or output 
sets. There is no universal quantification in datalog, so we cannot do the 
check by quantifying over the inputs/outputs.

Instead we write a rule that finds a single counterexample, i.e., there exists 
one input with label `L_i` and output `L_o` such that `L_i notFlowsTo L_o`, and 
call this relation `notModuleLabelChecks`.
Then the overall check is roughly `moduleLabelChecks :- !notModuleLabelChecks`. 
This works fine for the case where there are no downgrades.

Note that this strategy differs substantially from normal approaches for enforcing IFC, 
so it would complicate proving security hyperproperties. It may be possible to 
address this by proving that these rules refine more conventional ones and then 
proving that the conventional rules enforce hyperproperties.

However, this strategy does not seem to work with downgrades. Here are two ways 
to try to do downgrades 

#### Expand the relation fieldHasLabel to include downgraded Labels
Here we keep the notModuleLabelChecks relation the same, but we add an 
additional for proving that a field has a label using downgrades.

Roughly this is
`fieldHasLabel(f, l) :- fieldHasLabel(f, l_old), ownsLabel(p, l_old),
                        saysDowngrades(p, f, l_old, l).`

Note that this means it's possible to prove a field has more than one label 
(the original one written by the programmer or determined by DFA, or the 
downgraded one). We want the check to "fail" if it does not go through with the 
original label, but try again with the downgraded label. With a single 
notModuleChecks relation, the check will fail because of the original label, 
but the "retry" does not happen because we have already proved failure. The 
next section tries to address this

#### Split the check into non-downgrading and downgrading cases
We can split the check by making the relation for assigning a label to a field 
using downgrades `fieldHasLabelDown` instead (and keeping `fieldHasLabel` for
the downgrading case. We can then either have two rules for proving
`notModuleLabelchecks` (one using `fieldHasLabel` and another having 
`fieldHasLabelDown`) or we can split it at the module level instead. We run 
into the same issue either way. 

Assuming we have `notModuleLabelChecks` and `notModuleLabelChecksDown` (where 
the downgrading one uses `fieldHaslabelDown` for the input) the top-level check
could be:

`moduleChecks :- !notModuleChecks ; !notModuleLabelChecks`

but this does not work for at least the following case: suppose we have a 
counter-example when using non-downgrading label, but there are no rules that 
permit downgrades. In this case, the check should fail, but it passes. As 
expected, !notModuleChecks is false, but because there are no downgrade rules, 
we cannot prove `fieldHasLabelDown` which means we cannot prove 
notModuleLabelChecks, so `!notModuleLabelChecks` passes.

### Compute Join over inputs, Compute meet over outputs
This approach does the check by computing the join over the labels of the 
inputs and showing that this flows to the meet over the label of the outputs. 
To handle downgrades, we can use two separate rules for proving that a 
field has a label (one with and one without downgrades). This will not have the 
same problem as the counterexample approach because the top-level check can 
just be roughly `checks :- checksWithDowngrades || checksWithoutDowngrades` -- 
since both rules are positive this does the desired "try and retry with 
downgrades" check.
