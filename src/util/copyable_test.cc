#include "src/common/testing/gtest.h"
#include "src/util/copyable.h"

namespace raksha::util {

class ExplicitCopyClass {
 public:
  explicit ExplicitCopyClass(int x) : x_(x) {}

  ExplicitCopyClass(ExplicitCopyClass &&other) = default;
  ExplicitCopyClass &operator=(ExplicitCopyClass &&other) = default;

  ExplicitCopyClass Copy() const {
    return ExplicitCopyClass(*this);
  }

  int x() const {
    return x_;
  }

 private:
  explicit ExplicitCopyClass(const ExplicitCopyClass &other) = default;

  int x_;
};

TEST(CopyableImplicitCastTest, CopyableImplicitCastTest) {
  Copyable<ExplicitCopyClass> copyable(ExplicitCopyClass(5));

  // Show that we can just assign Copyable<ExplicitCopyClass> to
  // ExplicitCopyClass.
  ExplicitCopyClass other = copyable;

  ASSERT_EQ(other.x(), 5);
}

TEST(CopyableStarOp, CopyableStarOp) {
  Copyable<ExplicitCopyClass> copyable(ExplicitCopyClass(23));
  ASSERT_EQ((*copyable).x(), 23);
}

TEST(CopyableConstStarOp, CopyableConstStarOp) {
  const Copyable<ExplicitCopyClass> copyable(ExplicitCopyClass(23));
  ASSERT_EQ((*copyable).x(), 23);
}

TEST(CopyableArrowOp, CopyableArrowOp) {
  Copyable<ExplicitCopyClass> copyable(ExplicitCopyClass(57));
  ASSERT_EQ(copyable->x(), 57);
}

TEST(CopyableConstArrowOp, CopyableConstArrowOp) {
  const Copyable<ExplicitCopyClass> copyable(ExplicitCopyClass(57));
  ASSERT_EQ(copyable->x(), 57);
}

}  // namespace raksha::util
