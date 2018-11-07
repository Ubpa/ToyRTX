#include <Utility/OpNode.h>

using namespace CppUtility::Other;

OpNode::OpNode(const Ptr<Operation> & preOp, const Ptr<Operation> & postOp, bool isHold)
	: preOp(preOp), postOp(postOp), OpQueue(isHold) { }

OpNode::OpNode(Operation * preOp, Operation * postOp, bool isHold)
	: OpQueue(isHold){
	this->preOp = preOp != nullptr ? Operation::ToPtr(preOp) : nullptr;
	this->postOp = postOp != nullptr ? Operation::ToPtr(postOp) : nullptr;
}

OpNode::OpNode(const std::function<void()> & preOp, const std::function<void()> & postOp, bool isHold)
	: OpQueue(isHold){
	this->preOp = preOp != nullptr ? Operation::ToPtr(new LambdaOp(preOp)) : nullptr;
	this->postOp = postOp != nullptr ? Operation::ToPtr(new LambdaOp(postOp)) : nullptr;
}

OpNode::~OpNode() {
	printf("INFO: Delete OpNode\n");
}
//------------
void OpNode::SetPreOp(const Ptr<Operation> & preOp) {
	this->preOp = preOp;
}

void OpNode::SetPostOp(const Ptr<Operation> & postOp) {
	this->postOp = postOp;
}

void OpNode::SetPreOp(Operation * preOp) {
	SetPreOp(preOp != nullptr ? Operation::ToPtr(preOp) : nullptr);
}

void OpNode::SetPostOp(Operation * postOp) {
	SetPreOp(postOp != nullptr ? Operation::ToPtr(postOp) : nullptr);
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