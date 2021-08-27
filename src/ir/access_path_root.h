#ifndef SRC_IR_ACCESS_PATH_ROOT_H_
#define SRC_IR_ACCESS_PATH_ROOT_H_

#include "absl/types/variant.h"
#include "src/common/logging/logging.h"

// The classes in this file describe the root of an AccessPath. At the moment,
// we have only two types of roots: a SpecRoot and a ConcreteRoot,
// distinguishing between AccessPaths that have been constructed relative to
// some abstract structure (such as a ParticleSpec) and those that have been
// constructed relative to a concrete structure (such as a Particle).

namespace raksha::ir {

// A SpecAccessPathRoot describes the uninstantiated case. An attempt to
// print it to a string will fail and it indicates that it is not yet
// instantiated.
class SpecAccessPathRoot {
 public:
  std::string ToString() const {
    LOG(FATAL) << "Attempted to print out an AccessPath before connecting it "
                  "to a fully-instantiated root!";
  }

  // A SpecAccessPathRoot is not instantiated.
  bool IsInstantiated() const {
    return false;
  }
};

// Represents the root of an access path connected to a fully-instantiated
// construct. We currently just wrap a string for this case to allow us
// flexibility to more fully describe the structure as the Raksha IR matures.
class ConcreteAccessPathRoot {
 public:
  ConcreteAccessPathRoot(std::string root_string)
    : root_string_(std::move(root_string)) {}

  // A ConcreteAccessPathRoot returns its inner root_string_ when ToString()
  // is called.
  std::string ToString() const {
    return root_string_;
  }

  // A ConcreteAccessPathRoot is instantiated.
  bool IsInstantiated() const {
    return true;
  }
 private:
  std::string root_string_;
};

// The generic AccessPathRoot. Contains an absl::variant holding the specific
// root type and delegates to it using absl::visit.
class AccessPathRoot {
 public:
  // Use a using type alias to define the absl::variant type describing the
  // specific root types. Prevents the types used in the constructor and the
  // field from getting out of sync.
  using RootVariant = absl::variant<SpecAccessPathRoot, ConcreteAccessPathRoot>;

  AccessPathRoot(RootVariant specific_root)
    : specific_root_(std::move(specific_root)) {}

  // Dispatch ToString to the specific kind of AccessPathRoot.
  std::string ToString() const {
    return absl::visit(
        [](const auto &variant){ return variant.ToString(); }, specific_root_);
  }

  // Dispatch IsInstantiated to the specific kind of AccessPathRoot.
  bool IsInstantiated() const {
     return absl::visit(
        [](const auto &variant){ return variant.IsInstantiated(); },
        specific_root_);
  }

 private:
  // The specific variant of the root.
  RootVariant specific_root_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_ROOT_H_
