
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include "Imports.hpp"
#include <TlHelp32.h>


template <class T>
inline T read_chain(uintptr_t base, const std::vector<T>& offsets) {
	T result = DriverAPI.Read<T>(base + offsets.at(0));
	for (int i = 1; i < offsets.size(); i++) {
		result = DriverAPI.Read<T>(result + offsets.at(i));
	}
	return result;
}
