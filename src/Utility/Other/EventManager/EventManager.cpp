#include <Utility/EventManager.h>

#include <Utility/LambdaOp.h>

using namespace CppUtility::Other;

EventManager * EventManager::GetInstance() {
	return Singleton<EventManager>::GetInstance();
}

//------------

void EventManager::Register(size_t event, Ptr<Operation> & op) {
	if (directory.find(event) == directory.end())
		directory[event] = ToPtr(new OpQueue);
	directory[event]->Push(op);
}

void EventManager::Register(size_t event, Operation * op) {
	if(op!=nullptr)
		Register(event, ToPtr(op));
}

void EventManager::Register(size_t event, const std::function<void ()> & op) {
	Register(event, new LambdaOp(op));
}

void EventManager::Response(size_t event) {
	auto it = directory.find(event);
	if (it != directory.end())
		it->second->Run();
}