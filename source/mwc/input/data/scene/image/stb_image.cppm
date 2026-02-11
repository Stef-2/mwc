module;

#include <stb/stb_image.h>

export module mwc_stb_image;

export namespace stb {
  using ::stbi_load_from_memory;
  using ::stbi_uc;
}