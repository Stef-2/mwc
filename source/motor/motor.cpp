import mtr_definition;
import mtr_log;
import mtr_sink;

import std;

int main()
{
  mtr::string_t wtf = {"asd"};
  std::println("ait");

  std::println("{0}", MTR_LOG_DIR);
  // mtr::log::wtf();
  mtr::log::sink_ct<mtr::log::sink_et::e_console, 1> wtf_sink;
  const bool aaa = MTR_DEBUG;
}