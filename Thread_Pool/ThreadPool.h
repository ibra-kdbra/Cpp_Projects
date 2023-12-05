#ifndef _ASYNC_THREAD_POOL_H__
#define _ASYNC_THREAD_POOL_H__

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <future>
#include <functional>

struct Task
{
	std::function<void()> task;
	std::function<void()> callback;
};

class ThreadTask
{
public:
	ThreadTask() {}

	~ThreadTask()
	{
		this->_condition.notify_all();
		this->_thread.join();
	}

	void init()
	{
		_thread = std::thread(
			[this]()
		{
			while (true)
			{
				std::unique_lock<std::mutex> lock(this->_queueMutex);
				this->_condition.wait(lock,
					[this] { return !this->_taskQueue.empty(); });
				auto task = this->_taskQueue.front();
				this->_taskQueue.pop();
				task.task();
				if (task.callback != nullptr)
					task.callback();
			}
		}
		);
	}

	void add(std::function<void()> func, std::function<void()> callback = nullptr)
	{
		Task task;
		task.task = func;
		task.callback = callback;
		_taskQueue.push(task);
		this->_condition.notify_one();
	}

	int getSize()
	{
		return _taskQueue.size();
	}

private:
	std::thread _thread;
	std::queue<Task> _taskQueue;

	std::mutex _queueMutex;
	std::condition_variable _condition;
};

class ThreadPool
{
public:
	ThreadPool() {}

	~ThreadPool()
	{
		for (auto& t : _workers)
		{
			delete t;
		}
	}

	void init(int size)
	{
		for (int i = 0; i < size; ++i)
		{
			ThreadTask *task = new ThreadTask;
			task->init();
			_workers.emplace_back(task);
		}
	}

	void add(std::function<void()> task, std::function<void()> callback = nullptr)
	{
		int index = 0;
		for (int i = 1; i < _workers.size(); ++i)
		{
			if (_workers[i]->getSize() < _workers[index]->getSize())
				index = i;
		}
		_workers[index]->add(task, callback);
	}

private:
	std::vector<ThreadTask*> _workers;

};

#endif
