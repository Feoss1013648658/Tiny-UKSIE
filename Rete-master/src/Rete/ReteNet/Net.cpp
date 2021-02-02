#include "Net.h"

#include "../BetaPart/JoinNode.h"
#include "../BetaPart/DummyTopNode.h"
#include "../BetaPart/TokenFilterNode.h"

ParamTestNodeVector Net::getTestsFromCondition(Condition c, const ConditionVector & condsHigherUp) {
	ParamTestNodeVector ret;
	auto&& paramTests = ParamTestNode::generate(c, condsHigherUp);
	std::copy(paramTests.begin(), paramTests.end(), std::back_inserter(ret));
	return ret;
}

ReteNodePtr Net::buildOrShareJoinNode(ReteNodePtr parent, AlphaMemoryPtr am
	, const ParamTestNodeVector & tests, const Condition& c) {
	StructForHash node = { parent, am, tests, c };
	auto&& it = dict.find(node);
	if (it == dict.end()) {
		auto&& ret = ReteNodePtr((ReteNode*)new JoinNode(parent, am, tests, c));
		dict.insert(std::make_pair(node, ret));
		return ret;
	}
	else
		return it->second;
}

ReteNodePtr Net::buildOrShareTokenFilterNode(ReteNodePtr parent, AlphaMemoryPtr am
	, const ParamTestNodeVector & tests, const Condition& c) {
	StructForHash node = { parent, am, tests, c };
	auto&& it = dict.find(node);       // dict 中有的ReteNodePtr指向的对象中有parent节点，因此组成了网络？
 	if (it == dict.end()) {
		auto&& ret = ReteNodePtr(new TokenFilterNode(parent, am, tests, c, testAtTokenFilterNode));
		dict.insert(std::make_pair(node, ret));
		return ret;
	}
	else
		return it->second;
}

AlphaMemoryPtr Net::buildOrShareAlphaMemory(const Condition & c) {
	auto&& index = c.getIndex();
	if (conditionToAlphaMemory.count(index) == 0) {
		return conditionToAlphaMemory[index] = AlphaMemoryPtr(new AlphaMemory());
	}
	return conditionToAlphaMemory[index];
}

ReteNodePtr Net::buildOrShareNetworkForConditions(ReteNodePtr parent
	, const ConditionVector & conds, ConditionVector condsHigherUp) { //condsHigherUp是空 不用管
	for (auto&& c : conds) {
		auto&& tests = getTestsFromCondition(c, condsHigherUp);
		auto&& am = buildOrShareAlphaMemory(c);
		if (testAtTokenFilterNode.isNeedFilterToken(c)) {
			parent = buildOrShareTokenFilterNode(parent, am, tests, c);
		}
		else {
			parent = buildOrShareJoinNode(parent, am, tests, c);     // LC 这个循环是在根据当前规则的各个模式建立beta网络。
		}                                                      //一个模式和一个parent节点组成一对，如此循环建立网络
		condsHigherUp.push_back(c);
	}
	return parent;
}

Net::Net() : dummyTopNode(ReteNodePtr((ReteNode*)(new DummyTopNode()))) {
}

size_t i = 0;
void Net::addProduction(const ConditionVector& conditions, const std::vector<Condition>& getter) {
	auto&& curentNode = buildOrShareNetworkForConditions(dummyTopNode, conditions, {});
	resultNodes.insert(ProductionNodePtr(new ProductionNode(curentNode, conditions, getter))); //一个curentNode 就是一条规则所组成的beta网络
}

std::vector<ConditionVector> Net::invoke() {
	std::vector<ConditionVector> ret;
	for (auto&& node : resultNodes) {
		auto&& infos = node->getOutputInfos();
		std::copy(infos.begin(), infos.end(), std::back_inserter(ret)); //插入迭代器(insert iterator)向容器中插入元素  尾插法
	}   // copy函数将infos全部元素复制到ret里面去，采用尾插法
	return ret;
}

void Net::clearStatus() {
	for (auto&& node : resultNodes) {
		node->clearStatus();
	}
//	conditionToAlphaMemory.erase(conditionToAlphaMemory.begin(),conditionToAlphaMemory.end());  // LC
}

void Net::addWME(const WME & wme) {
	std::vector<std::vector<std::string>> vt = {
		{ wme.get(Field::id), Condition::arbitraryString },
		{ wme.get(Field::attr), Condition::arbitraryString },
		{ wme.get(Field::value), Condition::arbitraryString },
		{ wme.get(Field::px), Condition::arbitraryString }    //LC
	};
	for (auto&& id : vt.at(0)) {
		for (auto&& attr : vt.at(1)) {
			for (auto&& value : vt.at(2)) {
//				auto&& px = vt.at(3).at(0);
//				for (auto&& px : vt.at(3)) {           //LC
					auto&& px = vt.at(3).at(0);        //LC
					auto&& it = conditionToAlphaMemory.find(Condition(id, attr, value, px));  //这儿的px 该不该加进去;这里需要加Px 要改等号重载运算函数
					if (it != conditionToAlphaMemory.end()) {
						it->second->addWME(wme);
					}						
//			     }				
			}
		}
	}
}

void Net::addFunction(const std::string & key, TestAtTokenFilterNode::JudgeFunctionType judgeFunction) {
	testAtTokenFilterNode.insertJudgeFunction(key, judgeFunction);
}

bool StructForHash::operator==(const StructForHash & rhs) const {
	return *parent == *rhs.parent && 
		tests == rhs.tests && 
		*alphaMemory == *rhs.alphaMemory && 
		c == rhs.c;
}