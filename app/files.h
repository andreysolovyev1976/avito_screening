//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include "buffer.hpp"

#include <filesystem>
#include <fstream>
#include <optional>

#ifndef FREQ_FILES_H
#define FREQ_FILES_H

namespace freq::files {

  bool reading_rights_exist (std::filesystem::path const&);
  bool writing_rights_exist (std::filesystem::path const&);
  bool is_file (std::filesystem::path const&);

  struct filenames_t {
	  std::filesystem::path input_file, output_file;
  };

  std::optional<filenames_t> check_files (int, char**);

  // it is stateful due to two streams, no need to create them every time
  // if this is used for multiple rounds of processing
  class processor_t final {
  public:
	  explicit processor_t(filenames_t&&);
	  [[nodiscard]] text::buffer_t read_input();
	  void write_output(char const *, std::size_t);

  private:
	  std::ifstream in;
	  std::ofstream out;
	  filenames_t filenames;
  };
}//!namespace

namespace freq::text {
  buffer_t get_buffer_from (auto &file_processor) {
	  return file_processor.read_input();
  }
}//!namespace



#endif //FREQ_FILES_H
