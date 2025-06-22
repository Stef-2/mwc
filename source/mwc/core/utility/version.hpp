#pragma once

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  struct semantic_version {
    using version_t = uint8_t;
    using packed_version_t = uint32_t;

    static inline constexpr auto pack_offset = 0xFF;

    constexpr semantic_version(const packed_version_t a_packed_version) pre(a_packed_version != 0)
      pre(a_packed_version <= std::numeric_limits<packed_version_t>::max());
    constexpr semantic_version(const version_t a_major, const version_t a_minor, const version_t a_patch)
      pre(std::max({a_major, a_minor, a_patch}) > 0)
        pre(std::min({a_major, a_minor, a_patch}) <= std::numeric_limits<version_t>::max());

    [[nodiscard]] auto major() const -> version_t;
    [[nodiscard]] auto minor() const -> version_t;
    [[nodiscard]] auto patch() const -> version_t;

    // explicit conversion to a human readable string format
    [[nodiscard]] explicit operator const string_t() const;
    auto operator<=>(const semantic_version&) const = default;

    packed_version_t m_version {};
  };

  // implementation
  constexpr semantic_version::semantic_version(const packed_version_t a_packed_version) : m_version {a_packed_version} {}
  constexpr semantic_version::semantic_version(const version_t a_major, const version_t a_minor, const version_t a_patch)
  : m_version(vk::makeApiVersion(version_t {0}, a_major, a_minor, a_patch)) {}
}