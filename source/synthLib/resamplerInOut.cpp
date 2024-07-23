#include "resamplerInOut.h"

#include <array>

#include "dsp56kEmu/fastmath.h"
#include "dsp56kEmu/logging.h"

#include <cstring>	// memset/memcpy

using namespace dsp56k;

namespace synthLib
{
	ResamplerInOut::ResamplerInOut(uint32_t _channelCountIn, uint32_t _channelCountOut)
	: m_channelCountIn(_channelCountIn)
	, m_channelCountOut(_channelCountOut)
	, m_scaledInput(_channelCountIn)
	, m_input(_channelCountIn)
	{
	}

	void ResamplerInOut::setDeviceSamplerate(float _samplerate)
	{
		if(m_samplerateDevice == _samplerate)
			return;

		m_samplerateDevice = _samplerate;
		recreate();
	}
	
	void ResamplerInOut::setHostSamplerate(float _samplerate)
	{
		if(m_samplerateHost == _samplerate)
			return;

		m_samplerateHost = _samplerate;
		recreate();
	}

	void ResamplerInOut::setSamplerates(const float _hostSamplerate, const float _deviceSamplerate)
	{
		if(m_samplerateDevice == _deviceSamplerate && m_samplerateHost == _hostSamplerate)
			return;

		m_samplerateDevice = _deviceSamplerate;
		m_samplerateHost = _hostSamplerate;

		recreate();
	}

	void ResamplerInOut::recreate()
	{
		if(m_samplerateDevice < 1 || m_samplerateHost < 1)
			return;

		m_out.reset(new Resampler(m_samplerateDevice, m_samplerateHost));
		m_in.reset(new Resampler(m_samplerateHost, m_samplerateDevice));

		m_scaledInputSize = 0;
		m_inputLatency = 0;
		m_outputLatency = 0;

		// prewarm to calculate latency
		std::array<std::vector<float>, 12> data;

		TAudioInputs ins;
		TAudioOutputs outs;

		for(size_t i=0; i<data.size(); ++i)
			data[i].resize(512, 0);

		for(size_t i=0; i<ins.size(); ++i)
			ins[i] = i >= data.size() ? nullptr : &data[i][0];

		for(size_t i=0; i<outs.size(); ++i)
			outs[i] = i >= data.size() ? nullptr : &data[i][0];

		TMidiVec midiIn, midiOut;
		process(ins, outs, TMidiVec(), midiOut, static_cast<uint32_t>(data[0].size()), [&](const TAudioInputs&, const TAudioOutputs&, size_t, const TMidiVec&, TMidiVec&)
		{
		});
	}

	void ResamplerInOut::scaleMidiEvents(TMidiVec& _dst, const TMidiVec& _src, float _scale)
	{
		_dst.clear();
		_dst.reserve(_src.size());

		for(size_t i=0; i<_src.size(); ++i)
		{
			_dst.push_back(_src[i]);
			_dst[i].offset = floor_int(static_cast<float>(_src[i].offset) * _scale);
		}
	}

	void ResamplerInOut::clampMidiEvents(TMidiVec& _dst, const TMidiVec& _src, uint32_t _offsetMin, uint32_t _offsetMax)
	{
		_dst.clear();
		_dst.reserve(_src.size());

		for(size_t i=0; i<_src.size(); ++i)
		{
			_dst.push_back(_src[i]);
			_dst[i].offset = clamp(_dst[i].offset, _offsetMin, _offsetMax);
		}
	}

	void ResamplerInOut::extractMidiEvents(TMidiVec& _dst, const TMidiVec& _src, uint32_t _offsetMin, uint32_t _offsetMax)
	{
		_dst.clear();
		_dst.reserve(_src.size());

		for(size_t i=0; i<_src.size(); ++i)
		{
			const auto& m = _src[i];
			if(m.offset < static_cast<int>(_offsetMin) || m.offset > static_cast<int>(_offsetMax))
				continue;
			_dst.push_back(m);
		}
	}

	void ResamplerInOut::process(const TAudioInputs& _inputs, TAudioOutputs& _outputs, const TMidiVec& _midiIn, TMidiVec& _midiOut, const uint32_t _numSamples, const TProcessFunc& _processFunc)
	{
		if(!m_in || !m_out)
			return;

		if(m_samplerateDevice == m_samplerateHost)
		{
			_processFunc(_inputs, _outputs, _numSamples, _midiIn, _midiOut);
			return;
		}

		const auto devDivHost = m_samplerateDevice / m_samplerateHost;
		const auto hostDivDev = m_samplerateHost / m_samplerateDevice;

		m_scaledInput.ensureSize(static_cast<uint32_t>(static_cast<float>(_numSamples) * devDivHost * 2.0f));

		scaleMidiEvents(m_midiIn, _midiIn, devDivHost);

		m_input.append(_inputs, _numSamples);

		auto feedInput = [&](TAudioOutputs& _data, uint32_t _numRequestedSamples)
		{
			const auto offset = _numRequestedSamples > m_input.size() ? _numRequestedSamples - m_input.size() : 0;
			if(offset)
			{
				// resampler prewarming, wants more data than we have
				for(size_t c=0; c<m_channelCountIn; ++c)
				{
					memset(_data[c], 0, sizeof(float) * offset);
					_data[c] += offset;
				}
			}

			const auto count = (_numRequestedSamples - offset);

			if(count)
			{
				for(size_t c=0; c<m_channelCountIn; ++c)
					memcpy(_data[c], &m_input.getChannel(c)[0], sizeof(float) * count);

				m_input.remove(count);
			}

			m_inputLatency += static_cast<uint32_t>(offset);
			if(offset)
			{
				LOG("Resampler input latency " << m_inputLatency << " samples");
			}
		};

		auto feedOutput = [&](const TAudioOutputs& _outs, const uint32_t _numProcessedSamples)
		{
			m_scaledInputSize += m_in->process(m_scaledInput, m_scaledInputSize, m_channelCountIn, _numProcessedSamples, false, feedInput);

			clampMidiEvents(m_processedMidiIn, m_midiIn, 0, _numProcessedSamples-1);
			m_midiIn.clear();

			TAudioInputs inputs;
			if(_numProcessedSamples > m_scaledInputSize)
			{
				// resampler prewarming, wants more data than we have
				const auto diff = _numProcessedSamples - m_scaledInputSize;
				m_scaledInput.insertZeroes(diff);
				m_scaledInputSize += diff;
				m_outputLatency += static_cast<uint32_t>(diff);
				LOG("Resampler output latency " << m_outputLatency << " samples");
			}
			m_scaledInput.fillPointers(inputs);
			_processFunc(inputs, _outs, _numProcessedSamples, m_processedMidiIn, m_midiOut);
			m_scaledInput.remove(_numProcessedSamples);
			m_scaledInputSize -= _numProcessedSamples;
		};

		const auto outputSize = m_out->process(_outputs, m_channelCountOut, _numSamples, false, feedOutput);

		scaleMidiEvents(_midiOut, m_midiOut, hostDivDev);
		m_midiOut.clear();
	}
}
