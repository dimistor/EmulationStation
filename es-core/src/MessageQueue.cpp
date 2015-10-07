#include <thread>

#include "MessageQueue.h"
#include "Settings.h"
#include "Log.h"

MessageQueue* MessageQueue::sInstance = nullptr;

MessageQueue::MessageQueue() : mContext(1), mTimer(0), mIsRunning(true), mClient(mContext, ZMQ_REQ), mSubscriber(mContext, ZMQ_SUB)
{
	std::string server = Settings::getInstance()->getString("ZmqServer");
	std::string publisher = Settings::getInstance()->getString("ZmqPublisher");

	if(!server.empty())
		mClient.connect(server.c_str());

	if(!publisher.empty())
	{
		mSubscriber.connect(publisher.c_str());
		mSubscriber.setsockopt(ZMQ_SUBSCRIBE, "timer", 5);

		mSubscriberThread = std::thread(&MessageQueue::mSubscriberUpdater, this);
		mSubscriberThread.detach();
	}
}

MessageQueue::~MessageQueue()
{
	mIsRunning.store(false);
}

MessageQueue* MessageQueue::getInstance()
{
	if(sInstance == nullptr)
		sInstance = new MessageQueue();

	return sInstance;
}

unsigned int MessageQueue::getTimer(bool makeRequest)
{
	if(!makeRequest)
		return mTimer.load();

	if(!mClient.connected())
		return 0;

	std::string reqCmd = "get_time";
	zmq::message_t req(reqCmd.size());
	memcpy(req.data(), reqCmd.data(), reqCmd.size());
	mClient.send(req);

	zmq::message_t rep;
	mClient.recv(&rep);
	std::string reply = std::string(static_cast<char*>(rep.data()), rep.size());
	unsigned int seconds = 0;
	try
	{
		seconds = std::stoi(reply);
		mTimer.store(seconds);
	}
	catch(const std::invalid_argument &ex)
	{
		LOG(LogError) << "Error converting timer value '" << reply << "' to integer";
	}

	return seconds;
}

int MessageQueue::runSystemCommand(const std::string& cmd_utf8)
{
	if(!mClient.connected())
		return -1;

	std::string reqCmd = "run_cmd";
	zmq::message_t req1(reqCmd.size());
	memcpy(req1.data(), reqCmd.data(), reqCmd.size());
	zmq::message_t req2(cmd_utf8.size());
	memcpy(req2.data(), cmd_utf8.data(), cmd_utf8.size());
	mClient.send(req1, ZMQ_SNDMORE);
	mClient.send(req2);

	zmq::message_t rep;
	mClient.recv(&rep);
	std::string reply = std::string(static_cast<char*>(rep.data()), rep.size());
	int exitcode = -1;
	try
	{
		exitcode = std::stoi(reply);
	}
	catch(const std::invalid_argument &ex)
	{
		LOG(LogError) << "Error converting exit code value '" << reply << "' to integer";
	}

	return exitcode;
}

void MessageQueue::mSubscriberUpdater()
{
	while(mIsRunning.load())
	{
		try
		{
			zmq::message_t event, data;
			if(mSubscriber.recv(&event) && mSubscriber.recv(&data))
			{
				auto event_s = std::string(static_cast<char*>(event.data()), event.size());
				if(event_s == "timer")
				{
					auto data_s = std::string(static_cast<char*>(data.data()), data.size());
					int seconds = std::stoi(data_s);
					mTimer.store(seconds);
				}
			}
		}
		catch(const zmq::error_t &ex)
		{
			if(ex.num() != ETERM)
				LOG(LogError) << "MessageQueue: " << std::string(ex.what());
		}
	}
}
