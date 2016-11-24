#pragma once
namespace xraft
{
	namespace detail
	{
		using namespace std::chrono;
		class timer
		{
		public:
			timer()
				
			{
				checker_ = std::thread([this] { run(); });
			}
			int64_t set_timer(int64_t timeout, std::function<void()> &&callback)
			{
				utils::lock_guard lock(mtx_);
				auto timer_point = high_resolution_clock::now()
					+ high_resolution_clock::duration(milliseconds(timeout));
				auto timer_id = gen_timer_id();
				actions_.emplace(std::piecewise_construct, 
					std::forward_as_tuple(timer_point), 
					std::forward_as_tuple(timer_id,std::move(callback)));
				return timer_id;
			}
			void cancel(int64_t timer_id)
			{
				utils::lock_guard lock(mtx_);
				for (auto &itr = actions_.begin(); itr != actions_.end(); itr++)
				{
					if (itr->second.first == timer_id)
					{
						actions_.erase(itr);
						return;
					}
				}
			}
			void stop()
			{
				stop_ = false;
			}
		private:
			void run()
			{
				do 
				{
					std::unique_lock<std::mutex> lock(mtx_);
					if (actions_.empty())
					{
						cv_.wait_for(lock, std::chrono::milliseconds(100));
						continue;
					}
					auto itr = actions_.begin();
					auto timeout_point = itr->first;
					if (timeout_point > high_resolution_clock::now())
					{
						cv_.wait_until(lock, timeout_point, 
							[this] {return !!actions_.size(); });
						continue;
					}
					lock.unlock();
					itr->second.second();

				} while (!stop_);
			}
			int64_t gen_timer_id()
			{
				return ++timer_id_;
			}
			bool stop_ = false;
			std::condition_variable cv_;
			std::mutex mtx_;
			int64_t timer_id_ = 1;
			std::multimap<high_resolution_clock::time_point, 
				std::pair<int64_t, std::function<void()>>> actions_;
			std::thread checker_;
		};
	}
}