//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include "multi_threading/async_wrapper.hpp"
#include "const_values.h"
#include "word_stat.hpp"
#include "buffer.hpp"
#include "utils.hpp"

#include <algorithm>
#include <thread>

#ifndef FREQ_COUNTER_SHISHKOV_H
#define FREQ_COUNTER_SHISHKOV_H


namespace freq::counter {

  constexpr auto get_threads_count = []() -> std::size_t {
	auto _ = std::thread::hardware_concurrency();
	return _ == 0 ? const_values::k_default_threads_count : _ * const_values::k_default_page_per_thread;
  };

  auto paginate (auto & buffer) {
	  using namespace text;
	  using iter_t = typename std::decay_t<decltype(buffer)>::iterator;

	  std::size_t const threads_count = get_threads_count();
	  std::vector<std::pair<iter_t, iter_t>> paginated_input;

	  if (buffer.size() < const_values::k_min_symbols_count_for_multithreading) {
		  paginated_input.emplace_back(std::pair<iter_t, iter_t>{
				  (buffer.begin()), buffer.end()});
	  }
	  else {
		  paginated_input = paginate_by<pagination_type_t::page_count, iter_t>(buffer, threads_count);
	  }
	  return paginated_input;
  }

  template<std::forward_iterator iter_t>
  auto process_page(iter_t b, iter_t e) {
	  auto split = culib::utils::split(std::string_view{b,e}, const_values::k_space);
	  std::unordered_map<std::string_view, int> result;
	  for (std::string_view word : split) {
		++result[word];
	  }
	  return result;
  }

  auto merge(auto const& a, auto const& b) {
	  int L{static_cast<int>(a.size())}, R {static_cast<int>(b.size())};
	  std::vector<word::stat_t> result;
	  result.reserve(L + R);
	  for (int l{0}, r{0}; l < L || r < R; ) {
		  if (r >= R || (l < L && a[l].first < b[r].first )) {
			  if (!result.empty() && result.back().first == a[l].first)
				  result.back().second += a[l++].second;
			  else result.emplace_back(a[l++]);
		  }
		  else {
			  if (!result.empty() && result.back().first == b[r].first)
				  result.back().second += b[r++].second;
			  else result.emplace_back(b[r++]);
		  }
	  }
	  return result;
  }


  auto get_index(auto& buffer) {
	  using iter_t = typename std::decay_t<decltype(buffer)>::iterator;
	  using namespace freq;
	  using namespace text;
	  using namespace multi_threading;

	  auto paginated_input = paginate(buffer);

	  run_async(paginated_input, to_abc<iter_t>);
	  auto interim_results = run_async(paginated_input, process_page<iter_t>);

	  while (interim_results.size() > 1) {
		std::vector<std::future<std::unordered_map<std::string_view, int>>> futures;
		auto pop_back = [&interim_results] { 
			auto ret = std::move(interim_results.back()); 
			interim_results.pop_back();
			return ret;
		};
		while (interim_results.size() > 1) {
			futures.push_back(std::async([left = pop_back(), right = pop_back()]() mutable {
				for (auto [w, c] : right) {
					left[w] += c;
				}
				return std::move(left);
			}));
		}
		for (auto& f : futures) {
			interim_results.push_back(f.get());
		}
	  }

	  if (interim_results.size() != 1) {
		throw std::runtime_error("Error in Shishkov merge");
	  }
	  std::vector<word::stat_t> result(interim_results[0].begin(), interim_results[0].end());
	  std::sort(result.begin(), result.end(), word::stat_greater_t{});
	  return result;
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


#endif //FREQ_COUNTER_SHISHKOV_H
