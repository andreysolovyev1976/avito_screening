//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include "const_values.h"

#include <cmath>
#include <vector>
#include <array>
#include <utility>
#include <string>
#include <stdexcept>
#include <concepts>

#ifndef FREQ_BUFFER_H
#define FREQ_BUFFER_H


namespace freq::text {

  constexpr auto allowed_chars = []() -> std::array<char, 256> {
	std::array<char, 256> _ {'\0'};
	for (int i = 0; i != 256; ++i) {
		if (i >= 97 && i <= 122) {
			_[i] = static_cast<char>(i);
		}
		else if (i >= 65 && i <= 90) {
			_[i] = static_cast<char>(i + 32);
		}
		else {
			_[i] = ' ';
		}
	}
	return _;
  }();

  namespace details {

	inline
	auto page_size_counter = [](std::size_t buffer_size, std::size_t pages_count_ceiling) {
	  std::size_t const
			  tail {buffer_size%pages_count_ceiling},
			  expected_page_size {buffer_size/pages_count_ceiling}; //leaving out tail
	  if (tail == 0) return expected_page_size;
	  if (expected_page_size == 0) return (std::size_t)1u; //pages_count is greater than buffer size
	  std::size_t const expected_page_count {buffer_size/expected_page_size};
	  return (buffer_size + expected_page_count - tail)/pages_count_ceiling;
	};
  }//!namespace

  enum class pagination_type_t {
	  none,
	  page_size,
	  page_count,
  };

  //todo: push requirements into the func body therefore making its usage easier
  template<pagination_type_t by_what, std::random_access_iterator iter_t>
  std::vector<std::pair<iter_t, iter_t>> paginate_by (auto &buffer, std::size_t param) {
//	  page count ceiling < 2 || page size < 2
	  if (param < 2) {
		  return {{buffer.begin(), buffer.end()}};
	  }
	  if (buffer.empty()) return {};

	  std::size_t page_size;
	  if constexpr (by_what == pagination_type_t::page_count) {
		  page_size = details::page_size_counter(buffer.size(), param);
	  }
	  else if constexpr (by_what == pagination_type_t::page_size) {
		  page_size = param; //yeah, yeah, it is an extra copy, but so it is readable
	  }
	  else {
		  throw std::invalid_argument(
				  std::string(__PRETTY_FUNCTION__ ) + "\nparameter pagination_t::by_what is not set correctly\n"
		  );
	  }

	  std::vector<std::pair<iter_t, iter_t>> res;
	  if constexpr (by_what == pagination_type_t::page_count) {
		  res.reserve(param);
	  }
	  else if constexpr (by_what == pagination_type_t::page_size) {
		  res.reserve(buffer.size()/param + 1);
	  }
	  auto l{buffer.begin()}, r{buffer.begin()}, e{buffer.end()};
	  while (r < e) {
		  r = l + std::min(page_size, static_cast<std::size_t>(e - l));

		  //todo: remove this ugly thing, it prevents generalizing
		  if constexpr (by_what == pagination_type_t::page_count) {
			  while (r < e && *r != ' ') ++r;
		  }
		  res.emplace_back(std::pair<iter_t, iter_t>{l ,r});
		  l = r;
	  }
	  return res;
  }

  template<std::forward_iterator iter_t>
  auto to_abc (iter_t b, iter_t e) {
	  while (b < e) {
		  *b = allowed_chars[static_cast<unsigned char>(*b)];
		  ++b;
	  }
  }

  using buffer_t = typename std::vector<char>;
  buffer_t get_buffer_from (auto &file_processor);

}//!namespace

#endif //FREQ_BUFFER_H
