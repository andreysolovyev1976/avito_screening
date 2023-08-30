//
// Created by Andrey Solovyev on 29/08/2023.
//

#pragma once

#ifndef FREQ_CODE_BRANCH_SELECTOR_H
#define FREQ_CODE_BRANCH_SELECTOR_H


/**
 !!!\n
 One of the options must be defined\n
 if MT_TRIE is selected then either TRIE_ON_VECTOR or TRIE_ON_ARENA also must be defined

 */

//#define ST
#define MT_CONCURRENT_MAP
//#define MT_TRIE
	//#define TRIE_ON_VECTOR
	//#define TRIE_ON_ARENA
//#define MT_SHISHKOV


#endif //FREQ_CODE_BRANCH_SELECTOR_H
