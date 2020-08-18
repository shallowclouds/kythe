/*
 * Copyright 2020 The Kythe Authors. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "plugin.h"

#include "kythe/cxx/indexer/proto/vname_util.h"

namespace kythe {
namespace lang_textproto {

absl::Status ExamplePlugin::AnalyzeStringField(
    PluginApi* api, const proto::VName& file_vname,
    const google::protobuf::FieldDescriptor& field,
    std::vector<StringToken> tokens) {
  // Create an anchor spanning the full range of the string value.
  const char* begin = tokens.front().source_text.data();
  const char* end = tokens.back().source_text.end();
  const absl::string_view full_span = absl::string_view(begin, end - begin);
  const proto::VName anchor_vname =
      api->CreateAndAddAnchorNode(file_vname, full_span);

  auto target_vname = ::kythe::lang_proto::VNameForDescriptor(
      &field,
      [api](absl::string_view path) { return api->VNameForRelPath(path); });

  std::string full_value;
  for (const auto& t : tokens) {
    full_value += t.parsed_value;
  }

  LOG(ERROR) << "[Example Plugin] String value:" << full_value;
  LOG(ERROR) << "[Example Plugin] String value full span: " << full_span;

  // Add a ref edge from the anchor to the proto field descriptor.
  api->recorder()->AddEdge(VNameRef(anchor_vname), EdgeKindID::kRef,
                           VNameRef(target_vname));

  return absl::OkStatus();
}

}  // namespace lang_textproto
}  // namespace kythe
