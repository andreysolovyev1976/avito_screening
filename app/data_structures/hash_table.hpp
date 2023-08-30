//
// Created by Andrey Solovyev on 13/08/2023.
//

#pragma once

#include <vector>
#include <utility>
#include <iterator>
#include <optional>
#include <thread>
#include <functional>

#ifndef FREQ_HASH_TABLE_HPP
#define FREQ_HASH_TABLE_HPP

namespace freq {

  //todo: add a requirement that hash is a hash, and operator bool for Value
  template<typename Key, typename Value, typename Hash = std::hash<Key>>
  class hash_table_t final {
  private:
	  using mapped_type = std::pair<Key, Value>;

	  using iter_t = typename std::vector<mapped_type>::iterator;
	  using c_iter_t = typename std::vector<mapped_type>::const_iterator;
	  using cr_iter_t = typename std::vector<mapped_type>::const_reverse_iterator;
	  using r_iter_t = typename std::vector<mapped_type>::reverse_iterator;

  public:
	  //19 524'288
	  //20 1'048'576
	  //21 2'097'152
	  //22 4'194'304
	  //23 8'388'608
	  //24 16'777'216
	  //25 33'554'432
	  //todo: make it resizable
	  hash_table_t (std::size_t capacity) : k_size (capacity) {
		  hash_table.resize(k_size);
	  }
	  Value& operator [] (Key key) {
		  if (auto found = find(key); found != hash_table.end()) {
			  found->first = key;
			  return found->second;
		  }
		  else {
			  throw std::runtime_error("\nhash table is too small to handle what you want\n");
		  }
	  }
	  bool insert(Key key, Value value) & {
		  if (auto found = find(key); found != hash_table.end()) {
			  add(found, key, value);
			  return true;
		  }
		  else {
			  return false;
		  }
	  }
	  [[nodiscard]] std::vector<mapped_type> release () & {
		  return std::move(hash_table);
	  }
	  c_iter_t cbegin() const & {
		  return hash_table.cbegin();
	  }
	  c_iter_t cend() const & {
		  return hash_table.cend();
	  }
	  cr_iter_t crbegin() const & {
		  return hash_table.crbegin();
	  }
	  cr_iter_t crend() const & {
		  return hash_table.crend();
	  }
	  iter_t begin() & {
		  return hash_table.begin();
	  }
	  iter_t end() & {
		  return hash_table.end();
	  }
	  r_iter_t rbegin() & {
		  return hash_table.rbegin();
	  }
	  r_iter_t rend() & {
		  return hash_table.rend();
	  }
  private:
	  Hash hash_func;
	  std::size_t const k_size; //capacity
	  std::vector<mapped_type> hash_table;

	  iter_t find(Key key) & {
		  auto h = hash(key);
		  auto step = h;
		  step |= 1;

		  for (std::size_t i = 0; i != k_size; ++i) {
			  if (!has_value(h) || is_same_key(h, key)) {
				  return hash_table.begin() + h;
			  }
			  h = (h + step) % k_size;
		  }
		  return hash_table.end();
	  }
	  bool add(iter_t where_to, Key key, Value value) & {
		  *where_to = {key, value};
		  return true;
	  }
	  std::size_t hash(Key external_key) const {
		  return hash_func(external_key) % k_size;
	  }
	  bool has_value (std::size_t hash_key) const {
		  return (bool)hash_table[hash_key].second;
	  }
	  bool is_same_key (std::size_t hash_key, Key external_key) const {
		  return hash_table[hash_key].first == external_key;
	  }
  };

}//!namespace


#endif //FREQ_HASH_TABLE_HPP