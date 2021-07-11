#pragma once
#include "shade/config/API.h"

namespace shade
{
	namespace util
	{
		class ThreadPool
		{
			using Task = std::function<void()>;
		public:
			ThreadPool(std::size_t threadsCount = std::thread::hardware_concurrency());
			~ThreadPool();
		private:
			void Start(std::size_t threadsCount);
			void Quit() noexcept;
		private:
			std::queue<std::function<void()>>	m_Tasks;
			std::vector<std::thread>			m_Threads;
			std::condition_variable				m_Event;
			std::mutex							m_Mutex;
			bool								m_Quit;
		public:
			template<class T>
			auto Emplace(T task)->std::future<decltype(task())>
			{
				auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
				{
					std::unique_lock<std::mutex> lock{ m_Mutex };
					m_Tasks.emplace([=] {
						(*wrapper)();
						});
				}

				m_Event.notify_one();
				return wrapper->get_future();
			}
		};
	}
}