module;

export module mwc_contract_assertion;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace contract {
    [[nodiscard]] constexpr auto
    validate_data_size(const concepts::contiguous_storage_c auto& a_storage) {
      return a_storage.data() != nullptr and a_storage.size() > 0 and
             a_storage.size() < std::numeric_limits<size_t>::max();
    }
  }
}