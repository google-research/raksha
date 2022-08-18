
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
#include <cmath>
#include <cfloat>
#include <string>

extern "C" {

  const char* log_wrapper(const char* n) {
      std::string s(n);
      float f = stof(s);

      // TODO(#665): Need to fix. This way of converting string to char* leaks memory
      // but it's currently the least worst option.
      std::string strResult = std::to_string(std::log10(f));
      char * charResult = new char[strResult.size() + 1];
      std::copy(strResult.begin(), strResult.end(), charResult);
      charResult[strResult.size()] = '\0';
      return charResult;
  }
}
