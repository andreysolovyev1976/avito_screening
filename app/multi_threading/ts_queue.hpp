//-----------------------------------------------------------------------------
//
// Source code for MIPT masters course on C++
// Slides: https://sourceforge.net/projects/cpp-lects-rus
// Licensed after GNU GPL v3
//
//-----------------------------------------------------------------------------

/**
 * _________________________________________________________________________
 * \n\n
 *
 * Code was originally placed by K.Vladimirov\n
 * While working with this code some of its parts were modified or removed from origin.\n
 * _________________________________________________________________________
 * \n
 */

#pragma once

#include "packaged_task_wrapper.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>

#ifndef CPP_COURSE_TS_QUEUE_H
#define CPP_COURSE_TS_QUEUE_H

namespace chrono = std::chrono;
using namespace std::chrono_literals;

namespace multi_threading {

  template<typename T> class ts_queue {

	  // fixed-size queue
	  // look it is unaligned...
	  std::vector<T> Buffer;
	  int NCur = -1;
	  int NRel = 0; // start position
	  bool Done = false;
	  mutable std::mutex Mut;
	  std::condition_variable CondCons, CondProd;

	  // this interface cannot safely be public
	  bool full() const { return NCur>=static_cast<int>(Buffer.size()); }
	  bool empty() const { return NCur<0; }
	  bool done() const { return Done; }

  public:
	  ts_queue(std::size_t BufSize)
			  :Buffer(BufSize)
	  {
		  // prevent integer overflow cases
		  if (BufSize>(1 << 30))
			  throw std::runtime_error("unsupported buffer size");
	  }

	  ts_queue(ts_queue &&other)
			  :Buffer(std::move(other.Buffer))
	  {}

	  void push(T Data)
	  {
		  std::unique_lock<std::mutex> Lk{Mut};
		  CondProd.wait(Lk, [this] { return !full(); });

		  // exception safety
		  int NewCur = NCur+1;
		  Buffer[(NRel+NewCur)%Buffer.size()] = {std::move(Data)};
		  NCur = NewCur;
		  Lk.unlock();
		  CondCons.notify_one();
	  }

	  T wait_and_pop() {
		  std::unique_lock<std::mutex> Lk{Mut};
		  CondCons.wait(Lk, [this] { return !empty() || done(); });
		  if (empty()) return T{};  //need this for case of empty, but b-a-a-a-a-d....
		  auto res = std::move(Buffer[NRel%Buffer.size()]);
		  NRel = (NRel+1)%Buffer.size();
		  NCur -= 1;
		  Lk.unlock();
		  CondProd.notify_one();
		  return res;
	  }

	  void wake_and_done()
	  {
		  std::unique_lock<std::mutex> Lk{Mut};
		  Done = true;
		  Lk.unlock();
		  CondCons.notify_one();
	  }

	  // only for extern use, locks NCur
	  // we need this to not stop consume too early
	  bool is_empty_and_done() const
	  {
		  std::unique_lock<std::mutex> Lk{Mut};
		  bool res = (NCur<0) && Done;
		  return res;
	  }
  };


  using ts_queue_t = ts_queue<task_t>;

} //!namespace


#endif //CPP_COURSE_TS_QUEUE_H
