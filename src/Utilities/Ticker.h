#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace Utilities
{
	class Ticker
	{
	public:
		Ticker(std::function<void()> onTick, std::chrono::milliseconds interval) :
			m_OnTick(std::move(onTick)),
			m_Interval(interval)
		{}

		void Start()
		{
			if (m_Running.exchange(true)) {
				return;  // already running
			}
			REX::TRACE("Ticker started");
			std::thread([this] { RunLoop(); }).detach();
		}

		void Stop()
		{
			m_Running = false;
		}

		bool IsRunning() const
		{
			return m_Running;
		}

		void UpdateInterval(std::chrono::milliseconds newInterval)
		{
			std::scoped_lock lock(m_IntervalMutex);
			m_Interval = newInterval;
		}

	private:
		void RunLoop()
		{
			while (m_Running) {
				std::chrono::milliseconds interval;
				{
					std::scoped_lock lock(m_IntervalMutex);
					interval = m_Interval;
				}
				std::this_thread::sleep_for(interval);

				if (m_Running && m_OnTick) {
					m_OnTick();
				}
			}
		}

		std::function<void()> m_OnTick;
		std::chrono::milliseconds m_Interval;
		std::mutex m_IntervalMutex;
		std::atomic<bool> m_Running{ false };
	};

}
