
//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//---------------------------------------------------------------------------

#include "functors.h"

#include <cfloat>
#include <cmath>
#include <string>

#include "src/common/logging/logging.h"

extern "C" {
souffle::RamDomain log_wrapper(souffle::SymbolTable *symbolTable,
                               souffle::RecordTable *recordTable,
                               souffle::RamDomain n) {
  CHECK(symbolTable && "NULL symbol table");
  CHECK(recordTable && "NULL record table");
  souffle::RamFloat n_float = souffle::ramBitCast<souffle::RamFloat>(n);
  auto result = std::log10(n_float);
  return symbolTable->encode(std::to_string(result));
}
}

souffle::RamDomain log_wrapper_cxx(souffle::SymbolTable *symbolTable,
                                   souffle::RecordTable *recordTable,
                                   souffle::RamDomain n) {
  return log_wrapper(symbolTable, recordTable, n);
}
