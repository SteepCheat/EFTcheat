#include "hook.h"

bool nullhook::call_kernel_function(void* kernel_function_address)
{
	if (!kernel_function_address)
		return false;

	PVOID* function = reinterpret_cast<PVOID*>(get_system_module_export("\\SystemRoot\\System32\\drivers\\dxgkrnl.sys", "NtGdiDdDDINetDispGetNextChunkInfo"));

	if (!function)
		return false;

	BYTE orig[] = { 0xCF, 0xDF, 0xFF, 0xFD, 0xFC, 0xCD, 0xDC, 0xCC, 0xDD, 0xFE, 0xEF, 0xEE };

	BYTE shell_code[] = { 0x48, 0xB8 }; // mov rax, xxx
	BYTE shell_code_end[] = { 0xFF, 0xE0 }; //jmp rax

	RtlSecureZeroMemory(&orig, sizeof(orig));
	memcpy((PVOID)((ULONG_PTR)orig), &shell_code, sizeof(shell_code));
	uintptr_t hook_address = reinterpret_cast<uintptr_t>(kernel_function_address);
	memcpy((PVOID)((ULONG_PTR)orig + sizeof(shell_code)), &hook_address, sizeof(void*));
	memcpy((PVOID)((ULONG_PTR)orig + sizeof(shell_code) + sizeof(void*)), &shell_code_end, sizeof(shell_code_end));

	write_to_read_only_memory(function, &orig, sizeof(orig));

	return true;
}

NTSTATUS nullhook::hook_handler(PVOID called_param)
{
	NULL_MEMORY* instructions = (NULL_MEMORY*)called_param;
	static PEPROCESS s_target_process;
	if (instructions->req_base != FALSE)
	{
		PsLookupProcessByProcessId((HANDLE)instructions->pid, &s_target_process);

		ANSI_STRING GA;
		UNICODE_STRING GAA;

		RtlInitAnsiString(&GA, "GameAssembly.dll");
		RtlAnsiStringToUnicodeString(&GAA, &GA, TRUE);

		instructions->GA = get_module_base_x64(s_target_process, GAA);

		ANSI_STRING UP;
		UNICODE_STRING UPA;

		RtlInitAnsiString(&UP, "UnityPlayer.dll");
		RtlAnsiStringToUnicodeString(&UPA, &UP, TRUE);

		instructions->UP = get_module_base_x64(s_target_process, UPA);
	}

	if (instructions->write != FALSE)
	{
		if (instructions->address < 0x7FFFFFFFFFFF && instructions->address > 0)
		{
			PVOID kernelBuff = ExAllocatePool(NonPagedPool, instructions->size);

			if (!kernelBuff)
			{
				return STATUS_UNSUCCESSFUL;
			}

			if (!memcpy(kernelBuff, instructions->buffer_address, instructions->size))
			{
				return STATUS_UNSUCCESSFUL;
			}

			PEPROCESS process;
			PsLookupProcessByProcessId((HANDLE)instructions->pid, &process);
			write_kernel_memory((HANDLE)instructions->pid, instructions->address, kernelBuff, instructions->size);
			ExFreePool(kernelBuff);
		}
	}

	if (instructions->read != FALSE)
	{
		if (instructions->address < 0x7FFFFFFFFFFF && instructions->address > 0)
		{
			read_kernel_memory((HANDLE)instructions->pid, instructions->address, instructions->output, instructions->size);
		}
	}

	return STATUS_SUCCESS;
}