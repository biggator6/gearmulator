#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "patchdbtypes.h"

namespace baseLib
{
	class BinaryStream;
}

namespace juce
{
	class DynamicObject;
}

namespace pluginLib::patchDB
{
	class Tags
	{
	public:
		bool add(const Tag& _tag)
		{
			const auto cA = m_added.size();
			const auto cR = m_removed.size();

			m_added.insert(_tag);
			m_removed.erase(_tag);

			return cA != m_added.size() || cR != m_removed.size();
		}

		bool erase(const Tag& _tag)
		{
			const auto cA = m_added.size();
			const auto cR = m_removed.size();

			m_added.erase(_tag);
			m_removed.erase(_tag);

			return cA != m_added.size() || cR != m_removed.size();
		}

		bool addRemoved(const Tag& _tag)
		{
			const auto cA = m_added.size();
			const auto cR = m_removed.size();

			m_added.erase(_tag);
			m_removed.insert(_tag);

			return cA != m_added.size() || cR != m_removed.size();
		}

		const auto& getAdded() const { return m_added; }
		const auto& getRemoved() const { return m_removed; }

		bool containsAdded() const
		{
			return !m_added.empty();
		}

		bool containsAdded(const Tag& _tag) const
		{
			return m_added.find(_tag) != m_added.end();
		}

		bool containsRemoved(const Tag& _tag) const
		{
			return m_removed.find(_tag) != m_removed.end();
		}

		bool empty() const
		{
			return m_added.empty() && m_removed.empty();
		}

		void write(baseLib::BinaryStream& _s) const;
		bool read(baseLib::BinaryStream& _stream);

		bool operator == (const Tags& _t) const;

	private:
		std::unordered_set<Tag> m_added;
		std::unordered_set<Tag> m_removed;
	};

	class TypedTags
	{
	public:
		const Tags& get(TagType _type) const;
		const auto& get() const { return m_tags; }
		bool add(TagType _type, const Tag& _tag);
		bool add(const TypedTags& _tags);
		bool erase(TagType _type, const Tag& _tag);
		bool addRemoved(TagType _type, const Tag& _tag);
		bool containsAdded() const;
		bool containsAdded(TagType _type, const Tag& _tag) const;
		bool containsRemoved(TagType _type, const Tag& _tag) const;
		void clear();
		bool empty() const;
		juce::DynamicObject* serialize() const;
		void deserialize(juce::DynamicObject* _obj);
		bool operator == (const TypedTags& _tags) const;

		void write(baseLib::BinaryStream& _s) const;
		bool read(baseLib::BinaryStream& _stream);

	private:
		std::unordered_map<TagType, Tags> m_tags;
	};
}
