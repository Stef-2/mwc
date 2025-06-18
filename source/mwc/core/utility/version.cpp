#include "mwc/core/utility/version.hpp"

namespace mwc {
  auto semantic_version::major() const -> const version_t {
    return static_cast<version_t>(vk::apiVersionMajor(m_version));
  }

  auto semantic_version::minor() const -> const version_t {
    return static_cast<version_t>(vk::apiVersionMinor(m_version));
  }

  auto semantic_version::patch() const -> const version_t {
    return static_cast<version_t>(vk::apiVersionPatch(m_version));
  }
  semantic_version::operator const string_t() const {
    return std::to_string(major()) + ':' + std::to_string(minor()) + ':' + std::to_string(patch());
  }
}