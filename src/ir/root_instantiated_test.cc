#include "src/ir/root_instantiated.h"

#include "absl/strings/str_cat.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir {

// This is the actual meat of the tests below. We use separate TEST_Ps for
// what is essentially the same test to work around the complexity of gtest's
// mechanism for making unit tests have parameterized types.
template<class T>
void CheckToStringOfRootInstantiatedEqualsToStringWithRoot(
    std::string root, T unrooted) {
  ASSERT_EQ(
      RootInstantiated<T>(root, unrooted).ToString(),
      unrooted.ToStringWithRoot(root));
}


class TestFact1 {
 public:
  std::string ToStringWithRoot(std::string root) const {
    return absl::StrCat("test_fact1(\"", root, "\").\n");
  }
};

class ToStringTest : public testing::TestWithParam<std::string> {};

TEST_P(ToStringTest, Fact1Test) {
  CheckToStringOfRootInstantiatedEqualsToStringWithRoot(
      GetParam(), TestFact1());
}

class TestFact2 {
 public:
  std::string ToStringWithRoot(std::string root) const {
    return absl::StrCat("test_fact2(\"", root, "\", \"", root ,"\", 5).\n");
  }
};

TEST_P(ToStringTest, Fact2Test) {
 CheckToStringOfRootInstantiatedEqualsToStringWithRoot(
    GetParam(), TestFact2());
}

static const std::string test_roots[] = {
    "r.p",
    "r.h",
    "root2.store",
    "recipe"
};

INSTANTIATE_TEST_SUITE_P(ToStringTest, ToStringTest,
                         testing::ValuesIn(test_roots) );

};  // namespace raksha::ir
