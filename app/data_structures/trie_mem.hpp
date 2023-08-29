//
// Created by Andrey Solovyev on 16/08/2023.
//

#pragma once

#include "../word_stat.hpp"
#include "../memory/memresource.hpp"

#include <vector>

namespace freq {
  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct node_t final {
	  using allocator_type = std::pmr::polymorphic_allocator<char>;
	  std::pmr::vector<node_t*> next_chars {nullptr};
	  std::string_view word;
	  int freq {0};
	  bool is_leaf {false};

	  explicit node_t(allocator_type alloc = {});
	  node_t(node_t const& other, allocator_type alloc);
	  node_t(node_t &&other, allocator_type alloc);
	  bool has_children () const &;
  };

  template<std::size_t abc_size>
  node_t<abc_size>::node_t(allocator_type alloc)
		  : next_chars {abc_size, nullptr, alloc}
		  , freq {0}
		  , is_leaf {false}
  {}

  template<std::size_t abc_size>
  node_t<abc_size>::node_t(node_t const& other, allocator_type alloc)
		  : next_chars {other.next_chars, alloc}
		  , word (other.word)
		  , freq {0}
		  , is_leaf {other.is_leaf}
  {}

  template<std::size_t abc_size>
  node_t<abc_size>::node_t(node_t &&other, allocator_type alloc)
		  : next_chars {std::move(other.next_chars), alloc}
		  , word (other.word)
		  , freq {other.freq}
		  , is_leaf {other.is_leaf}
  {}

  template<std::size_t abc_size>
  bool node_t<abc_size>::has_children () const &{
	  for (auto node: next_chars){
		  if (node) return true;
	  }
	  return false;
  }

  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct trie_shard_t final {
	  //19 524'288
	  //20 1'048'576
	  //21 2'097'152
	  //22 4'194'304
	  //23 8'388'608
	  //24 16'777'216
	  //25 33'554'432
	  //todo: fits 26 shards, think on other sizes
	  culib::memory::mem_resource_t< (1 << 24), char> arena;
	  node_t<abc_size> root;
	  std::size_t sz;

	  trie_shard_t(std::size_t) //todo used an arg for keeping API
			  : arena()
			  , root (&arena)
			  , sz{0}
	  {}

	  //todo - this is dirty and cheap, move ctor is turned into init ctor
	  trie_shard_t(trie_shard_t &&)
			  : arena()
			  , root (&arena)
			  , sz {0}
	  {}

	  //no need in dtor, mem will be freed by using arena's dtor
#if 0
	  ~trie_shard_t(){
	  std::vector<node_t<abc_size>*> stack;
	  stack.reserve(sz);
	  node_t<abc_size>* v = &root;
	  stack.push_back(v);

	  while (!stack.empty()) {
		  auto top = stack.back();
		  stack.pop_back();
		  for (auto node : top->next_chars) {
			  if (!node) continue;
			  if (node->has_children()) {
				  stack.push_back(node);
			  }
			  else {
				  delete node;
			  }
		  }
	  }
  }
#endif
	  bool insert(std::string_view key) &{
		  int const key_size{static_cast<int>(key.size())};
		  node_t<abc_size> *v = &root;
		  int idx;
		  for (int i = 0; i < key_size; ++i) {
			  idx = key[i] - const_values::k_abc_first;
			  if (v->next_chars[idx] == nullptr) {
				  auto addr = arena.allocate(sizeof(node_t<abc_size>), alignof(std::size_t));
				  v->next_chars[idx] = new (addr) node_t<abc_size>;
			  }
			  v = v->next_chars[idx];
		  }
		  if (!v->is_leaf /** it means it will become on the after next line */) {
			  v->word = key;
		  }
		  v->is_leaf = true;
		  ++v->freq;
		  ++sz;
		  return v->is_leaf;
	  }

	  [[nodiscard]] std::vector<word::stat_t> traverse() const&{
		  std::vector<word::stat_t> res;
		  res.reserve(sz);
		  node_t<abc_size> const* v = &root;
		  std::vector<node_t<abc_size> const*> stack;
		  stack.reserve(sz);
		  stack.emplace_back(v);

		  while (!stack.empty()) {
			  auto top = stack.back();
			  stack.pop_back();
			  for (auto node: top->next_chars) {
				  if (!node) continue;
				  if (node->is_leaf) {
					  res.emplace_back(word::stat_t{node->word, node->freq});
				  }
				  if (node->has_children()) {
					  stack.push_back(node);
				  }
			  }
		  }
		  return res;
	  }
  };

}//!namespace
