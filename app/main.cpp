//
// Created by Andrey Solovyev on 11/08/2023.
//

#include "../code_branch_selector.h"

#if defined (MT_TRIE) || defined (MT_CONCURRENT_MAP) || defined (ST) && !defined (MT_SHISHKOV)
#include "counter.hpp"
#elif !defined (MT_TRIE) && !defined (MT_CONCURRENT_MAP) && !defined (ST) && defined (MT_SHISHKOV)
#include "counter_shishkov.hpp"
#endif
#include "files.h"

int main(int argc, char* argv[]) {
	using namespace freq;

	auto file_paths = files::check_files(argc, argv);
	if (!file_paths) {
		return EXIT_FAILURE;
	}

	try {
		files::processor_t file_processor(std::move(*file_paths));
		auto buffer = text::get_buffer_from(file_processor);
		auto index = counter::get_index(buffer);
		counter::dump(std::move(index), file_processor);
	}
	catch (std::exception &e) {
		std::printf("Error: Exception is caught: %s", e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
