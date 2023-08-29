//
// Created by Andrey Solovyev on 16/08/2023.
//

#pragma once

#if defined (MT_TRIE) && defined(TRIE_ON_VECTOR) && !defined (TRIE_ON_ARENA)
#include "trie_vec.hpp"
#elif defined (MT_TRIE) && !defined(TRIE_ON_VECTOR) && defined (TRIE_ON_ARENA)
#include "trie_mem.hpp"
#endif
#include "../multi_threading/strand.hpp"
#include "../multi_threading/ts_queue.hpp"

#include <vector>
#include <utility>

namespace freq {


  template<typename data_t, typename hash_t>
  struct hash_table_t final {
	  std::vector<data_t> data;
	  hash_t hasher;

	  hash_table_t () = default;
	  hash_table_t (std::size_t sz)
			  :data(sz)
	  {}
  };


  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct ideal_hash_t final {
	  std::size_t operator()(char c, std::size_t shard_count) const {
		  if (shard_count==0) return 0; //single shard, shard_id == 0
		  auto block_size = abc_size/shard_count;
		  if (block_size==0) block_size = 1; //shard_count > abc_size
		  auto char_idx = c - const_values::k_abc_first;
		  auto shard_id = char_idx/block_size;
		  return shard_id>=shard_count ? shard_count-1 : shard_id;
	  }
  };




  template<std::size_t abc_size = const_values::k_default_abc_size>
  struct sharded_trie_t final {
	  hash_table_t<trie_shard_t<abc_size>, ideal_hash_t<abc_size>> shards;
	  std::vector<multi_threading::strand_t<multi_threading::ts_queue_t_flex>> strands;

	  explicit sharded_trie_t (std::size_t shard_count = abc_size) {
		  shards.data.reserve(shard_count);
		  strands.reserve(shard_count);
		  for (std::size_t i = 0; i != shard_count; ++i) {
			  shards.data.emplace_back(32'768);
			  strands.emplace_back(65'536);
		  }
	  }
	  std::pair<trie_shard_t<abc_size>*, std::size_t> operator()(char c)& {
		  auto hash_value = shards.hasher(c, shards.data.size());
		  return {&shards.data[hash_value], hash_value} ;
	  }
	  void insert(std::string_view key)  {
		  auto hash_value = shards.hasher(key[0], shards.data.size());
		  auto* trie_ptr = &shards.data[hash_value];
		  auto callable = [trie_ptr](std::string_view key){
			trie_ptr->insert(key);
			return 0;
		  };
		  strands[hash_value].template produce_task<int(std::string_view)>(callable, key);
	  }
	  std::vector<word::stat_t> traverse() const& {
		  std::vector<word::stat_t> res, tmp;
		  // res.reserve(some_number); //todo: optimizable
		  for (auto& shard: shards.data) {
//		  if (shard.size() > tmp.capacity()) tmp.reserve(shard.size()); //todo: fit this for trie_shard
			  tmp = shard.traverse();
			  res.insert(res.cend(), tmp.begin(), tmp.end());
			  tmp.clear();
		  }
		  return res;
	  }
	  void shutdown() &{
		  for (auto& strand : strands) {
			  strand.shutdown();
		  }
	  }
  };

}//!namespace
