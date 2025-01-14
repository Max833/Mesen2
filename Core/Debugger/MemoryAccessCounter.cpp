#include "pch.h"
#include "Debugger/MemoryAccessCounter.h"
#include "Debugger/DebugBreakHelper.h"
#include "Debugger/Debugger.h"
#include "Debugger/DebugUtilities.h"
#include "Debugger/MemoryDumper.h"
#include "SNES/SnesMemoryManager.h"
#include "SNES/SnesConsole.h"
#include "SNES/Spc.h"
#include "SNES/Coprocessors/SA1/Sa1.h"
#include "SNES/Coprocessors/GSU/Gsu.h"
#include "SNES/Coprocessors/CX4/Cx4.h"
#include "SNES/BaseCartridge.h"
#include "Gameboy/Gameboy.h"

MemoryAccessCounter::MemoryAccessCounter(Debugger* debugger)
{
	_debugger = debugger;

	for(int i = (int)DebugUtilities::GetLastCpuMemoryType() + 1; i < DebugUtilities::GetMemoryTypeCount(); i++) {
		uint32_t memSize = _debugger->GetMemoryDumper()->GetMemorySize((MemoryType)i);
		_counters[i].reserve(memSize);
		for(uint32_t j = 0; j < memSize; j++) {
			_counters[i].push_back({});
		}
	}
}

ReadResult MemoryAccessCounter::ProcessMemoryRead(AddressInfo &addressInfo, uint64_t masterClock)
{
	if(addressInfo.Address < 0) {
		return ReadResult::Normal;
	}

	AddressCounters& counts = _counters[(int)addressInfo.Type][addressInfo.Address];
	if(counts.WriteStamp == 0 && DebugUtilities::IsVolatileRam(addressInfo.Type)) {
		ReadResult result = counts.ReadStamp == 0 ? ReadResult::FirstUninitRead : ReadResult::UninitRead;
		counts.ReadStamp = masterClock;
		counts.ReadCounter++;
		return result;
	}

	counts.ReadStamp = masterClock;
	counts.ReadCounter++;
	return ReadResult::Normal;
}

void MemoryAccessCounter::ProcessMemoryWrite(AddressInfo& addressInfo, uint64_t masterClock)
{
	if(addressInfo.Address < 0) {
		return;
	}

	AddressCounters& counts = _counters[(int)addressInfo.Type][addressInfo.Address];
	counts.WriteStamp = masterClock;
	counts.WriteCounter++;
}

void MemoryAccessCounter::ProcessMemoryExec(AddressInfo& addressInfo, uint64_t masterClock)
{
	if(addressInfo.Address < 0) {
		return;
	}

	AddressCounters& counts = _counters[(int)addressInfo.Type][addressInfo.Address];
	counts.ExecStamp = masterClock;
	counts.ExecCounter++;
}

void MemoryAccessCounter::ResetCounts()
{
	DebugBreakHelper helper(_debugger);
	for(int i = 0; i < DebugUtilities::GetMemoryTypeCount(); i++) {
		memset(_counters[i].data(), 0, _counters[i].size() * sizeof(AddressCounters));
	}
}

void MemoryAccessCounter::GetAccessCounts(uint32_t offset, uint32_t length, MemoryType memoryType, AddressCounters counts[])
{
	if(DebugUtilities::IsRelativeMemory(memoryType)) {
		AddressInfo addr = {};
		addr.Type = memoryType;
		for(uint32_t i = 0; i < length; i++) {
			addr.Address = offset + i;
			AddressInfo info = _debugger->GetAbsoluteAddress(addr);
			if(info.Address >= 0) {
				counts[i] = _counters[(int)info.Type][info.Address];
			}
		}
	} else {
		if(offset + length <= _counters[(int)memoryType].size()) {
			memcpy(counts, _counters[(int)memoryType].data() + offset, length * sizeof(AddressCounters));
		}
	}
}
