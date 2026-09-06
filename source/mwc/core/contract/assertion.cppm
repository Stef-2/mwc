module;

export module mwc_contract_assertion;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace contract {
    constexpr bool_t validate_storage(const concepts::contiguous_storage_c auto& a_storage) {
      return a_storage.data() != nullptr and a_storage.size() > 0 and a_storage.size() < std::numeric_limits<size_t>::max();
    }
    constexpr bool_t validate_enumerator(const concepts::enumerator_c auto a_enumerator) {
      template for (constexpr auto enumerator : std::meta::enumerators_of(std::meta::type_of(a_enumerator))) {
        if ([:enumerator:] == a_enumerator)
          return true;
      }

      return false;
    }
  }
}