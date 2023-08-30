//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include <cstddef>

#ifndef FREQ_CONST_VALUES_H
#define FREQ_CONST_VALUES_H

namespace freq::const_values {

  //Bible has 4047392 bytes, ~700'000 tokens, resulting in ~12'000 of unique words (58 tokens / 1 word), av word's length is 7.1
  //Book2 610856 bytes, ~100'000 tokens, 7747 unique words (12.5 tokens / 1 word) , av word's size == 8.02659
  //News 377109 bytes, file 54'000 tokens, 8387 unique words (6.3 tokens / 1 word) and av word size == 8.60391
  //Scientific paper 1 has 53161 bytes, 8'300 tokens and 1529 unique words (5.5 tokens / 1 word), av word size == 7.12099
  //Scientific paper 2 has 82199 bytes, 14'000 tokens, 2320 unique words (6 tokens / 1 word) and av word size == 7.05819
  //CIA's 1992 World FactBook contains 2473400 bytes, about 330'000 tokens, resulting in ~19'000 unique words (17.3 tokens / 1 word), has av word size == 7.24366
  //PG file contains 336'183'276 bytes, about 60'000'000 tokens, resulting in ~480'000 unique words (125 tokens / 1 word), has av word size == 9.2532
  //average is > 7 but to have some buffer

  static std::size_t const k_world_length {5u};
  static int const k_no_freq {0};


  //requirements
  static std::size_t const k_default_abc_size {26};
  static char const k_abc_first {'a'};
  static char const k_space {' '};
  static char const k_new_line {'\n'};


  static int const k_default_threads_count {8};
  static int const k_default_page_per_thread {8};

  //mt
  static int const k_min_symbols_count_for_multithreading {10'000}; //arbitrary value
  static std::size_t const k_allowance_for_container_own_data {32};
  static std::size_t const k_ts_queue_default_size {1'000};
}//!namespace

#endif //FREQ_CONST_VALUES_H
