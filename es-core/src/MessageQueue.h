#include <zmq.hpp>

class MessageQueue
{
public:
	static MessageQueue* getInstance();
	void open();
	void close();

private:
	static MessageQueue* sInstance;
	MessageQueue();
};
