#pragma once

#include <array>
#include <cstdint>

namespace xtJucePlugin
{
	using WaveData = std::array<int8_t, 128>;

	enum class WaveCategory
	{
		Invalid = -1,

		Rom,
		User,
		Plugin,

		Count
	};

	static constexpr float g_waveEditorScale = 2.0f * 1.3f;
}
