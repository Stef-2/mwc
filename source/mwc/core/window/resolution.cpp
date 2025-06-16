module;

module mwc_resolution;

namespace mwc {
  constexpr resolution_st::resolution_st() : m_width {0}, m_height {0} {}
  constexpr resolution_st::resolution_st(const extent_t a_width, const extent_t a_height)
  : m_width(a_width),
    m_height(a_height) {}
  /*constexpr resolution_st::resolution_st(const resolution_et a_resolution) : m_width {0}, m_height {0} {
      switch (a_resolution) {
        using enum resolution_et;
        case e_320x200 : m_width = {320}; m_height = {200}; return;
        case e_640x360 : m_width = {640}; m_height = {360}; return;
        case e_800x600 : m_width = {800}; m_height = {600}; return;
        case e_1024x768 : m_width = {1024}; m_height = {768}; return;
        case e_1280x720 : m_width = {1280}; m_height = {720}; return;
        case e_1280x800 : m_width = {1280}; m_height = {800}; return;
        case e_1280x1024 : m_width = {1280}; m_height = {1024}; return;
        case e_1360x768 : m_width = {1360}; m_height = {768}; return;
        case e_1366x768 : m_width = {1366}; m_height = {768}; return;
        case e_1440x900 : m_width = {1440}; m_height = {900}; return;
        case e_1536x864 : m_width = {1536}; m_height = {864}; return;
        case e_1600x900 : m_width = {1600}; m_height = {900}; return;
        case e_1680x1050 : m_width = {1680}; m_height = {1050}; return;
        case e_1920x1080 : m_width = {1920}; m_height = {1080}; return;
        case e_1920x1200 : m_width = {1920}; m_height = {1200}; return;
        case e_2048x1152 : m_width = {2048}; m_height = {1152}; return;
        case e_2048x1536 : m_width = {2048}; m_height = {1536}; return;
        case e_2560x1080 : m_width = {2560}; m_height = {1080}; return;
        case e_2560x1440 : m_width = {2560}; m_height = {1440}; return;
        case e_2560x1600 : m_width = {2560}; m_height = {1600}; return;
        case e_3440x1440 : m_width = {3440}; m_height = {1440}; return;
        case e_3840x2160 : m_width = {3840}; m_height = {2160}; return;
      }
    }*/
}