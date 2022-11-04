#ifndef SRC_PARSER_SQL_PARSER_H__
#define SRC_PARSER_SQL_PARSER_H__

#include "absl/status/status.h"
#include "absl/strings/string_view.h"

namespace raksha::parser::sql {

absl::Status ParseStatement(std::string_view sql);

}

#endif
