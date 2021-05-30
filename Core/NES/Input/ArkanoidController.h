#pragma once
#include "stdafx.h"
#include "Shared/BaseControlDevice.h"
#include "Shared/KeyManager.h"
#include "Shared/Emulator.h"
#include "Shared/EmuSettings.h"
#include "Utilities/Serializer.h"

class ArkanoidController : public BaseControlDevice
{
private:
	uint32_t _currentValue = (0xF4 - 0x54) / 2;
	uint32_t _stateBuffer = 0;
	enum Buttons { Fire };

protected:
	bool HasCoordinates() override { return true; }

	string GetKeyNames() override
	{
		return "F";
	}

	void InternalSetStateFromInput() override
	{
		if(_emu->GetSettings()->IsInputEnabled()) {
			SetPressedState(Buttons::Fire, KeyManager::IsMouseButtonPressed(MouseButton::LeftButton));
			SetMovement(KeyManager::GetMouseMovement(_emu->GetSettings()->GetInputConfig().MouseSensitivity + 1));
		}
	}

	void Serialize(Serializer& s) override
	{
		BaseControlDevice::Serialize(s);
		s.Stream(_stateBuffer, _currentValue);
	}

	void RefreshStateBuffer() override
	{
		MouseMovement mov = GetMovement();

		_currentValue += mov.dx;
		if(_currentValue < 0x54) {
			_currentValue = 0x54;
		} else if(_currentValue > 0xF4) {
			_currentValue = 0xF4;
		}

		_stateBuffer = _currentValue;
	}

public:
	ArkanoidController(Emulator* emu, ControllerType type, uint8_t port) : BaseControlDevice(emu, type, port)
	{
	}

	uint8_t ReadRam(uint16_t addr) override
	{
		uint8_t output = 0;
		if(IsExpansionDevice()) {
			if(addr == 0x4016) {
				//Fire button is on port 1
				if(IsPressed(ArkanoidController::Buttons::Fire)) {
					output |= 0x02;
				}
			} else if(addr == 0x4017) {
				//Serial data is on port 2
				output |= ((~_stateBuffer) >> 6) & 0x02;
				_stateBuffer <<= 1;
			}
		} else if(IsCurrentPort(addr)) {
			output = ((~_stateBuffer) >> 3) & 0x10;
			_stateBuffer <<= 1;

			if(IsPressed(ArkanoidController::Buttons::Fire)) {
				output |= 0x08;
			}
		}

		return output;
	}

	void WriteRam(uint16_t addr, uint8_t value) override
	{
		StrobeProcessWrite(value);
	}
};