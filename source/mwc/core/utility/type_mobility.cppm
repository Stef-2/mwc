module;

export module mwc_type_mobility;

export namespace mwc {
  struct irreproducible_st {
    irreproducible_st() = default;
    irreproducible_st(const irreproducible_st&) = delete("non copyable type");
    auto operator=(const irreproducible_st&) -> irreproducible_st& = delete("non copyable type");
    irreproducible_st(irreproducible_st&&) noexcept = default;
    auto operator=(irreproducible_st&&) noexcept -> irreproducible_st& = default;
  };

  struct immovable_st {
    immovable_st() = default;
    immovable_st(immovable_st&&) noexcept = delete("non movable type");
    auto operator=(immovable_st&&) noexcept -> immovable_st& = delete("non movable type");
  };

  struct immobile_st : public irreproducible_st, public immovable_st {};
}
