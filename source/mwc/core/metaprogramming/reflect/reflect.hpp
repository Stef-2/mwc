#pragma once

//generate a string_view_t from the contents of this file
consteval auto file_contents() post(r : mwc::contracts::valid_data_size(r)) {
  // import the file contents into an array of characters
  // append the null terminator suffix so it can be parsed as a C char array
  static constexpr char file_contents[] = {
#embed __FILE__ suffix(, char{'\0'})
  };

  static_assert(false, string_view_t {file_contents});
  return string_view_t {file_contents};
}

// attempt to find the position of the first occurrence of [tp_string]
template <static_string_st tp_string>
consteval auto symbol_offset() post(r : r != string_view_t::npos) {
  return file_contents().find(tp_string);
}