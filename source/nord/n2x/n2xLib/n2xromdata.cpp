#include "n2xromdata.h"

#include "n2xtypes.h"

#include "baseLib/filesystem.h"

#include "synthLib/os.h"

namespace n2x
{
	template <uint32_t Size> RomData<Size>::RomData() : RomData(synthLib::findROM(MySize, MySize))
	{
	}

	template <uint32_t Size> RomData<Size>::RomData(const std::string& _filename)
	{
		if(_filename.empty())
			return;
		if(!baseLib::filesystem::readFile(m_data, _filename))
			return;
		if(m_data.size() != MySize)
			return;
		m_filename = _filename;
	}

	template <uint32_t Size> RomData<Size>::RomData(const std::vector<uint8_t>& _data, const std::string& _filename)
	{
		if(_data.size() != MySize)
			return;
		m_data = _data;
		m_filename = _filename;
	}

	template <uint32_t Size> void RomData<Size>::saveAs(const std::string& _filename) const
	{
		baseLib::filesystem::writeFile(_filename, m_data);
	}

	template class RomData<g_flashSize>;
	template class RomData<g_romSize>;
}
