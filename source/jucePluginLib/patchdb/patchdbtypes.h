#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <chrono>

namespace pluginLib::patchDB
{
	enum class SourceType
	{
		Invalid,
		Rom,
		Folder,
		File,
		LocalStorage,
		Count
	};

	enum class DataSourceOrigin
	{
		Invalid,
		Manual,			// manually added datasource by user
		Autogenerated,	// automatically added child as part of a folder being added
	};

	using Tag = std::string;

	enum class TagType
	{
		Invalid,
		Category,
		Tag,
		Favourites,
		CustomA,
		CustomB,
		CustomC,

		Count
	};

	struct Patch;
	using PatchPtr = std::shared_ptr<Patch>;

	struct PatchModifications;
	using PatchModificationsPtr = std::shared_ptr<PatchModifications>;

	using Data = std::vector<uint8_t>;
	using DataList = std::vector<Data>;

	using SearchHandle = uint32_t;

	static constexpr SearchHandle g_invalidSearchHandle = ~0;
	static constexpr uint32_t g_invalidBank = ~0;
	static constexpr uint32_t g_invalidProgram = ~0;

	struct Dirty
	{
		bool dataSources = false;
		bool patches = false;

		std::set<TagType> tags;
		std::set<SearchHandle> searches;
		std::vector<std::string> errors;
	};

	struct DataSource;
	struct DataSourceNode;
	using DataSourceNodePtr = std::shared_ptr<DataSourceNode>;

	std::string toString(SourceType _type);
	std::string toString(TagType _type);

	SourceType toSourceType(const std::string& _string);
	TagType toTagType(const std::string& _string);

	using Timestamp = std::chrono::time_point<std::chrono::system_clock>;

	using Color = uint32_t;
	static constexpr uint32_t g_invalidColor = 0;

	namespace chunks
	{
		constexpr char g_patchManager[] = "Pmpm";
		constexpr char g_patchManagerDataSources[] = "PmDs";
		constexpr char g_patchManagerTagColors[] = "PmTC";
		constexpr char g_patchManagerTags[] = "PmTs";
		constexpr char g_patchManagerPatchModifications[] = "PMds";

		constexpr char g_patchModification[] = "PMod";
		constexpr char g_datasource[] = "DatS";
		constexpr char g_patch[] = "Patc";
		constexpr char g_typedTags[] = "TpTg";
		constexpr char g_tags[] = "Tags";
	}
}