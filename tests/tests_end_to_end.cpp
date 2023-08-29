//
// Created by Andrey Solovyev on 16/08/2023.
//

#include "../code_branch_selector.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

#if (defined (MT_TRIE) || defined (MT_CONCURRENT_MAP) || defined (ST)) && !defined (MT_SHISHKOV)
#include "../app/counter.hpp"
#elif !defined (MT_TRIE) && !defined (MT_CONCURRENT_MAP) && !defined (ST) && defined (MT_SHISHKOV)
#include "../app/counter_shishkov.hpp"
#endif
#include "../app/files.h"

using namespace freq;

TEST(end_to_end, arbitrary_input) {
	std::stringstream ss;

	auto t_start = std::chrono::high_resolution_clock::now();

	files::filenames_t file_paths;
	file_paths.input_file = std::filesystem::path{"../../test_data/inputs/pg"};
	file_paths.output_file = std::filesystem::path{"../../test_data/outputs/output.txt"};
	files::processor_t file_processor(std::move(file_paths));

	auto t_start_buff = std::chrono::high_resolution_clock::now();

	auto buffer = text::get_buffer_from(file_processor);

	ss << "buffer size: " << buffer.size() << '\n';
	auto t_end_buff = std::chrono::high_resolution_clock::now();
	ss << std::fixed << std::setprecision(2)
	   << "Wall clock time passed reading buffer: "
	   << std::chrono::duration<double, std::milli>(t_end_buff-t_start_buff).count() << '\n';

	auto t_start_process_input = std::chrono::high_resolution_clock::now();

	auto index = counter::get_index(buffer);

	auto t_end_process_input = std::chrono::high_resolution_clock::now();
	ss << std::fixed << std::setprecision(2)
	   << "Wall clock time passed processing input: "
	   << std::chrono::duration<double, std::milli>(t_end_process_input-t_start_process_input).count() << '\n';
	auto t_start_dump = std::chrono::high_resolution_clock::now();

	counter::dump(std::move(index), file_processor);

	auto t_end_dump = std::chrono::high_resolution_clock::now();
	ss << std::fixed << std::setprecision(2)
	   << "Wall clock time passed dumping to file: "
	   << std::chrono::duration<double, std::milli>(t_end_dump-t_start_dump).count() << '\n';

	auto t_end = std::chrono::high_resolution_clock::now();
	ss << std::fixed << std::setprecision(2)
	   << "Wall clock time passed TOTAL: "
	   << std::chrono::duration<double, std::milli>(t_end-t_start).count() << '\n';

	std::cout << ss.str();
	ASSERT_TRUE(true);
}
