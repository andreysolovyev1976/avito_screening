//
// Created by Andrey Solovyev on 19/08/2023.
//

#pragma once

#include "packaged_task_wrapper.hpp"

#include "../const_values.h"
#include <thread>
#include <cstdio>

#ifndef FREQ_SHARD_THREADING_H
#define FREQ_SHARD_THREADING_H

namespace multi_threading {

//  void myhandler() {
//	  std::printf("\nterminate happened\n");
//	  abort();
//  }
//  std::set_terminate(myhandler);

  template <typename queue_t>
  struct strand_t final {
  public:
	  explicit strand_t (std::size_t q_size = freq::const_values::k_ts_queue_default_size);
	  ~strand_t();
	  strand_t(strand_t &&) = default;

	  strand_t(strand_t const&) = delete;
	  strand_t& operator = (strand_t const&) = delete;
	  strand_t& operator = (strand_t &&) = delete;

	  template <typename Task, typename Callable, typename... Args>
	  void produce_task (Callable callable, Args... args) &;
	  void consume_task () &;

	  void shutdown() &;

	  queue_t q;
	  std::jthread jth;
  };


  template <typename queue_t>
  template <typename Task, typename Callable, typename... Args>
  void strand_t<queue_t>::produce_task (Callable callable, Args... args) & {
	  auto [task, future] = create_task<Task>(callable, std::forward<Args>(args)...);
	  q.push(std::move(task));
  }

  template <typename queue_t>
  strand_t<queue_t>::strand_t (std::size_t q_size)
		  : q (q_size)
		  , jth {&strand_t::consume_task, this}
  {}

  template <typename queue_t>
  strand_t<queue_t>::~strand_t() {
	  shutdown();
  }

  template <typename queue_t>
  void strand_t<queue_t>::shutdown() & {
	  task_t sentinel{[] () { return 42; }};
	  q.push(std::move(sentinel));
	  if (jth.joinable()) {
		  jth.join();
	  }
  }

  template <typename queue_t>
  void strand_t<queue_t>::consume_task () & {
	  using namespace std::chrono_literals;
	  try {
		  for (;;) {
			  if (q.is_empty_and_done()) break;
			  auto cur = q.wait_and_pop();
			  if (!cur) {
				  std::this_thread::yield();
				  continue;
			  }
			  int res = std::move(cur)();
			  if (res==42) {
				  break;
			  }
		  }
	  } catch (std::exception &e) {
		  std::printf("\n%s\n", e.what());
		  std::terminate();
	  } catch (...) {
		  std::printf("\n%s\n", "Something wrong in thread function");
		  std::terminate();
	  }
  }
}//!namespace


#endif //FREQ_SHARD_THREADING_H
