#ifndef _UNIVERSAL_UPDATER_THREAD_HPP
#define _UNIVERSAL_UPDATER_THREAD_HPP

#include <3ds.h>
#include <vector>

namespace Threads {
	void create(ThreadFunc entrypoint);
	void destroy(void);
}

#endif