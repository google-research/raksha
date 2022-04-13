# Raksha 

[NOTE: This is not an officially supported Google product.]

Raksha[^1] is a project to build a unified framework for specifying and
enforcing privacy policies in a system with heterogeneous runtimes.

## Building

On a Linux Debian system, for example:

% sudo apt-get install bazel mcpp
% bazel build ...

At the time of writing (Apr 12, 2022) the latest version of Bazel is 5.1.1 and is known to work.

TODO(harsha-mandadi): Add troubleshooting tips specific to MacOS.

TODO(markww): mcpp must be pre-installed due to souffle searching the user's system path -- consider a fix to potentially allow more hermetic builds.

## Testing

% bazel test ...

## Running

TODO(markww): Elaborate.

[^1]: The name [Raksha](https://en.wikipedia.org/wiki/Raksha_(Vedic)) is used here in the  context of protection.
