module;

export module mwc_operating_system;

import mwc_definition;

export namespace mwc {
  namespace utility {
    enum class operating_system_et { e_linux, e_windows, e_mac_os, e_other, end };

    constexpr auto operating_system() -> operating_system_et {
      using enum operating_system_et;
#ifdef __linux__
      return e_linux;
#endif
#ifdef _WIN32
      return e_windows;
#endif
#ifdef __APPLE__
      return e_mac_os;
#endif
      return e_other;
    }
  }
}