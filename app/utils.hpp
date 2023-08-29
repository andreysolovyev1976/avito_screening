//
// Created by Andrey Solovyev on 30/01/2023.
//

#pragma once

#include "types_requirements/numerics.h"

#include <cstddef>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <stdexcept>
#include <system_error>
#include <optional>

#ifndef BASE_UTILS_H
#define BASE_UTILS_H

namespace culib::utils {

#ifndef __cpp_concepts
  template<typename number_t, freq::requirements::IsArithmetic<number_t> = true>
#else
  template<culib::requirements::IsArithmetic number_t>
#endif
  std::string to_chars(number_t input) {
	  std::array<char, 32> chars;
	  if (auto [ptr, ec] = std::to_chars(chars.data(), chars.data()+chars.size(), input);
			  ec == std::errc()) {
		  return std::string(chars.data(), ptr);
	  }
	  else {
		  throw std::invalid_argument(std::make_error_code(ec).message());
	  }
  }



#ifndef __cpp_concepts
  template<typename number_t, freq::requirements::IsArithmetic<number_t> = true>
#else
  template<culib::requirements::IsArithmetic number_t>
#endif
  std::optional<number_t> from_chars(std::string_view str) {
	  number_t local_result;
	  auto const last = str.data() + str.size();

	  auto [ptr, ec]{std::from_chars(str.data(), last, local_result)};

	  if (ec == std::errc::invalid_argument
			  || ec == std::errc::result_out_of_range
			  || ptr != last
			  ) {
		  return std::nullopt;
	  }
	  return {local_result};
//	  throw std::invalid_argument("Attempt to convert not a number; ");
//	  throw std::invalid_argument("Out of bound; ");
  }


  inline
  std::vector<std::string_view> split(std::string_view str, char delim) {
	  std::vector<std::string_view> result;
	  result.reserve(100u); //arbitrary figure
	  std::size_t found_delim {0};
	  while (true) {
		  found_delim = str.find(delim);
		  auto substr = str.substr(0, found_delim);
		  if (not substr.empty()) result.push_back(substr);
		  if (found_delim == str.npos) break;
		  else str.remove_prefix(found_delim + 1);
	  }
	  return result;
  }


}//!namespace
#endif //BASE_UTILS_H
