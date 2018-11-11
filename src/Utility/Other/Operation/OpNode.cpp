#include <Utility/OpNode.h>

using namespace CppUtility::Other;

OpNode::OpNode(const Operation::Ptr & preOp, const Operation::Ptr & postOp, bool isHold)
	: preOp(preOp), postOp(postOp), OpQueue(isHold) { }

OpNode::OpNode(Operation * preOp, Operation * postOp, bool isHold)
	: OpQueue(isHold){
	this->preOp = preOp != nullptr ? ToPtr(preOp) : nullptr;
	this->postOp = postOp != nullptr ? ToPtr(postOp) : nullptr;
}

OpNode::OpNode(const std::function<void()> & preOp, const std::function<void()> & postOp, bool isHold)
	: OpQueue(isHold){
	this->preOp = preOp != nullptr ? ToPtr(new LambdaOp(preOp)) : nullptr;
	this->postOp = postOp != nullptr ? ToPtr(new LambdaOp(postOp)) : nullptr;
}

//------------
void OpNode::SetPreOp(const Operation::Ptr & preOp) {
	this->preOp = preOp;
}

void OpNode::SetPostOp(const Operation::Ptr & postOp) {
	this->postOp = postOp;
}

void OpNode::SetPreOp(Operation * preOp) {
	SetPreOp(preOp != nullptr ? ToPtr(preOp) : nullptr);
}

void OpNode::SetPostOp(Operation * postOp) {
	SetPreOp(postOp != nullptr ? ToPtr(postOp) : nullptr);
}

size_t OpNode::ChildNum() {
	return Size();
}

void OpNode::Run() {
	if (preOp != nullptr)
		preOp->Run();
	OpQueue::Run();
	if (postOp != nullptr)
		postOp->Run();
}