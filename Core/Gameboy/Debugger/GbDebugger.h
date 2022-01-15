#pragma once
#include "stdafx.h"
#include "Debugger/DebugTypes.h"
#include "Debugger/IDebugger.h"

class Disassembler;
class Debugger;
class GbTraceLogger;
class Gameboy;
class CallstackManager;
class MemoryAccessCounter;
class Console;
class BreakpointManager;
class EmuSettings;
class GbEventManager;
class GbAssembler;
class Emulator;
class CodeDataLogger;
class GbCpu;
class GbPpu;
class GbPpuTools;

enum class MemoryOperationType;

class GbDebugger final : public IDebugger
{
	Debugger* _debugger;
	Emulator* _emu;
	GbCpu* _cpu;
	GbPpu* _ppu;
	Disassembler* _disassembler;
	MemoryAccessCounter* _memoryAccessCounter;
	Gameboy* _gameboy;
	EmuSettings* _settings;

	shared_ptr<GbEventManager> _eventManager;
	shared_ptr<CallstackManager> _callstackManager;
	shared_ptr<CodeDataLogger> _codeDataLogger;
	unique_ptr<BreakpointManager> _breakpointManager;
	unique_ptr<StepRequest> _step;
	shared_ptr<GbAssembler> _assembler;
	unique_ptr<GbTraceLogger> _traceLogger;
	unique_ptr<GbPpuTools> _ppuTools;

	uint8_t _prevOpCode = 0xFF;
	uint32_t _prevProgramCounter = 0;
	bool _enableBreakOnUninitRead = false;

public:
	GbDebugger(Debugger* debugger);
	~GbDebugger();

	void Reset() override;

	void ProcessRead(uint32_t addr, uint8_t value, MemoryOperationType type);
	void ProcessWrite(uint32_t addr, uint8_t value, MemoryOperationType type);
	void ProcessInterrupt(uint32_t originalPc, uint32_t currentPc, bool forNmi) override;
	void ProcessPpuRead(uint16_t addr, uint8_t value, SnesMemoryType memoryType);
	void ProcessPpuWrite(uint16_t addr, uint8_t value, SnesMemoryType memoryType);
	void ProcessPpuCycle();

	void Run() override;
	void Step(int32_t stepCount, StepType type) override;

	shared_ptr<BaseEventManager> GetEventManager() override;
	shared_ptr<IAssembler> GetAssembler() override;
	shared_ptr<CallstackManager> GetCallstackManager() override;
	shared_ptr<CodeDataLogger> GetCodeDataLogger() override;
	BreakpointManager* GetBreakpointManager() override;
	ITraceLogger* GetTraceLogger() override;
	PpuTools* GetPpuTools() override;

	BaseState& GetState() override;
	void GetPpuState(BaseState& state) override;
	void SetPpuState(BaseState& state) override;
};