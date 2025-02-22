#pragma once

#include <map>
#include <list>
#include <unordered_map>
#include <mutex>

#define LOG(msg) std::cout << msg << std::endl;

namespace Utilities
{
	template <typename KEY, typename VAL>
	class LRUCache
	{
	public:
		typedef KEY KeyType;
		typedef VAL ValType;
		typedef std::list<KeyType> KeyTrackingType;
		typedef std::unordered_map<KeyType, std::pair<ValType, typename KeyTrackingType::iterator>> KeyToValType;

		LRUCache(std::function<ValType(const KeyType&)> onCacheMiss, size_t maxCacheSize) noexcept :
			m_OnCacheMiss(onCacheMiss),
			m_MaxCacheSize(maxCacheSize)
		{
			assert(m_MaxCacheSize > 0);
		}

		ValType operator()(const KeyType& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			const auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				//In cache, update tracked by moving key to back
				m_CacheKeyTracker.splice(m_CacheKeyTracker.end(), m_CacheKeyTracker, (*it).second.second);

				return (*it).second.first;
			}
			else {
				//Cache miss, evaluate function and add new record
				const ValType val = m_OnCacheMiss(key);
				AddToCache(key, val);

				return val;
			}
		}

		void UpdateItem(const KeyType& key, const ValType& val)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				(*it).second.first = val;
			}
			else {
				AddToCache(key, val);
			}
		}

		//Remove item from cache so it will be recalculated on next fetch
		void PurgeItem(const KeyType& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			const auto it = m_CacheData.find(key);
			if (it != m_CacheData.end()) {
				m_CacheKeyTracker.erase((*it).second.second);
				m_CacheData.erase(it);
			}
		}

	private:
		void AddToCache(const KeyType& key, const ValType& val)
		{
			assert(m_CacheData.find(key) == m_CacheData.end());

			if (m_CacheData.size() == m_MaxCacheSize) {
				ClearOldestValue();
			}

			const auto it = m_CacheKeyTracker.emplace(m_CacheKeyTracker.end(), key);
			m_CacheData.emplace(key, std::make_pair(val, it));
		}

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


		mutable std::mutex m_Mutex;
	};

}
