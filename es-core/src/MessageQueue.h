#pragma once

#include <thread>
#include <atomic>

#include <zmq.hpp>


class MessageQueue
{
public:
	static MessageQueue* getInstance();
	~MessageQueue();

	unsigned int getTimer(bool makeRequest = false);
	int runSystemCommand(const std::string& cmd_utf8);

private:
	static MessageQueue* sInstance;
	MessageQueue();

	zmq::context_t mContext;
	zmq::socket_t mClient;
	zmq::socket_t mSubscriber;

	std::atomic<bool> mIsRunning;
	std::thread mSubscriberThread;
	void mSubscriberUpdater();

	std::atomic<unsigned int> mTimer;
};
