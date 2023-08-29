//
// Created by Andrey Solovyev on 17/08/2023.
//

#pragma once

#include <future>
#include <vector>
#include <type_traits>

#ifndef FREQ_ASYNC_WRAPPER_H
#define FREQ_ASYNC_WRAPPER_H

namespace multi_threading {

  template<std::random_access_iterator iter_t, typename Callable, typename... Args>
  auto run_async (std::vector<std::pair<iter_t, iter_t>> const& pages, Callable callable, Args... args) {

	  using return_type = std::invoke_result_t<std::decay_t<Callable>, iter_t, iter_t, Args...>;
	  std::vector<std::future<return_type>> futures;
	  futures.reserve(pages.size());

	  if constexpr (std::is_same_v<return_type, void>) {
		  for (auto [b, e] : pages) {
			  futures.template emplace_back(std::async(callable, b, e, std::forward<Args>(args)...));
		  }
		  for (auto &f : futures) {
			  f.get();
		  }
	  }
	  else {
		  std::vector<return_type> result;
		  result.reserve(pages.size());
		  for (auto [b, e] : pages) {
			  futures.template emplace_back(std::async(callable, b, e, std::forward<Args>(args)...));
		  }
		  for (auto &f : futures) {
			  result.emplace_back(f.get());
		  }
		  return result;
	  }
  }

}//!namespace

#endif //FREQ_ASYNC_WRAPPER_H
