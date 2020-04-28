#ifndef CIA_HPP
#define CIA_HPP

#include "common.hpp"

#include <3ds.h>

Result CIA_LaunchTitle(u64 titleId, FS_MediaType mediaType);
Result deletePrevious(u64 titleid, FS_MediaType media);
Result installCia(const char * ciaPath, bool updateSelf);

#endif