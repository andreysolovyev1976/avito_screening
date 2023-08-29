/**
 * _________________________________________________________________________
 * \n\n
 *
 * Code was originally placed by K.Vladimirov
 * While working with this code some of its parts were modified from origin.
 * \n
 * _________________________________________________________________________
 * \n\n
 * source:
 * \n
 * https://sourceforge.net/p/cpp-lects-rus/code/HEAD/tree/trunk/cpp_code/threads/coe_queue.cc#l13
 * \n\n
 * _________________________________________________________________________
 * \n\n
 */

#pragma once

#include "packaged_task_wrapper.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

#ifndef FLEX_SIZE_TS_QUEUE_H
#define FLEX_SIZE_TS_QUEUE_H

namespace chrono = std::chrono;
using namespace std::chrono_literals;

namespace multi_threading {

  template<typename T>
  class ts_coe_queue { //keeping an original from K.Vladimirov

	  // fixed-size queue
	  // look it is unaligned...
	  std::queue<T> Buffer;
	  bool Done = false;
	  mutable std::mutex Mut;
	  std::condition_variable CondCons, CondProd;

	  // this interface cannot safely be public
	  bool empty() const { return Buffer.empty(); }
	  bool done() const { return Done; }

  public:
	  ts_coe_queue () = default;

	  ts_coe_queue (std::size_t) {}

	  ts_coe_queue(ts_coe_queue &&other)
			  :Buffer(std::move(other.Buffer))
	  {}

	  void push(T Data) {
		  std::unique_lock<std::mutex> Lk{Mut};
		  Buffer.push(std::move(Data));
		  Lk.unlock();
		  CondCons.notify_one();
	  }

	  T wait_and_pop() {
		  std::unique_lock<std::mutex> Lk{Mut};
		  CondCons.wait(Lk, [this] { return !empty() || done(); });
		  if (empty()) return T{}; //need this for case of empty, but b-a-a-a-a-d....
		  auto res = std::move(Buffer.front());
		  Buffer.pop();
		  Lk.unlock();
		  CondProd.notify_one();
		  return res;
	  }

	  void wake_and_done() {
		  std::unique_lock<std::mutex> Lk{Mut};
		  Done = true;
		  Lk.unlock();
		  CondCons.notify_one();
	  }

	  // only for extern use, locks NCur
	  // we need this to not stop consume too early
	  bool is_empty_and_done() const {
		  std::unique_lock<std::mutex> Lk{Mut};
		  bool res = empty() && Done;
		  return res;
	  }

	  std::size_t size() const {
		  return Buffer.size();
	  }
  };


  using ts_queue_t = ts_coe_queue<task_t>;


} //!namespace


#endif //FLEX_SIZE_TS_QUEUE_H
