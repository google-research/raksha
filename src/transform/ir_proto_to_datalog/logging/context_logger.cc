#include "src/transform/ir_proto_to_datalog/logging/context_logger.h"

#include <cassert>

namespace raksha::transform::logging {

void ContextLogger::init(std::ostream &base_ostream) {
  assert(!singleton_logger_);
  singleton_logger_ =
      std::unique_ptr<ContextLogger>(new ContextLogger(base_ostream));
}

void ContextLogger::open_context(const std::string context) {
  ctxt_header_stack_.push_back(context);
}

void ContextLogger::close_context() {
  ctxt_header_stack_.pop_back();
  if (num_headers_printed > ctxt_header_stack_.size()) {
    num_headers_printed = ctxt_header_stack_.size();
  }
}

void ContextLogger::print_indentation(const uint64_t context_depth) {
  for (uint64_t i = 0; i < context_depth; ++i) {
    base_ostream_ << "  ";
  }
}

void ContextLogger::print_headers() {
  assert(num_headers_printed <= ctxt_header_stack_.size());
  for (
      uint64_t idx = num_headers_printed;
      idx < ctxt_header_stack_.size();
      ++idx)
  {
    print_indentation(idx);
    base_ostream_ << "In " << ctxt_header_stack_.at(idx) << ":" << std::endl;
  }
  num_headers_printed = ctxt_header_stack_.size();
}

void ContextLogger::print_error(const absl::string_view str) {
  print_headers();
  print_indentation(ctxt_header_stack_.size());
  base_ostream_ << "Error: ";
  base_ostream_ << str;
  base_ostream_ << std::endl;
}

void ContextLogger::error(const absl::string_view str) {
  assert(ContextLogger::singleton_logger_);
  ContextLogger::singleton_logger_->print_error(str);
}

LoggingContext::LoggingContext(const std::string header) {
  // If we did not initialize the logger, we can't shut things down a nice way.
  // Just assert.
  assert(ContextLogger::singleton_logger_);
  ContextLogger::singleton_logger_->open_context(header);
}

LoggingContext::~LoggingContext() {
  assert(ContextLogger::singleton_logger_);
  ContextLogger::singleton_logger_->close_context();
}

ErrorStream ErrorStream::operator<<(
    const absl::string_view input_string) const {
  assert(ContextLogger::singleton_logger_);
  ContextLogger::singleton_logger_->print_error(input_string);

  return *this;
}
}  // namespace raksha::transform::logging
