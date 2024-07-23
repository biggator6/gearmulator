#pragma once

#include <cstdint>

#include "wLib/wMidiTypes.h"

namespace mqLib
{
	enum MidiHeaderByte : uint8_t
	{
		IdQ = 0x0f,
		IdMicroQ = 0x10,
	};

	enum class SysexCommand : uint8_t
	{
		Invalid = 0xff,

		SingleRequest = 0x00, SingleDump = 0x10, SingleParameterChange = 0x20, SingleParameterRequest = 0x30,
		MultiRequest  = 0x01, MultiDump  = 0x11, MultiParameterChange  = 0x21, MultiParameterRequest  = 0x31,
		DrumRequest	  = 0x02, DrumDump   = 0x12, DrumParameterChange   = 0x22, DrumParameterRequest   = 0x32,
		GlobalRequest = 0x04, GlobalDump = 0x14, GlobalParameterChange = 0x24, GlobalParameterRequest = 0x34,
		ModeRequest   = 0x07, ModeDump   = 0x17, ModeParameterChange   = 0x27, ModeParameterRequest   = 0x37,

		EmuLCD = 0x50,
		EmuLEDs = 0x51,
		EmuButtons = 0x52,
		EmuRotaries = 0x53,
		EmuLCDCGRata = 0x54
	};

	enum class MidiBufferNum : uint8_t
	{
		DeprecatedSingleBankA = 0x00,
		DeprecatedSingleBankB = 0x01,
		DeprecatedSingleBankC = 0x02,
		DeprecatedSingleBankX = 0x03,
		AllSounds = 0x10,
		SingleEditBufferSingleMode = 0x20,
		SingleEditBufferMultiMode = 0x30,
		EditBufferSingleLayer = 0x30,
		EditBufferDrumMap = 0x30,
		SingleBankA = 0x40,
		SingleBankB = 0x41,
		SingleBankC = 0x42,
		SingleBankX = 0x48,

		DeprecatedMultiBankInternal = 0x00,
		DeprecatedMultiBankCard = 0x03,
		MultiEditBuffer = 0x20,
		MultiBankInternal = 0x40,
		MultiBankCard = 0x48,

		DeprecatedDrumBankInternal = 0x00,
		DeprecatedDrumBankCard = 0x01,
		DrumEditBuffer = 0x20,
		DrumBankInternal = 0x40,
		DrumBankCard = 0x48,	// ?? midi doc says $40, but that is for the internal one, assuming 48 is meant, similar to all other card buffers
	};

	enum class MidiSoundLocation : uint8_t
	{
		AllSinglesBankA = 0x40,
		AllSinglesBankB,
		AllSinglesBankC,
		AllSinglesBankX = 0x48,
		EditBufferCurrentSingle = 0x00,
		EditBufferFirstMultiSingle = 0x00,
		EditBufferFirstSingleLayer = 0x00,
		EditBufferFirstDrumMapInstrument = 0x10,
	};
	
	enum class GlobalParameter : uint8_t
	{
		// Global data
		Version,

		// Initial instrument settings
		InstrumentSelection = 20,
		SingleMultiMode = 21,
		MultiNumber = 22,

		// Instr. 1-4
		InstrumentASingleNumber = 1,	InstrumentBSingleNumber,	InstrumentCSingleNumber,	InstrumentDSingleNumber,
		InstrumentABankNumber = 9,		InstrumentBBankNumber,		InstrumentCBankNumber,		InstrumentDBankNumber,

		// Pedal/CV
		PedalOffset = 70,
		PedalGain,
		PedalCurve,
		PedalControl,

		// MIDI Setup
		Tuning = 5,
		Transpose,
		ControllerSend,
		ControllerReceive,
		ControllerW = 53,
		ControllerX,
		ControllerY,
		ControllerZ,
		ArpSend = 15,
		Clock = 19,
		MidiChannel = 24,
		SysExDeviceId,
		LocalControl,

		// Program change
		ProgramChangeRx = 57,
		ProgramChangeTx = 74,

		// Display Setup
		PopupTime = 27,
		LabelTime,
		DisplayContrast,

		// Keyboard setp
		OnVelocityCurve = 30,
		ReleaseVelocityCurve,
		PressureCurve,

		// External input
		InputGain = 33,

		// FX setup
		GlobalLinkFX2 = 35,

		// Mix in
		Send = 58,
		Level
	};

	enum class MultiParameter
	{
		Volume = 0,

		ControlW = 1,		ControlX,		ControlY,		ControlZ,

		Name00 = 16, Name01, Name02, Name03, Name04, Name05, Name06, Name07, Name08, Name09, Name10, Name11, Name12, Name13, Name14, Name15,

		Inst0SoundBank = 32,
		Inst0SoundNumber,
		Inst0MidiChannel,
		Inst0Volume,
		Inst0Transpose,
		Inst0Detune,
		Inst0Output,
		Inst0Flags,
		Inst0Pan,
		Inst0ReservedA,
		Inst0ReservedB,
		Inst0Pattern,
		Inst0VeloLow,
		Inst0VeloHigh,
		Inst0KeyLow,
		Inst0KeyHigh,
		Inst0MidiRxFlags,

		Inst0 = Inst0SoundBank,		Inst1 = 54,		Inst2 = 76,		Inst3 = 98,
		Inst4 = 120,				Inst5 = 142,	Inst6 = 164,	Inst7 = 186,
		Inst8 = 208,				Inst9 = 230,	Inst10 = 252,	Inst11 = 274,
		Inst12 = 296,				Inst13 = 318,	Inst14 = 340,	Inst15 = 362,

		Last = 378,

		Padding0,
		Padding1,
		Padding2,
		Padding3,
		Padding4,

		Count
	};

	enum SysexIndex
	{
		// first parameter of a dump
		IdxSingleParamFirst = 7,
		IdxMultiParamFirst  = IdxSingleParamFirst,
		IdxDrumParamFirst   = IdxSingleParamFirst,
		IdxGlobalParamFirst = wLib::IdxBuffer,
		IdxModeParamFirst = wLib::IdxBuffer,

		IdxSingleParamIndexH = wLib::IdxBuffer + 1,
		IdxSingleParamIndexL = IdxSingleParamIndexH + 1,
		IdxSingleParamValue  = IdxSingleParamIndexL + 1,

		IdxMultiParamIndexH = wLib::IdxBuffer,
		IdxMultiParamIndexL = IdxMultiParamIndexH + 1,
		IdxMultiParamValue  = IdxMultiParamIndexL + 1,

		IdxDrumParamIndexH = wLib::IdxBuffer,
		IdxDrumParamIndexL = IdxMultiParamIndexH + 1,
		IdxDrumParamValue  = IdxMultiParamIndexL + 1,

		IdxGlobalParamIndexH = wLib::IdxBuffer,
		IdxGlobalParamIndexL = IdxGlobalParamIndexH + 1,
		IdxGlobalParamValue  = IdxGlobalParamIndexL + 1,

		IdxModeParamIndexH = wLib::IdxBuffer,
		IdxModeParamIndexL = IdxModeParamIndexH + 1,
		IdxModeParamValue  = IdxModeParamIndexL + 1
	};

	enum class BankSelectLSB : uint8_t
	{
		BsDeprecatedSingleBankA = 0x0,
		BsDeprecatedSingleBankB = 0x1,
		BsDeprecatedSingleBankC = 0x2,
		BsDeprecatedDrumBank = 0x4,
		BsSingleBankA = 0x40,
		BsSingleBankB = 0x41,
		BsSingleBankC = 0x42,
		BsDrumBank = 0x50,
		BsMultiBank = 0x60,
	};
}
