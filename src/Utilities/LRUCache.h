#pragma once

#include <map>
#include <list>
#include <shared_mutex>
#include <mutex>

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
			//First try to read with shared Lock
			{
				std::shared_lock<std::shared_mutex> readLock(m_Mutex);
				auto it = m_CacheData.find(key);
				if (it != m_CacheData.end())
				{
					//Cache Hit - Copy value while holding the read lock
					ValType val = it->second.first;
					KeyType trackingKey = *(it->second.second);

					//Release Read lock and acquire Write Lock
					readLock.unlock();
					std::unique_lock<std::shared_mutex> writeLock(m_Mutex);

					//Recheck if key exists and iterator valid
					it = m_CacheData.find(key);
					if (it != m_CacheData.end() && *(it->second.second) == trackingKey) {
						m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, it->second.second);
						return val;
					}
				}
			}

			//Handle Cache Miss - (release lock before calling callback)
			ValType val;
			{
				std::unique_lock<std::shared_mutex> tempLock(m_Mutex);
				auto it = m_CacheData.find(key);
				if (it != m_CacheData.end()) {
					val = it->second.first;
					m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, it->second.second);
					return val;
				}
				tempLock.unlock();
			}

			val = m_OnCacheMiss(key);

			//Re-aquire Lock to update cache
			{
				std::unique_lock<std::shared_mutex> writeLock(m_Mutex);

				auto it = m_CacheData.find(key);
				if (it != m_CacheData.end())
				{
					//Some other thread updated cache before we re-acquired lock
					m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, it->second.second);
					return it->second.first;
				}

				AddToCache(key, val);
				return val;
			}
		}

		void UpdateItem(const KeyType& key, const ValType& val)
		{
			std::unique_lock<std::shared_mutex> lock(m_Mutex);
			auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				(*it).second.first = val;
				m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, (*it).second.second);
			}
			else {
				AddToCache(key, val);
			}
		}

		//Remove item from cache so it will be recalculated on next fetch
		void PurgeItem(const KeyType& key)
		{
			std::unique_lock<std::shared_mutex> lock(m_Mutex);

			const auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				m_CacheKeyTracker.erase((*it).second.second);
				m_CacheData.erase(it);
			}
		}

	private:
		//Caller must hold exclusive Lock
		void AddToCache(const KeyType& key, const ValType& val)
		{
			assert(m_CacheData.find(key) == m_CacheData.end());

			if (m_CacheData.size() == m_MaxCacheSize) {
				ClearOldestValue();
			}

			const auto it = m_CacheKeyTracker.insert(m_CacheKeyTracker.end(), key);
			m_CacheData.insert(std::make_pair(
				key,
				std::make_pair(val, it)));
		}

		//Caller must hold exclusive Lock
		void ClearOldestValue()
		{
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

		mutable std::shared_mutex m_Mutex;
	};

}
