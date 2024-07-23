#include "buttons.h"

#include "mqtypes.h"

#include "mc68k/port.h"

namespace mqLib
{
	Buttons::Buttons()
	{
		m_buttonStates.fill(0);
	}

	bool Buttons::processButtons(mc68k::Port& _gp, mc68k::Port& _e)
	{
		if(_gp.getDirection() == 0xff)
			return false;

		const auto w = _e.getWriteCounter();

		const bool cycleEncoders = w != m_writeCounter;
		m_writeCounter = w;

		if (getButtonState(ButtonType::Power) && !_e.bittest(BtPower))
		{
			_e.setBitRX(BtPower);
		}
		else if (!getButtonState(ButtonType::Power) && _e.bittest(BtPower))
		{
			_e.clearBitRX(BtPower);
		}
		
		if(!_e.bittest(Buttons0CS))
		{
			uint8_t res = 0;
			for(size_t i=0; i<8; ++i)
				res |= m_buttonStates[i] << i;
			_gp.writeRX(res);
			return true;
		}

		if(!_e.bittest(Buttons1CS))
		{
			uint8_t res = 0;
			for(size_t i=0; i<8; ++i)
				res |= m_buttonStates[i+8] << i;
			_gp.writeRX(res);
			return true;
		}
				
		if(!_e.bittest(Encoders0CS))
		{
			uint8_t res = 0;

			for(size_t i=0; i<4; ++i)
			{
				res |= processStepEncoder(static_cast<Encoders>(i), cycleEncoders) << (i<<1);
			}
			_gp.writeRX(res);
			return true;
		}

		if(!_e.bittest(Encoders1CS))
		{
			uint8_t res = 0;

			for(size_t i=4; i<static_cast<uint32_t>(Encoders::Master); ++i)
			{
				res |= processStepEncoder(static_cast<Encoders>(i), cycleEncoders) << ((i-4)<<1);
			}

			res |= processStepEncoder(Encoders::Master, cycleEncoders) << ((static_cast<uint32_t>(Encoders::Master)-4)<<1);

			_gp.writeRX(res);
			return true;
		}

		return false;
	}

	void Buttons::setButton(ButtonType _type, bool _pressed)
	{
		m_buttonStates[static_cast<uint32_t>(_type)] = _pressed;
	}

	void Buttons::toggleButton(ButtonType _type)
	{
		m_buttonStates[static_cast<uint32_t>(_type)] ^= 1;
	}

	void Buttons::rotate(Encoders _encoder, const int _amount)
	{
		auto& current = m_remainingRotations[static_cast<uint32_t>(_encoder)];

		// The master encoder is a step encoder where 4 steps (one full cycle) is 1 tick
		// The other encoders do not have any snapping, there are no ticks and there is some acceleration going on in the uc so use the smallest value that does something useful
		current += _amount;// * (_encoder == Encoders::Master ? 4 : 3);
	}

	uint8_t Buttons::processStepEncoder(Encoders _encoder, bool cycleEncoders)
	{
		const auto i = static_cast<uint32_t>(_encoder);

		auto& v = m_encoderValues[i];

		constexpr uint8_t pattern[] = {0b00, 0b10, 0b11, 0b01};

		if(!cycleEncoders)
			return pattern[v&3];

		auto& c = m_remainingRotations[i];

		if(c > 0)
		{
			--v;
			--c;
		}
		else if(c < 0)
		{
			++v;
			++c;
		}
		return pattern[v&3];
	}
}
