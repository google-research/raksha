#ifndef SRC_IR_ACCESS_PATH_ROOT_H_
#define SRC_IR_ACCESS_PATH_ROOT_H_

#include "absl/types/variant.h"
#include "absl/strings/str_join.h"
#include "src/common/logging/logging.h"
#include "third_party/arcs/proto/manifest.pb.h"

// The classes in this file describe the root of an AccessPath. At the moment,
// we have only two types of roots: a HandleConnectionSpecAccessPathRoot and
// a HandleConnectionAccessPathRoot, describing a HandleConnectionSpec in a
// ParticleSpec and a Handle in a Particle, respectively.
namespace raksha::ir {

// A HandleConnectionSpecAccessPathRoot describes the root as a
// HandleConnectionSpec in a ParticleSpec. We consider this uninstantiated as
// ParticleSpecs are abstract Particles which may be instantiated in multiple
// locations.
class HandleConnectionSpecAccessPathRoot {
 public:
  static HandleConnectionSpecAccessPathRoot CreateFromProto(
      const arcs::AccessPathProto_HandleRoot &handle_root_proto) {
    std::string particle_spec_name = handle_root_proto.particle_spec();
    CHECK(!particle_spec_name.empty())
      << "Expected a HandleRoot message to have a non-empty particle_spec.";
    std::string handle_connection_spec_name =
        handle_root_proto.handle_connection();
    CHECK(!handle_connection_spec_name.empty())
      << "Expected a HandleRoot message to have a non-empty handle_connection.";
    return HandleConnectionSpecAccessPathRoot(
        particle_spec_name, handle_connection_spec_name);
  }

  explicit HandleConnectionSpecAccessPathRoot(
      std::string particle_spec_name, std::string handle_connection_spec_name)
      : particle_spec_name_(std::move(particle_spec_name)),
        handle_connection_spec_name_(std::move(handle_connection_spec_name))
  {}

  // Do not allow printing a HandleConnectionSpecAccessPathRoot to datalog,
  // as it has not been fully instantiated.
  std::string ToString() const {
    LOG(FATAL) << "Attempted to print out an AccessPath before connecting it "
                  "to a fully-instantiated root!";
  }

  // Make this class back into an arcs HandleRoot proto.
  arcs::AccessPathProto_HandleRoot MakeProto() const {
    arcs::AccessPathProto_HandleRoot result;
    result.set_particle_spec(particle_spec_name_);
    result.set_handle_connection(handle_connection_spec_name_);
    return result;
  }

  // A HandleConnectionSpecAccessPathRoot has not been fully instantiated.
  bool IsInstantiated() const {
    return false;
  }

  const std::string &particle_spec_name() const {
    return particle_spec_name_;
  }

  const std::string &handle_connection_spec_name() const {
    return handle_connection_spec_name_;
  }

  bool operator==(const HandleConnectionSpecAccessPathRoot &other) const {
    return (particle_spec_name_ == other.particle_spec_name_) &&
      (handle_connection_spec_name_ == other.handle_connection_spec_name_);
  }

  template<typename H>
  friend H AbslHashValue(
      H h, const HandleConnectionSpecAccessPathRoot &hcs_access_path_root) {
    return H::combine(std::move(h), hcs_access_path_root.particle_spec_name_,
                      hcs_access_path_root.handle_connection_spec_name_);
  }

 private:
  std::string particle_spec_name_;
  std::string handle_connection_spec_name_;
};

// Represents the root of an access path connected to a HandleConnection on a
// fully-instantiated Particle. This contains three strings: the recipe,
// particle, and handle that identify the path to this handle from the root
// of the Arcs manifest data.
class HandleConnectionAccessPathRoot {
 public:
  HandleConnectionAccessPathRoot(
      std::string recipe_name, std::string particle_name,
      std::string handle_connection_name)
    : recipe_name_(recipe_name), particle_name_(particle_name),
      handle_connection_name_(handle_connection_name) {}


  // A HandleConnectionAccessPathRoot joins together its recipe, particle,
  // and handle name to generate its string.
  std::string ToString() const {
    return absl::StrJoin({
      recipe_name_, particle_name_, handle_connection_name_ }, ".");
  }

  // A HandleConnectionAccessPathRoot is instantiated.
  bool IsInstantiated() const {
    return true;
  }

  bool operator==(const HandleConnectionAccessPathRoot &other) const {
    return (recipe_name_ == other.recipe_name_) &&
      (particle_name_ == other.particle_name_) &&
      (handle_connection_name_ == other.handle_connection_name_);
  }

  template<typename H>
  friend H AbslHashValue(
      H h, const HandleConnectionAccessPathRoot &hc_access_path_root) {
    return H::combine(
        std::move(h), hc_access_path_root.recipe_name_,
        hc_access_path_root.particle_name_,
        hc_access_path_root.handle_connection_name_);
  }

 private:
  std::string recipe_name_;
  std::string particle_name_;
  std::string handle_connection_name_;
};

// A root representing a handle in a recipe.
class HandleAccessPathRoot {
 public:
  explicit HandleAccessPathRoot(
      std::string recipe_name, std::string handle_name)
      : recipe_name_(std::move(recipe_name)),
        handle_name_(std::move(handle_name)) {}

  // The string representation of a HandleAccessPathRoot is just
  // recipe_name_.handle_name_
  std::string ToString() const {
    return absl::StrJoin({recipe_name_, handle_name_}, ".");
  }

  // A HandleAccessPathRoot is instantiated.
  bool IsInstantiated() const {
    return true;
  }

  bool operator==(const HandleAccessPathRoot &other) const {
    return (recipe_name_ == other.recipe_name_) &&
      (handle_name_ == other.handle_name_);
  }

  template<typename H>
  friend H AbslHashValue(
      H h, const HandleAccessPathRoot &hc_access_path_root) {
    return H::combine(
        std::move(h), hc_access_path_root.recipe_name_,
        hc_access_path_root.handle_name_);
  }

 private:
  std::string recipe_name_;
  std::string handle_name_;
};

// The generic AccessPathRoot. Contains an absl::variant holding the specific
// root type and delegates to it using absl::visit.
class AccessPathRoot {
 public:
  // Use a using type alias to define the absl::variant type describing the
  // specific root types. Prevents the types used in the constructor and the
  // field from getting out of sync.
  using RootVariant = absl::variant<
      HandleConnectionSpecAccessPathRoot, HandleConnectionAccessPathRoot,
      HandleAccessPathRoot>;

  explicit AccessPathRoot(RootVariant specific_root)
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

  bool operator==(const AccessPathRoot &other) const {
    return specific_root_ == other.specific_root_;
  }

  template<typename H>
  friend H AbslHashValue(H h, const AccessPathRoot &access_path_root) {
    return H::combine(std::move(h), access_path_root.specific_root_);
  }

 private:
  // The specific variant of the root.
  RootVariant specific_root_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ACCESS_PATH_ROOT_H_
