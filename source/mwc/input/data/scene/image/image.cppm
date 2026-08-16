module;

export module mwc_input_image;

import mwc_input_resource;
import mwc_host_image;
import mwc_device_image;

export namespace mwc {
  namespace input {
    struct image_st : public resource_st {
      dynamic_host_image_st m_host_image;
      graphics::device_image_st m_device_image;
    };
  }
}