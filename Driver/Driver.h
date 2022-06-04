#pragma once
#include "Imports.hpp"

class CDriverAPI
{
	enum IO_CMD : ULONG
	{
		CMD_NONE = 0x100000,
		CMD_READ_VM = 0x200000,
		CMD_WRITE_VM = 0x300000,
		CMD_GET_MOD_INFO = 0x600000,
		CMD_CHECKSTATUS = 0x900000,
	};


	struct IOCOMMAND
	{
		ULONG NameHash;
		ULONG ProcessID;
		ULONG CommandID;
		ULONG64 Src, Dst;
		ULONG CopySize;
	};
public:
	struct MODINFO
	{
		ULONG64 ModBase;
		ULONG ModSize;
	};

	CDriverAPI() = default;
	CDriverAPI(ULONG PID);

	//read vm
	template <typename T>
	T Read(uintptr_t Addr)
	{
		T ReadData{};
		IOCOMMAND IO_Data;
		IO_Data.NameHash = 'GLOW';
		IO_Data.CommandID = IO_CMD::CMD_READ_VM;
		IO_Data.ProcessID = this->m_PID;
		IO_Data.Src = (ULONG64)Addr;
		IO_Data.Dst = (ULONG64)&ReadData;
		IO_Data.CopySize = sizeof(T);

		NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);

		return ReadData;
	}

	void ReadArr(uintptr_t Addr, PVOID Data, ULONG Size);
	void WriteArr(uintptr_t Addr, PVOID Data, ULONG Size);

	//write vm
	template <typename T>
	__declspec(noinline) void Write(uintptr_t Addr, T Data)
	{
		IOCOMMAND IO_Data;
		IO_Data.NameHash = 'GLOW';
		IO_Data.CommandID = IO_CMD::CMD_WRITE_VM;
		IO_Data.ProcessID = this->m_PID;
		IO_Data.Src = (ULONG64)&Data;
		IO_Data.Dst = (ULONG64)Addr;
		IO_Data.CopySize = sizeof(T);

		NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);
	}

	//get module base
	MODINFO GetModInfo(const char* ModName);

	bool IsDriverLoaded();

	inline void SetPID(ULONG PID)
	{
		this->m_PID = PID;
	}

private:
	ULONG m_PID = 0x0;
};

extern CDriverAPI DriverAPI;