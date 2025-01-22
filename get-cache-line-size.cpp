
#ifdef _WIN32 // Windows-specific code
#include <windows.h>
#include <iostream>
#include <memory> // For std::make_unique

int getCacheLineSize() {
    DWORD bufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize);

    // Use std::make_unique for memory allocation
    auto buffer = std::make_unique<unsigned char[]>(bufferSize);
    auto info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.get());

    if (!GetLogicalProcessorInformation(info, &bufferSize)) {
        std::cerr << "Failed to retrieve processor information." << std::endl;
        return 0;
    }

    for (size_t i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (info[i].Relationship == RelationCache && info[i].Cache.Level == 1) {
            return info[i].Cache.LineSize;
        }
    }

    return 0;
}

#else // Linux/Unix-specific code
#include <unistd.h>
#include <iostream>

int getCacheLineSize() {
    long cacheLineSize = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if (cacheLineSize == -1) {
        std::cerr << "Unable to determine cache line size using sysconf." << std::endl;
        return 0;
    }
    return static_cast<int>(cacheLineSize);
}

#endif
