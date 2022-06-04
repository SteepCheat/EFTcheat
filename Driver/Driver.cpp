#include "Driver.h"

CDriverAPI::CDriverAPI(ULONG PID)
	:
	m_PID(PID)
{}

void CDriverAPI::ReadArr(uintptr_t Addr, PVOID Data, ULONG Size)
{
	IOCOMMAND IO_Data;
	IO_Data.NameHash = 'GLOW';
	IO_Data.CommandID = IO_CMD::CMD_READ_VM;
	IO_Data.ProcessID = this->m_PID;
	IO_Data.Src = (ULONG64)Addr;
	IO_Data.Dst = (ULONG64)Data;
	IO_Data.CopySize = Size;

	NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);
}

void CDriverAPI::WriteArr(uintptr_t Addr, PVOID Data, ULONG Size)
{
	IOCOMMAND IO_Data;
	IO_Data.NameHash = 'GLOW';
	IO_Data.CommandID = IO_CMD::CMD_WRITE_VM;
	IO_Data.ProcessID = this->m_PID;
	IO_Data.Src = (ULONG64)Data;
	IO_Data.Dst = (ULONG64)Addr;
	IO_Data.CopySize = Size;

	NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);
}

CDriverAPI::MODINFO CDriverAPI::GetModInfo(const char* ModName)
{
	IOCOMMAND IO_Data;
	IO_Data.NameHash = 'GLOW';
	IO_Data.ProcessID = this->m_PID;
	IO_Data.CommandID = IO_CMD::CMD_GET_MOD_INFO;

	MODINFO ModBase = { 0, 0 };
	IO_Data.Src = (ULONG64)ModName;
	IO_Data.Dst = (ULONG64)&ModBase;

	NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);

	return ModBase;
}

bool CDriverAPI::IsDriverLoaded()
{
	ULONG64 Dst = 0;
	IOCOMMAND IO_Data;
	IO_Data.NameHash = 'GLOW';
	IO_Data.ProcessID = GetCurrentProcessId();
	IO_Data.CommandID = IO_CMD::CMD_CHECKSTATUS;
	IO_Data.Src = 5;
	IO_Data.Dst = (ULONG64)&Dst;

	NtQuerySystemInformation((_SYSTEM_INFORMATION_CLASS)76, &IO_Data, sizeof(IO_Data), &IO_Data.ProcessID);

	return Dst == IO_Data.Src * IO_Data.Src;
}

CDriverAPI DriverAPI = {};