//
// Created by Andrey Solovyev on 11/08/2023.
//

#pragma once

#include <mutex>
#include <shared_mutex>
#include <vector>
#include <unordered_map>
#include <utility>

#include "../word_stat.hpp"

#ifndef FREQ_MAP_CONCURRENT_H
#define FREQ_MAP_CONCURRENT_H

namespace multi_threading {
  template<typename Key, typename Value, typename Hash = std::hash<Key>>
  class ts_map_t final {
  public:
	  using bucket_t = std::unordered_map<Key, Value, Hash>;
	  using mapped_t = std::pair<Key, Value>;

	  struct write_access_t {
		  std::unique_lock<std::shared_mutex> lg;
		  Value& ref_to_value;
	  };

	  struct read_access_t {
		  std::shared_lock<std::shared_mutex> lg;
		  const Value& ref_to_value;
	  };

	  explicit ts_map_t(size_t bucket_count)
			  : bucket_count(bucket_count)
			  , data (bucket_count)
			  , mutexes(bucket_count) { }

	  write_access_t operator[](const Key& key)
	  {
		  size_t idx = get_bucket_index(key);
		  bucket_t& map = get_bucket(key);
		  return {std::unique_lock(mutexes[idx]), map[key]};
	  }

	  read_access_t at(const Key& key) const
	  {
		  if (!has(key)) throw std::out_of_range("no key");
		  size_t idx = get_bucket_index(key);
		  const bucket_t& map = get_bucket(key);

		  return {std::shared_lock(mutexes.at(idx)), map.at(key)};
	  }

	  bool has(const Key& key) const
	  {
		  const bucket_t& current_bucket = get_bucket(key);
		  return current_bucket.count(key);
	  }

	  void insert(Key key) & {
		  ++this->operator[](key).ref_to_value;
	  }

	  std::vector<freq::word::stat_t> traverse() const& {
		  std::vector<freq::word::stat_t> output;
		  for (size_t i = 0; i < bucket_count; i++) {
			  for (auto const& [w, f] : data[i]){
				  output.emplace_back(freq::word::stat_t{w, f});
			  }
		  }
		  return output;
	  }

  private:
	  Hash hasher;

	  size_t bucket_count;
	  std::vector<bucket_t> data;
	  mutable std::vector<std::shared_mutex> mutexes;

	  bucket_t& get_bucket(Key const& key) {
		  return data[get_bucket_index(key)];
	  }

	  const bucket_t& get_bucket(Key const& key) const {
		  return data[get_bucket_index(key)];
	  }

	  std::size_t get_bucket_index(Key const& key) const {
		  return hasher(key)%data.size();
	  }
  };

}//!namespace

#endif //FREQ_MAP_CONCURRENT_H
