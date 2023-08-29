//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#if defined (MT_TRIE) && !defined (MT_CONCURRENT_MAP) && !defined (ST)
#include "data_structures/sharded_trie.hpp"
#include "multi_threading/async_wrapper.hpp"
#elif !defined (MT_TRIE) && defined (MT_CONCURRENT_MAP) && !defined (ST)
#include "multi_threading/ts_map.hpp"
#include "multi_threading/async_wrapper.hpp"
#elif !defined(MT_TRIE) && !defined(MT_HASH) && defined (ST)
#include "data_structures/trie.hpp"
#endif

#include "const_values.h"
#include "word_stat.hpp"
#include "buffer.hpp"
#include "utils.hpp"

#include <algorithm>
#include <thread>

#ifndef FREQ_COUNTER_H
#define FREQ_COUNTER_H

namespace freq::counter {


  template<std::forward_iterator iter_t,
		  std::size_t abc_size = const_values::k_default_abc_size>

#if defined (MT_TRIE) && !defined (MT_CONCURRENT_MAP) && !defined (ST)
  auto get_frequencies(iter_t b, iter_t e, sharded_trie_t<abc_size> &sorted) {
#elif !defined (MT_TRIE) && defined (MT_CONCURRENT_MAP) && !defined (ST)
  auto get_frequencies(iter_t b, iter_t e, multi_threading::ts_map_t<std::string_view, int> &sorted) {
#elif !defined(MT_TRIE) && !defined(MT_HASH) && defined (ST)
	  auto get_frequencies(iter_t b, iter_t e, auto &sorted) {
#endif

	  auto l{b}, r{b};
	  while (r < e) {
		  l = std::find_if_not(l, e, [](auto a){ return a == const_values::k_space; });
		  r = std::find_if(l, e, [](auto a){ return a == const_values::k_space; });
		  if (l >= r) continue;
		  sorted.insert(std::string_view (l, r));
		  l = r;
	  }
  }

  constexpr auto get_threads_count = []() -> std::size_t {
	auto _ = std::thread::hardware_concurrency();
	return _ == 0 ? const_values::k_default_threads_count : _ * const_values::k_default_page_per_thread;
  };

  auto get_index(auto& buffer) {
	  using iter_t = typename std::decay_t<decltype(buffer)>::iterator;
	  using namespace freq;
	  using namespace text;


#if !defined (ST) && (defined(MT_TRIE) || defined(MT_CONCURRENT_MAP))
	  using namespace multi_threading;
	  std::size_t const threads_count = get_threads_count();
	  std::vector<std::pair<iter_t, iter_t>> paginated_input;
	  if (buffer.size() < const_values::k_min_symbols_count_for_multithreading) {
		  paginated_input.emplace_back(std::pair<iter_t, iter_t>{buffer.begin(), buffer.end()});
	  }
	  else {
		  paginated_input = paginate_by<pagination_type_t::page_count, iter_t>(buffer, threads_count);
	  }
	  run_async(paginated_input, to_abc<iter_t>);
#endif


#if defined (MT_TRIE) && !defined (MT_CONCURRENT_MAP) && !defined (ST)
	  std::size_t shard_count {threads_count};
	  sharded_trie_t index_tmp(shard_count);
	  get_frequencies<iter_t>(buffer.begin(), buffer.end(), index_tmp);
	  run_async(paginated_input, get_frequencies<iter_t>, std::ref(index_tmp));
	  index_tmp.shutdown();
#elif !defined (MT_TRIE) && defined (MT_CONCURRENT_MAP) && !defined (ST)
	  ts_map_t<std::string_view, int> index_tmp (510'000);
	  run_async(paginated_input, get_frequencies<iter_t>, std::ref(index_tmp));
#elif !defined (MT_TRIE) && !defined (MT_HASH) && defined (ST)
	  to_abc(buffer.begin(), buffer.end());
	  trie_t<const_values::k_default_abc_size> index_tmp(510'000);
	  get_frequencies<iter_t>(buffer.begin(), buffer.end(), index_tmp);
#endif

	  auto index = index_tmp.traverse();
	  std::sort(index.begin(), index.end(), word::stat_greater_t{});
	  return index;
  }

  auto dump (auto &&index, auto &file_processor) {
	  std::string number;
	  auto it {index.begin()}, ite {index.end()};
	  for (; it != ite; ++it) {
		  if (it->second == const_values::k_no_freq) break;
		  number = culib::utils::to_chars(it->second); //todo: optimizable, new array is used each time, instead of that only one can be used for all conversions
		  file_processor.write_output(number.data(), number.size());
		  file_processor.write_output(&const_values::k_space, 1);
		  file_processor.write_output(it->first.data(), it->first.size());
		  file_processor.write_output(&const_values::k_new_line, 1);
	  }
  }

}//!namespace

#endif //FREQ_COUNTER_H
