#pragma once
#include <string>

#include "virusLib/romfile.h"
#include "virusLib/microcontroller.h"
#include "virusLib/demoplayback.h"
#include "virusLib/dspSingle.h"

class ConsoleApp
{
public:
	ConsoleApp(const std::string& _romFile, virusLib::DeviceModel _tiModel);
	~ConsoleApp();

	bool isValid() const;

	void loadSingle(int b, int p);
	bool loadSingle(const std::string& _preset);

	bool loadDemo(const std::string& _filename);
	bool loadInternalDemo();

	std::string getSingleName() const;
	std::string getSingleNameAsFilename() const;

	static void waitReturn();

	void run(const std::string& _audioOutputFilename, uint32_t _maxSampleCount = 0, uint32_t _blockSize = 64, bool _createDebugger = false, bool _dumpAssembler = false);

	const virusLib::ROMFile& getRom() const { return m_rom; }

private:

	void bootDSP(bool _createDebugger) const;
	dsp56k::IPeripherals& getYPeripherals() const;
	void audioCallback(uint32_t _audioCallbackCount);
	void destroy();

	const std::string m_romName;
	virusLib::ROMFile m_rom;
	std::unique_ptr<virusLib::DspSingle> m_dsp1;
	virusLib::DspSingle* m_dsp2 = nullptr;
	std::unique_ptr<virusLib::Microcontroller> m_uc;
	std::unique_ptr<virusLib::DemoPlayback> m_demo;

	virusLib::Microcontroller::TPreset m_preset;
};
