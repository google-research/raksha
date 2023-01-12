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
//-----------------------------------------------------------------------------
//
// C/C++ interface for function callbacks that are executed when
// parsing SELECT statements from sqlite queries. These will be
// used to generate Raksha IR that represents these.
#ifndef SRC_FRONTENDS_SQLITE_SELECT_SNOOP_H_
#define SRC_FRONTENDS_SQLITE_SELECT_SNOOP_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Select Select;

void SnoopSelect(Select* select) { printf("%s\n", "hello select"); }

#ifdef __cplusplus
}
#endif

#endif  // SRC_FRONTENDS_SQLITE_SELECT_SNOOP_H_
