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
	  std::vector<int> next_chars;
	  std::string_view word;
	  int freq;
	  bool is_leaf;

	  explicit node_t()
			  : next_chars(abc_size, -1)
			  , freq (0)
			  , is_leaf {false}
	  {}
  };

  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct trie_t final {
	  std::vector<node_t<abc_size>> data;

	  trie_t()
	  : data (1)
	  {}
	  trie_t(std::size_t capacity)
	  : data (1) {
		  data.reserve(capacity);
	  }

	  bool insert(std::string_view key) &  {
		  int const key_size{static_cast<int>(key.size())};
		  int idx, v{0};
		  for (int i = 0; i < key_size; ++i) {
			  idx = key[i] - const_values::k_abc_first;
			  if (data[v].next_chars[idx] == -1) {
				  data[v].next_chars[idx] = data.size();
				  data.template emplace_back();
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

	  [[nodiscard]] std::vector<word::stat_t> traverse() const&  {
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
