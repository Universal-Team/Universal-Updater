#include <string>
#include "utils/formatting.hpp"

// adapted from GM9i's byte parsing.
std::string formatBytes(int bytes) {
    char out[32];
    if(bytes == 1)
        snprintf(out, sizeof(out), "%d Byte", bytes);
    else if(bytes < 1024)
        snprintf(out, sizeof(out), "%d Bytes", bytes);
    else if(bytes < 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f KB", (float)bytes / 1024);
    else if (bytes < 1024 * 1024 * 1024)
        snprintf(out, sizeof(out), "%.1f MB", (float)bytes / 1024 / 1024);
    else
        snprintf(out, sizeof(out), "%.1f GB", (float)bytes / 1024 / 1024 / 1024);

    return out;
}