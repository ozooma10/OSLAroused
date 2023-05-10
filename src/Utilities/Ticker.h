#pragma once

#include <functional>
#include <chrono>

namespace Utilities
{
	class Ticker
	{
	public:
		Ticker(std::function<void()> onTick, std::chrono::milliseconds interval) :
			m_OnTick(onTick),
			m_Interval(interval),
			m_Running(false),
			m_ThreadActive(false)
		{}

		void Start()
		{
			if (m_Running) {
				return;
			}
			m_Running = true;
			logger::trace("Start Called with thread active state of: {}", m_ThreadActive);
			if (!m_ThreadActive) {
				std::thread tickerThread(&Ticker::RunLoop, this);
				tickerThread.detach();
			}
		}

		void Stop()
		{
			m_Running = false;
		}

		void UpdateInterval(std::chrono::milliseconds newInterval)
		{
			m_IntervalMutex.lock();
			m_Interval = newInterval;
			m_IntervalMutex.unlock();
		}

	private:
		void RunLoop()
		{
			m_ThreadActive = true;
			while (m_Running) {
				std::thread runnerThread(m_OnTick);
				runnerThread.detach();

				m_IntervalMutex.lock();
				std::chrono::milliseconds interval = m_Interval;
				m_IntervalMutex.unlock();
				std::this_thread::sleep_for(interval);
			}
			m_ThreadActive = false;
		}

		std::function<void()> m_OnTick;
		std::chrono::milliseconds m_Interval;

		std::atomic<bool> m_ThreadActive;
		std::atomic<bool> m_Running;
		std::mutex m_IntervalMutex;
	};

}
