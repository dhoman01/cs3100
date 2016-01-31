#include <queue>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include "ThreadSafeQueue.hpp"

class WorkQueue {
private:
	ThreadSafeQueue<std::function<void(void)>> tasklist;
	std::mutex mutex_pool;
	std::atomic<bool> cont;
	std::vector<std::thread> threadpool;
public:
	WorkQueue():cont(true), threadpool(), tasklist(){};
	~WorkQueue(){ stop(); };
	void start(int);
	void stop();
	void post(std::function<void(void)>);
};
