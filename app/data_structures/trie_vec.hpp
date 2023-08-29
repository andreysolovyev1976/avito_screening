//
// Created by Andrey Solovyev on 16/08/2023.
//

#pragma once

#include "../const_values.h"
#include "../word_stat.hpp"
#include <array>
#include <vector>

namespace freq {

  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct node_t final {
	  std::array<int, abc_size> next_chars;
	  std::string_view word;
	  int freq;
	  bool is_leaf;

	  explicit node_t()

	  : freq (0)
	  , is_leaf {false}
	  {
		  next_chars.fill(-1);
	  }
  };


  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct trie_shard_t final {
	  //15 32'768
	  //18 262'144
	  //19 524'288
	  //20 1'048'576
	  //21 2'097'152
	  //22 4'194'304
	  //23 8'388'608
	  //24 16'777'216
	  //25 33'554'432
	  //todo: fits 26 shards, think on other sizes
	  std::vector<node_t<abc_size>> data;

	  explicit trie_shard_t(std::size_t capacity)
	  {
		  data.reserve(capacity);
		  data.template emplace_back();
	  }

	  int insert(std::string_view key) & {
		  int const key_size{static_cast<int>(key.size())};
		  int idx, v{0};
		  for (int i = 0; i < key_size; ++i) {
			  idx = key[i] - const_values::k_abc_first;
			  if (data[v].next_chars[idx] == -1) {
				  data[v].next_chars[idx] = data.size();
				  data.emplace_back();
			  }
			  v = data[v].next_chars[idx];
		  }
		  if (!data[v].is_leaf /** it means it will become on the after next line */) {
			  data[v].word = key;
		  }
		  data[v].is_leaf = true;

		  ++data[v].freq;
		  return data[v].is_leaf;
	  }
	  [[nodiscard]] std::vector<word::stat_t> traverse() const& {
		  std::vector<word::stat_t> res;
		  res.reserve(data.size());
		  for (auto const& node : data) {
			  if (node.is_leaf) {
				  res.emplace_back(word::stat_t{node.word, node.freq});
			  }
		  }
		  return res;
	  }
	  void clear() & {
		  data.clear();
		  data.emplace_back();
	  }
  };

}//!namespace
