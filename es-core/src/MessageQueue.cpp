#include <thread>

#include "MessageQueue.h"
#include "Settings.h"

MessageQueue* MessageQueue::sInstance = nullptr;

MessageQueue::MessageQueue() : mContext(1), mTime(-1), mIsTimeChanged(false),
mClient(mContext, ZMQ_REQ), mSubscriber(mContext, ZMQ_SUB)
{
	// std::string server = Settings::getInstance()->getString("ZmqAddress");
	// std::string publisher = Settings::getInstance()->getString("ZmqAddress");
	std::string server = "tcp://127.0.0.1:3001";
	std::string publisher = "tcp://127.0.0.1:3002";

	mClient.connect(server.c_str());

	mSubscriber.connect(publisher.c_str());
	mSubscriber.setsockopt(ZMQ_SUBSCRIBE, "timer", 5);

	mSubscriberThread = std::thread(&MessageQueue::updateRunner, this);
	mSubscriberThread.detach();
}

MessageQueue* MessageQueue::getInstance()
{
	if(sInstance == nullptr)
		sInstance = new MessageQueue();

	return sInstance;
}

int MessageQueue::getTime(bool makeRequest)
{
	if(!makeRequest)
		return mTime.load();

	zmq::message_t req, rep;
	mClient.send(req);
	mClient.recv(&rep);
	return 0;
}

bool MessageQueue::isTimeChanged()
{
	return mIsTimeChanged.load();
}

int MessageQueue::run(std::string command)
{
	if(!mClient.connected())
		return -1;

	zmq::message_t req, rep;
	mClient.send(req);
	mClient.recv(&rep);

	return 0;
}

void MessageQueue::updateRunner()
{
	while(true)
	{
		zmq::message_t event, data;
		if(mSubscriber.recv(&event) && mSubscriber.recv(&data))
		{
			auto event_s = std::string(static_cast<char*>(event.data()), event.size());
			if(event_s == "timer")
			{
				auto data_s = std::string(static_cast<char*>(data.data()), data.size());
				int seconds = std::stoi(data_s);
				int last = mTime.load();
				if(last != seconds)
				{
					mTime.store(seconds);
					mIsTimeChanged.store(true);
				}
			}
		}
	}
}
