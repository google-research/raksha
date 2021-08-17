#ifndef SRC_TRANSFORM_IR_PROTO_TO_DATALOG_LOGGING_CONTEXT_LOGGER_H_
#define SRC_TRANSFORM_IR_PROTO_TO_DATALOG_LOGGING_CONTEXT_LOGGER_H_

#include "absl/strings/string_view.h"

#include <ostream>
#include <memory>
#include <string>
#include <vector>

namespace raksha::transform::logging {

class ContextLogger;

class ErrorStream {
 public:
  ErrorStream operator<<(const absl::string_view) const;

  friend class ContextLogger;

 private:
  ErrorStream() = default;
};

class ContextLogger {
 public:

  // ContextLogger is neither copyable nor movable.
  ContextLogger(const ContextLogger &) = delete;
  ContextLogger &operator=(const ContextLogger &) = delete;

  static void init(std::ostream &base_ostream);
  static void error(absl::string_view str);

  friend class LoggingContext;
  friend class ErrorStream;
 private:
  explicit ContextLogger(std::ostream &base_ostream)
    : base_ostream_(base_ostream),
      ctxt_header_stack_(),
      num_headers_printed(0) {};

  std::ostream &base_ostream_;
  std::vector<std::string> ctxt_header_stack_;
  uint64_t num_headers_printed;
  static std::unique_ptr<ContextLogger> singleton_logger_;
  void open_context(std::string context);
  void close_context();
  void print_headers();
  void print_indentation(uint64_t context_depth);
  void print_error(absl::string_view);
};

class LoggingContext {
 public:
  explicit LoggingContext(const std::string header);
  ~LoggingContext();
};

}  // namespace raksha::transform::logging

#endif  // SRC_TRANSFORM_IR_PROTO_TO_DATALOG_LOGGING_CONTEXT_LOGGER_H_
