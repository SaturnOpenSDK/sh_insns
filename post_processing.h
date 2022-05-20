#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include <list>

struct insns;

std::list<insns> post_processing(std::list<insns>&& insn_blocks);

#endif // POST_PROCESSING_H
