#ifndef THREAD_HPP
#define THREAD_HPP

#include <3ds.h>
#include <vector>

namespace Threads {
	void create(ThreadFunc entrypoint);
	void destroy(void);
}

#endif