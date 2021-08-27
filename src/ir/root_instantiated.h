#ifndef SRC_IR_ROOT_INSTANTIATED_H_
#define SRC_IR_ROOT_INSTANTIATED_H_

#include <string>

namespace raksha::ir {

// Associates a class which does not have a root for its AccessPath with a
// root.
template<class Unrooted>
class RootInstantiated {
 public:
  RootInstantiated(std::string root, Unrooted unrooted)
    : root_(std::move(root)), unrooted_(std::move(unrooted)) {}

  // Use the ToStringWithRoot method of the underlying unrooted class to
  // produce a string with the root contained in this wrapper class.
  std::string ToString() const {
    return unrooted_.ToStringWithRoot(root_);
  }

 private:
  std::string root_;
  Unrooted unrooted_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_ROOT_INSTANTIATED_H_
