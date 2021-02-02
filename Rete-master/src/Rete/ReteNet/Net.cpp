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
	auto&& it = dict.find(node);       // dict ���е�ReteNodePtrָ��Ķ�������parent�ڵ㣬�����������磿
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
	, const ConditionVector & conds, ConditionVector condsHigherUp) { //condsHigherUp�ǿ� ���ù�
	for (auto&& c : conds) {
		auto&& tests = getTestsFromCondition(c, condsHigherUp);
		auto&& am = buildOrShareAlphaMemory(c);
		if (testAtTokenFilterNode.isNeedFilterToken(c)) {
			parent = buildOrShareTokenFilterNode(parent, am, tests, c);
		}
		else {
			parent = buildOrShareJoinNode(parent, am, tests, c);     // LC ���ѭ�����ڸ��ݵ�ǰ����ĸ���ģʽ����beta���硣
		}                                                      //һ��ģʽ��һ��parent�ڵ����һ�ԣ����ѭ����������
		condsHigherUp.push_back(c);
	}
	return parent;
}

Net::Net() : dummyTopNode(ReteNodePtr((ReteNode*)(new DummyTopNode()))) {
}

size_t i = 0;
void Net::addProduction(const ConditionVector& conditions, const std::vector<Condition>& getter) {
	auto&& curentNode = buildOrShareNetworkForConditions(dummyTopNode, conditions, {});
	resultNodes.insert(ProductionNodePtr(new ProductionNode(curentNode, conditions, getter))); //һ��curentNode ����һ����������ɵ�beta����
}

std::vector<ConditionVector> Net::invoke() {
	std::vector<ConditionVector> ret;
	for (auto&& node : resultNodes) {
		auto&& infos = node->getOutputInfos();
		std::copy(infos.begin(), infos.end(), std::back_inserter(ret)); //���������(insert iterator)�������в���Ԫ��  β�巨
	}   // copy������infosȫ��Ԫ�ظ��Ƶ�ret����ȥ������β�巨
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
					auto&& it = conditionToAlphaMemory.find(Condition(id, attr, value, px));  //�����px �ò��üӽ�ȥ;������Ҫ��Px Ҫ�ĵȺ��������㺯��
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