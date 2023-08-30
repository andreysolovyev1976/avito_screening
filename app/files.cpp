//
// Created by Andrey Solovyev on 11/08/2023.
//

#include "files.h"

#include <stdexcept>

namespace freq::files {

  bool reading_rights_exist (std::filesystem::path const& file_path) {
	  auto no_rights = std::filesystem::perms::none;
	  auto perms = std::filesystem::status(file_path).permissions();

	  bool reads_only_owner =
			  no_rights == (perms & std::filesystem::perms::group_read) &&
					  no_rights == (perms & std::filesystem::perms::others_read);
	  bool reads_nobody =
			  reads_only_owner &&
					  no_rights == (perms & std::filesystem::perms::owner_read);

	  if (reads_nobody) {
		  std::fprintf(stderr, "Error: %s - nobody has reading rights\n", file_path.c_str());
		  return false;
	  }
	  if (reads_only_owner) {
		  std::fprintf(stderr, "Warning: %s - only owner has reading rights\n", file_path.c_str());
	  }

	  return true;
  }

  bool writing_rights_exist (std::filesystem::path const& file_path) {
	  auto no_rights = std::filesystem::perms::none;
	  auto perms = std::filesystem::status(file_path).permissions();

	  bool writes_only_owner =
			  no_rights == (perms & std::filesystem::perms::group_write) &&
					  no_rights == (perms & std::filesystem::perms::others_write);
	  bool writes_nobody =
			  writes_only_owner &&
					  no_rights == (perms & std::filesystem::perms::owner_write);

	  if (writes_nobody) {
		  std::fprintf(stderr, "Error: %s - nobody has writing rights\n", file_path.c_str());
		  return false;
	  }
	  if (writes_only_owner) {
		  std::fprintf(stderr, "Warning: %s - only owner has writing rights\n", file_path.c_str());
	  }

	  return true;
  }

  bool is_file (std::filesystem::path const& file_path) {
	  bool is_not_regular_file = !std::filesystem::is_regular_file(file_path);
	  if (is_not_regular_file) {
		  std::fprintf(stderr, "Error: %s, doesn't fit, it is not a file\n", file_path.c_str());
		  return false;
	  }
	  return true;
  }

  std::optional<filenames_t> check_files (int argc, char** argv) {
	  if (argc < 3) {
		  std::fprintf(stderr, "Error: usage is this_file_name [input_file] [output_file]\n");
		  return std::nullopt;
	  }
	  filenames_t paths;
	  paths.input_file = argv[1];
	  if (!is_file(paths.input_file) || !reading_rights_exist(paths.input_file)) return std::nullopt;
	  paths.output_file = argv[2];
	  if (!is_file(paths.output_file) || !writing_rights_exist(paths.output_file)) return std::nullopt;

	  return {paths};
  }

  processor_t::processor_t(filenames_t&& fnms)
      : filenames(std::move(fnms)),
        in(filenames.input_file),
        out(filenames.output_file) {}

  text::buffer_t processor_t::read_input() {
	  text::buffer_t buffer(static_cast<size_t>(std::filesystem::file_size(filenames.input_file)), '\0');
	  in.read(buffer.data(), buffer.size());
	  if (!in) //todo process this correctly
		  throw std::runtime_error("can't read from input while reading file\n");
	  return buffer;
  }

  void processor_t::write_output(char const *data, std::size_t sz) {
	  if (!out) //todo process this correctly
		  throw std::runtime_error("can't write to output while dumping to file\n");
	  out.write(data, sz);
  }

}//!namespace