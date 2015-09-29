#include <thread>
#include <atomic>

#include <zmq.hpp>


class MessageQueue
{
public:
	static MessageQueue* getInstance();

	int getTime(bool makeRequest = false);
	bool isTimeChanged();
	int run(std::string command);

private:
	static MessageQueue* sInstance;
	MessageQueue();

	zmq::context_t mContext;
	zmq::socket_t mClient;
	zmq::socket_t mSubscriber;

	std::thread mSubscriberThread;
	void updateRunner();

	std::atomic<bool> mIsTimeChanged;
	std::atomic<int> mTime;
};
