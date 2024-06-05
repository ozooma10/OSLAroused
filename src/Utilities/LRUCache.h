#pragma once

#include <map>
#include <list>
#include "Spinlock.h"

namespace Utilities
{
	template <typename KEY, typename VAL>
	class LRUCache
	{
	public:
		typedef KEY KeyType;
		typedef VAL ValType;
		typedef std::list<KeyType> KeyTrackingType;
		typedef std::map<KeyType, std::pair<ValType, typename KeyTrackingType::iterator>> KeyToValType;

		LRUCache(std::function<ValType(const KeyType&)> onCacheMiss, size_t maxCacheSize) :
			m_OnCacheMiss(onCacheMiss),
			m_MaxCacheSize(maxCacheSize)
		{
			assert(m_MaxCacheSize > 0);
		}

		ValType operator()(const KeyType& key)
		{
            auto loc_lock = UniqueLock(m_Lock);
			const auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				//In cache, update tracked by moving key to back
				m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, (*it).second.second);

				return (*it).second.first;
			} else {
				//Cache miss, evaluate function and add new record
				const ValType val = m_OnCacheMiss(key);
                m_Lock.Unlock();
				AddToCache(key, val);

				return val;
			}
		}

		void UpdateItem(const KeyType& key, const ValType& val)
		{
            auto loc_lock = UniqueLock(m_Lock);
			auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				(*it).second.first = val;
			} else {
                m_Lock.Unlock();
				AddToCache(key, val);
			}
		}

		//Remove item from cache so it will be recalculated on next fetch
		void PurgeItem(const KeyType& key)
		{
            auto loc_lock = UniqueLock(m_Lock);
			const auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				m_CacheKeyTracker.erase((*it).second.second);
				m_CacheData.erase(it);
			}
		}

	private:
		void AddToCache(const KeyType& key, const ValType& val)
		{
            auto loc_lock = UniqueLock(m_Lock);

			assert(m_CacheData.find(key) == m_CacheData.end());

			if (m_CacheData.size() == m_MaxCacheSize) {
                m_Lock.Unlock();
				ClearOldestValue();
                m_Lock.Lock();
			}

			const auto it = m_CacheKeyTracker.insert(m_CacheKeyTracker.end(), key);
			m_CacheData.insert(std::make_pair(
				key,
				std::make_pair(val, it)));
		}

		void ClearOldestValue()
		{
            auto loc_lock = UniqueLock(m_Lock);
			assert(!m_CacheKeyTracker.empty());

			//Front of tracker has oldest access
			const auto it = m_CacheData.find(m_CacheKeyTracker.front());
			assert(it != m_CacheData.end());

			m_CacheData.erase(it);
			m_CacheKeyTracker.pop_front();
		}

		std::function<ValType(const KeyType&)> m_OnCacheMiss;
		const size_t m_MaxCacheSize;

		KeyTrackingType m_CacheKeyTracker;
		KeyToValType m_CacheData;
        Spinlock m_Lock;
	};

}
