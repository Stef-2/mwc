module;

export module mwc_contract_assertions;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace contracts {
    [[nodiscard]] constexpr auto
    valid_data_size(const concepts::data_size_storage_c auto& a_storage) {
      return a_storage.data() != nullptr and a_storage.size() > 0 and
             a_storage.size() < std::numeric_limits<size_t>::max();
    }
  }
}