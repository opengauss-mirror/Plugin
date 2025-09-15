/*
 * memutils.h
 *   utility functions to help with postgres' memory management primitives
 */

#ifndef SPQ_MEMUTILS_H
#define SPQ_MEMUTILS_H

#include "utils/palloc.h"

namespace spq {
/** The memory address alignment size is defined as 8 bytes, because some arm platform
 instructions require at least 4 bytes alignment.*/
constexpr int SHARE_MEM_ALIGN_SIZE = 8;

}  // namespace spq

#endif /*SPQ_MEMUTILS_H */
