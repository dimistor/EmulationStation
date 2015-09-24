#include "MessageQueue.h"

MessageQueue* MessageQueue::sInstance = NULL;

MessageQueue::MessageQueue()
{
}

MessageQueue* MessageQueue::getInstance()
{
	if(sInstance == NULL)
		sInstance = new MessageQueue();

	return sInstance;
}

void MessageQueue::open()
{
}

void MessageQueue::close()
{
}
