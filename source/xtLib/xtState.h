#pragma once

#include <array>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "xtMidiTypes.h"

#include "synthLib/deviceTypes.h"
#include "synthLib/midiTypes.h"

#include "wLib/wState.h"

namespace synthLib
{
	struct SMidiEvent;
}

namespace xt
{
	class Xt;

	using SysEx = wLib::SysEx;
	using Responses = wLib::Responses;

	class State : public wLib::State
	{
	public:
		enum class Origin
		{
			Device,
			External
		};

		enum class DumpType
		{
			Single,
			Multi,
			Global,
			Mode,

			Count
		};

		struct Dump
		{
			DumpType type;
			SysexCommand cmdRequest;
			SysexCommand cmdDump;
			SysexCommand cmdParamChange;
			uint32_t firstParamIndex;
			uint32_t idxParamIndexH;
			uint32_t idxParamIndexL;
			uint32_t idxParamValue;
			uint32_t dumpSize;
		};

		static constexpr Dump Dumps[] = 
		{
			{DumpType::Single, SysexCommand::SingleRequest, SysexCommand::SingleDump, SysexCommand::SingleParameterChange, IdxSingleParamFirst, IdxSingleParamIndexH, IdxSingleParamIndexL, IdxSingleParamValue, 265},
			{DumpType::Multi , SysexCommand::MultiRequest , SysexCommand::MultiDump , SysexCommand::MultiParameterChange , IdxMultiParamFirst , IdxMultiParamIndexH , IdxMultiParamIndexL , IdxMultiParamValue , 265},
			{DumpType::Global, SysexCommand::GlobalRequest, SysexCommand::GlobalDump, SysexCommand::GlobalParameterChange, IdxGlobalParamFirst, IdxGlobalParamIndexH, IdxGlobalParamIndexL, IdxGlobalParamValue, 39},
			{DumpType::Mode  , SysexCommand::ModeRequest  , SysexCommand::ModeDump  , SysexCommand::ModeParameterChange  , IdxModeParamFirst  , IdxModeParamIndexH  , IdxModeParamIndexL  , IdxModeParamValue  , 7},
		};

		using Single = std::array<uint8_t, Dumps[static_cast<uint32_t>(DumpType::Single)].dumpSize>;
		using Multi = std::array<uint8_t, Dumps[static_cast<uint32_t>(DumpType::Multi)].dumpSize>;
		using Global = std::array<uint8_t, Dumps[static_cast<uint32_t>(DumpType::Global)].dumpSize>;
		using Mode = std::array<uint8_t, Dumps[static_cast<uint32_t>(DumpType::Mode)].dumpSize>;

		State(Xt& _xt);

		bool loadState(const SysEx& _sysex);

		bool receive(Responses& _responses, const synthLib::SMidiEvent& _data, Origin _sender);
		bool receive(Responses& _responses, const SysEx& _data, Origin _sender);
		void createInitState();

		bool getState(std::vector<uint8_t>& _state, synthLib::StateType _type) const;
		bool setState(const std::vector<uint8_t>& _state, synthLib::StateType _type);

		static void createSequencerMultiData(std::vector<uint8_t>& _data);

	private:

		template<size_t Size> static bool append(SysEx& _dst, const std::array<uint8_t, Size>& _src, uint32_t _checksumStartIndex)
		{
			if(!isValid(_src))
				return false;
			auto src = _src;
			if(_checksumStartIndex != ~0)
				wLib::State::updateChecksum(src, _checksumStartIndex);
			_dst.insert(_dst.end(), src.begin(), src.end());
			return true;
		}

		static bool updateChecksum(SysEx& _src, uint32_t _startIndex)
		{
			if(_src.size() < 3)
				return false;
			uint8_t& c = _src[_src.size() - 2];
			c = 0;
			for(size_t i= wLib::IdxCommand; i<_src.size()-2; ++i)
				c += _src[i];
			c &= 0x7f;
			return true;
		}

		bool parseSingleDump(const SysEx& _data);
		bool parseMultiDump(const SysEx& _data);
		bool parseGlobalDump(const SysEx& _data);
		bool parseModeDump(const SysEx& _data);

		bool modifySingle(const SysEx& _data);
		bool modifyMulti(const SysEx& _data);
		bool modifyGlobal(const SysEx& _data);
		bool modifyMode(const SysEx& _data);

		uint8_t* getSingleParameter(const SysEx& _data);
		uint8_t* getMultiParameter(const SysEx& _data);
		uint8_t* getGlobalParameter(const SysEx& _data);
		uint8_t* getModeParameter(const SysEx& _data);

		bool getSingle(Responses& _responses, const SysEx& _data);
		Single* getSingle(LocationH _buf, uint8_t _loc);

		bool getMulti(Responses& _responses, const SysEx& _data);
		Multi* getMulti(LocationH _buf, uint8_t _loc);

		bool getGlobal(Responses& _responses);
		Global* getGlobal();

		bool getMode(Responses& _responses);
		Mode* getMode();

		bool getDump(DumpType _type, Responses& _responses, const SysEx& _data);
		bool parseDump(DumpType _type, const SysEx& _data);
		bool modifyDump(DumpType _type, const SysEx& _data);

		uint8_t getGlobalParameter(GlobalParameter _parameter) const;
		void setGlobalParameter(GlobalParameter _parameter, uint8_t _value);

		uint8_t getModeParameter(ModeParameter _parameter) const;

		bool isMultiMode() const
		{
			return getModeParameter(ModeParameter::Mode) != 0;
		}

		static bool isValid(const Single& _single)
		{
			return _single.front() == 0xf0;
		}

		static bool isValid(const Global& _global)
		{
			return _global.back() == 0xf7;
		}

		static bool isValid(const Mode& _mode)
		{
			return _mode.front() == 0xf0;
		}

		static SysexCommand getCommand(const SysEx& _data);

		void forwardToDevice(const SysEx& _data) const;

		void requestGlobal() const;
		void requestMode() const;
		void requestSingle(LocationH _buf, uint8_t _location) const;
		void requestMulti(LocationH _buf, uint8_t _location) const;
		void sendMulti(const std::vector<uint8_t>& _multiData) const;
		void sendGlobalParameter(GlobalParameter _param, uint8_t _value);
		void sendSysex(const std::initializer_list<uint8_t>& _data) const;
		void sendSysex(const SysEx& _data) const;

		void onPlayModeChanged();

		Xt& m_xt;

		// ROM
		std::array<Single, 256> m_romSingles{Single{}};
		std::array<Multi, 128> m_romMultis{Multi{}};

		// Edit Buffers
		std::array<Single, 8> m_currentMultiSingles{Single{}};
		std::array<Single, 1> m_currentInstrumentSingles{Single{}};
		Multi m_currentMulti{};

		// Global settings, only available once
		Global m_global{};
		Mode m_mode{};

		// current state, valid while receiving data
		Origin m_sender = Origin::External;
		bool m_isEditBuffer = false;

		synthLib::SMidiEvent m_lastBankSelectMSB;
		synthLib::SMidiEvent m_lastBankSelectLSB;
	};
}
