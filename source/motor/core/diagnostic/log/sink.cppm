module;

export module mtr_sink;

import mtr_bit;
import mtr_enum_bitwise_operators;

import std;

export namespace mtr
{
  namespace log
  {
    class sink_ct
    {
      public:
      enum class type_et
      {
        e_console = utility::bit<1>(),
        e_string = utility::bit<2>(),
        e_file = utility::bit<3>()
      };

      struct configuration_st
      {
        type_et m_type;
        std::ostream& m_stream;
      };
    };
  }
}