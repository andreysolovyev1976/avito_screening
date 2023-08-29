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

#include <functional>
#include <future>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef CPP_COURSE_PACKAGED_TASK_H
#define CPP_COURSE_PACKAGED_TASK_H

// std::move_only_function replacement while it is not ready

namespace multi_threading {

  template<typename T> class fire_once;

  template<typename R, typename... Args>
  class fire_once<R(Args...)> {
	  std::unique_ptr<void, void (*)(void*)> ptr{nullptr, +[](void*) {} };
	  R (* invoke)(void*, Args...) = nullptr;

  public:
	  fire_once() = default;
	  fire_once(fire_once&&) = default;
	  fire_once& operator=(fire_once&&) = default;

	  template<typename F>
	  fire_once(F&& f) {
		  auto pf = std::make_unique<F>(std::move(f));
		  invoke = +[](void* pf, Args&& ... args) -> R {
			F* f = reinterpret_cast<F*>(pf);
			return (*f)(std::forward<Args>(args)...);
		  };
		  ptr = {pf.release(), [](void* pf) {
					  F* f = reinterpret_cast<F*>(pf);
					  delete f;
				  }
		  };
	  }
	  R operator() (Args&& ... args)&& {
		  R ret = invoke(ptr.get(), std::forward<Args>(args)...);
		  clear();
		  return std::move(ret);
	  }

	  void clear() {
		  invoke = nullptr;
		  ptr.reset();
	  }

	  explicit operator bool() const { return static_cast<bool>(ptr); }
  };

// generic (type-erased) task to put on queue
// returns -1 if it is special signalling task (end of work for consumers)
// otherwise do what it shall and return 0
  using task_t = fire_once<int()>;

  template<typename Task, typename Callable,  typename... Args>
  auto create_task(Callable callable, Args&& ... args) {
	  std::packaged_task<Task> tsk{callable};
	  auto fut = tsk.get_future();
	  task_t t{
		  [ct = std::move(tsk), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
			  std::apply([ct = std::move(ct)](auto&& ... args) mutable { ct(args...); }, std::move(args));
			  return 0;
		  }
	  };
	  return std::make_pair(std::move(t), std::move(fut));
  }
}//!namespace

#endif //CPP_COURSE_PACKAGED_TASK_H
