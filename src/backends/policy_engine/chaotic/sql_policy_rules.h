#ifndef THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_SQL_POLICY_RULES_H_
#define THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_SQL_POLICY_RULES_H_

#include <vector>

#include "absl/container/btree_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "src/frontends/sql/sql_ir.pb.h"
#include "src/frontends/sql/sql_ir.proto.h"

namespace raksha::backends::policy_engine {

class SqlPolicyRule {
 public:
  using Tag = size_t;
  using TagSet = absl::btree_set<Tag>;
  class Precondition {
   public:
    Precondition(absl::string_view arg_name, Tag tag)
        : arg_name_(arg_name), tag_(tag) {}

    absl::string_view arg_name() const { return arg_name_; }
    Tag tag() const { return tag_; }

   private:
    std::string arg_name_;
    Tag tag_;
  };

  class Result {
   public:
    enum class Action {
      AddConfidentialityTag,
      RemoveConfidentialityTag,
      AddIntegrityTag
    };

    Result(Action action, Tag tag) : action_(action), tag_(tag) {}

    Action action() const { return action_; }
    Tag tag() const { return tag_; }

   private:
    Action action_;
    Tag tag_;
  };

  SqlPolicyRule(absl::string_view rule_name, Result result,
                absl::flat_hash_map<std::string, TagSet> preconditions)
      : rule_name_(rule_name),
        result_(std::move(result)),
        preconditions_(std::move(preconditions)) {}

  Result result() const { return result_; }

  const TagSet* GetPreconditionTagSet(absl::string_view arg_name) const {
    auto find_result = preconditions_.find(arg_name);
    return find_result == preconditions_.end() ? nullptr
                                               : &(find_result->second);
  }

 private:
  // Field 0 is the rule name, field 1 is the result of the rule
  // executing, and field 2 is the preconditions
  std::string rule_name_;
  Result result_;
  // std::vector<Precondition> preconditions_;
  absl::flat_hash_map<std::string, TagSet> preconditions_;
};

// A policy checker based on computing fixpoints using chaotic iterations.
class SqlPolicyRules {
 public:
  using Tag = SqlPolicyRule::Tag;
  using TagSet = SqlPolicyRule::TagSet;
  SqlPolicyRules(const frontends::sql::ExpressionArena& arena);

  const SqlPolicyRule* GetRule(absl::string_view rule_name) const {
    auto find_result = rules_.find(rule_name);
    return (find_result == rules_.end()) ? nullptr : &find_result->second;
  }
  // Returns tag name if it exists.
  std::optional<absl::string_view> GetTagName(Tag tag) const {
    return (tag < tag_names_.size())
               ? std::optional<absl::string_view>(tag_names_[tag])
               : std::nullopt;
  }

 private:
  // Creates a new tag or returns one if it exists.
  Tag GetOrCreateTag(absl::string_view tag) {
    auto insert_result = tags_.insert({std::string(tag), tags_.size()});
    if (insert_result.second) {
      // Remember the tag name if a new entry was added.
      tag_names_.push_back(insert_result.first->first);
    }
    return insert_result.first->second;
  }

  absl::flat_hash_map<std::string, SqlPolicyRule> rules_;
  // Tags Table
  absl::flat_hash_map<std::string, Tag> tags_;
  std::vector<std::string> tag_names_;
};

}  // namespace raksha::backends::policy_engine

#endif  // THIRD_PARTY_RAKSHA_SRC_BACKENDS_POLICY_ENGINE_CHAOTIC_SQL_POLICY_RULES_H_
