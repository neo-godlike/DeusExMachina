#include <cstdint>
#include <string>
#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <time.h>
#include <Psapi.h>
#include <functional>
#include <tlhelp32.h>
#include <vector>
#include "TCHAR.h"
#include <codecvt>
#include <thread>

class Memory
{
public:
	Memory();
	~Memory();

	uint32_t process_id;
	HANDLE handle;
	uint64_t base_address;

	HANDLE   Open();
	uint32_t FindProcess(const std::string& name);
	MODULEENTRY32 FindModule(const std::string& name);

	uintptr_t FindPattern(MODULEENTRY32 module, const char* pattern, const char* mask);

	std::vector<uintptr_t> FindReferences(const char* pattern, const char* mask, const size_t numThreads);

	void PatternScanImpl(std::vector<uintptr_t>& addresses, const uint8_t* const pattern, const char* const mask, uintptr_t start, const uintptr_t end) const;

	template <typename T>
	T Read(uint64_t address);

	uint64_t ReadBuffer(uint64_t address, PVOID buffer, SIZE_T size);
	uint64_t ReadChain(uint64_t base, const std::vector<uint64_t>& offsets);
};

template<typename T>
inline T Memory::Read(uint64_t address)
{
	T buffer{ };
	ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), nullptr);
	return buffer;
}

inline uint64_t Memory::ReadBuffer(uint64_t address, PVOID buffer, SIZE_T size)
{
	ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(address), buffer, size, &size);
	return (uint64_t)buffer;
}

inline uint64_t Memory::ReadChain(uint64_t base, const std::vector<uint64_t>& offsets) {
	uint64_t result = Read<uint64_t>(base + offsets.at(0));
	for (int i = 1; i < offsets.size(); i++) {
		result = Read<uint64_t>(result + offsets.at(i));
	}
	return result;
}

extern Memory* mem;
