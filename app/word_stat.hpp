//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include "types_requirements/numerics.h"
#include "const_values.h"

#include <string>
#include <string_view>
#include <cctype>
#include <execution>

#ifndef FREQ_WORD_H
#define FREQ_WORD_H

namespace freq::word {

  using stat_t = typename std::pair<std::string_view, int>;

  struct stat_greater_t {
	  bool operator () (stat_t const& a, stat_t const& b) const {
		  return a.second == b.second ? a.first < b.first : a.second > b.second;
	  }
  };
  struct stat_equal_t {
	  bool operator () (stat_t const& a, stat_t const& b) const {
		  return a.first == b.first && a.second == b.second ;
	  }
  };
  struct stat_less_t {
	  bool operator () (stat_t const& a, stat_t const& b) const {
		  return a.second == b.second ? a.first < b.first : a.second < b.second;
	  }
  };

  struct stat_hasher_t {
	  std::size_t operator () (stat_t const& a) const {
		  return std::hash<std::string_view>{}(a.first);
	  }
  };
}//!namespace


#endif //FREQ_WORD_H
