//
// Created by Andrey Solovyev on 15/03/2023.
//

#pragma once

#include "../const_values.h"

#include <cstddef>
#include <memory_resource>
#include <array>


#ifndef MEMORY_RESOURCE_H
#define MEMORY_RESOURCE_H

namespace culib::memory {

  namespace details {
	template<std::size_t N, typename T>
	struct buf_wrapper_t {
		std::vector <std::byte> buffer;
		explicit buf_wrapper_t() : buffer(sizeof(T)*N + freq::const_values::k_allowance_for_container_own_data) {}
	};
  }//!namespace

  template<std::size_t N, typename T>
  struct mem_resource_t :
		  private details::buf_wrapper_t<N, T>,
		  public std::pmr::monotonic_buffer_resource {
	  explicit mem_resource_t() noexcept
			  : monotonic_buffer_resource(
			  	details::buf_wrapper_t<N, T>::buffer.data(),
			  	details::buf_wrapper_t<N, T>::buffer.size(),
//			  	std::pmr::get_default_resource())
			  	std::pmr::null_memory_resource())
	  { }
  };

}//!namespace

#endif //MEMORY_RESOURCE_H
