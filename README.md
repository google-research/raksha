# Raksha

[NOTE: This is not an officially supported Google product.]

Raksha[^1] is a project to build a unified framework for specifying and
enforcing privacy policies in a system with heterogeneous runtimes. Raksha
focuses on systems that are easily expressed as dataflow programs. There
is a large family of data processing systems that fall under this
umbrella:
[Deep Neural Networks (DNNs)](https://en.wikipedia.org/wiki/Deep_learning),
[MapReduce](https://en.wikipedia.org/wiki/MapReduce),
[Jetpack Compose](https://developer.android.com/jetpack/compose),
[Android IPC](https://developer.android.com/guide/components/aidl),
[IREE](https://google.github.io/iree/), SQL queries, etc. Even systems expressed
using imperative programming languages can be converted to equivalent dataflow
programs if needed.

## Specification Language

To state the obvious, automating enforcement of privacy policy requires a
machine-readable description of the behaviors of a system that needs to be
reasoned about. The actual implementation is obviously the most precise
specification of the system. However, the programming language used to
implement can be hard to reason about and contains more details than what
we might care about for privacy policy enforcement.

A well studied approach in formal verification is to abstract away unnecessary
details of the implementation using another high-level specification language
that we can precisely reason about. For lack of a better term, we will refer
to this specification language as an Intermediate Representation (IR) borrowing
from the world of compilers. We should define a common IR that captures the
following aspects:

 * _Data Model_: how the data is logically structured and organized. Ideally,
   these data models should be associated with a type system as well.
 * _Computations_: how the system uses existing data to produce new data.
 * _Execution Schedule_: when the computations are performed.
 * _Policy_: what behaviors are allowed. Typically, these would appear as some
   kind of predicates in an expressive logic (e.g., First order logic, Temporal
   Logic, etc.).

Having a common IR to describe different systems provides a vocabulary to
reason about the behavior of individual systems as well as the interactions
between them. There are other languages such as
[TLA+](https://lamport.azurewebsites.net/tla/tla.html) and
[Alloy](https://alloytools.org/) that are widely used in the research as well as
[industrial settings](https://lamport.azurewebsites.net/tla/industrial-use.html).

## Architecture

Given an IR, we envsion an architecture that is illustrated below, consisting
of multiple frontends, a middle-end, and multiple backends:

<img src="https://raw.githubusercontent.com/google-research/raksha/main/images/raksha_architecture.svg" alt="Raksha Architecture" width="600px"/>

Note that this architecture doesn’t have to be centralized and can be
distributed if we use the common IR as a way to communicate between
distributed components.

### Frontend
A frontend is responsible for generating the IR for the system that needs
to be reasoned about. There will be a frontend for each system that we want
to reason about.  As mentioned earlier, by specifying the behavior of
different systems in a common IR, we gain the ability to reason about
interactions between different systems.

To guarantee safety of the reasoning, the frontend should ensure that the
behaviors modeled by the IR is a superset of the actual behaviors manifested
by the actual implementation as shown below:

<img src="https://raw.githubusercontent.com/google-research/raksha/main/images/modeled_behaviors.svg" alt="Modeled Behaviors" width="300px"/>


Each frontend will need to take into account the features and intricacies of
their respective systems (including the runtime) to ensure that this
over-approximating property holds. This architecture provides flexibility for
each frontend to choose its own precision/efficiency trade-offs as they see
fit. The precision of the reasoning performed using the generated IR is
determined by how close the green area is to the blue area in the above figure.

The flip side of over-approximating the behavior is that there is a possibility
of false positives when it comes to policy verification:
<table>
  <tr>
    <td>No Policy Violation</td>
    <td>Real Policy Violation</td>
    <td>False positive</td>
  </tr>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/google-research/raksha/main/images/no_policy_violation.svg" alt="No Policy Violation" width="300px"/>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/google-research/raksha/main/images/real_policy_violation.svg" alt="Policy Violation" width="300px"/>
    </td>
    <td>
      <img src="https://raw.githubusercontent.com/google-research/raksha/main/images/false_positive.svg" alt="False Positive" width="300px"/>
    </td>
  </tr>
</table>

We could leverage standard techniques in formal verification literature to
limit the effects of false positives.

### Middle-End
The middle end would consist of analysis and transformation phases that work
exclusively on the IR.  Some examples of such analysis and transformations
are as follows:
 * Type Inference/Dataflow analysis
 * Optimizations (e.g, pipeline optimizations in MapReduce)
 * IR normalization (e.g., converting predicates to DNF form)
 * ...

### Backend
There will be multiple kinds of backends that operate on the IR and focus on
different aspects. Note that, in compilers, a backend typically refers to the
code generator for different hardware architectures. However, we are using the
term “backend” more broadly here. Here are some possible examples:

#### Verification Engines
These backends analyze the IR and verify that no policies are violated. These
verification engines could be based on well-known formal verification techniques
like abstract interpretation and model checking, which have different precision
and efficiency trade offs. There are also a lot of tools that we could leverage
like SMT solvers, theorem provers, datalog solvers, etc.

#### Code Generators
Code generators would take the IR and generate artifacts that satisfy all the
constraints specified by the policy. An example of such a backend would be a
synthesizer that generates a java/kotlin implementation that matches the
behaviors specified with the IR.

#### Task Schedulers/Planners
An example of such a backend is a planner that analyzes the IR and splits them
into different subtasks that need to be scheduled on different runtimes. e.g.,
an application that contains a mix of TF graphs and SQL Queries could be
split up into their respective parts and executed on the respective runtimes
along with the necessary communication.

#### Execution Engines
It is conceivable that one could generate runtimes that interpret the IR and
implement the modeled semantics/behaviors.

## Building

On a Linux Debian system, for example:

```
% sudo apt-get install bazel mcpp
% bazel build ...
```

At the time of writing (Apr 12, 2022) the latest version of Bazel is 5.1.1 and is known to work.

TODO(harsha-mandadi): Add troubleshooting tips specific to MacOS.

TODO(markww): mcpp must be pre-installed due to souffle searching the user's system path -- consider a fix to potentially allow more hermetic builds.

## Testing

% bazel test ...

## Running

TODO(markww): Elaborate.

[^1]: The name [Raksha](https://en.wikipedia.org/wiki/Raksha_(Vedic)) is used here in the context of protection.
