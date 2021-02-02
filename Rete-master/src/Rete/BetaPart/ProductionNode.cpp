#include "ProductionNode.h"

#include "../TestNode/ParamTestNode.h"
#include "../Utils/VectorPrinter.h"
#include "../Structure/Field.h"

#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
ConditionVector ProductionNode::getOutputInfo(const Token & token) const {
	ConditionVector ret = infoGetter;

	vector<vector<string>> leadtoV, hasProV;
	bool isBayesin = false;
	for (auto&& pair : indexerForInfoGetter) {
		auto&& value = token.at(pair.first.first).get(pair.first.second);//根据规则替换变量  （原来就写好的）
		ret.at(pair.second.first).set(pair.second.second, value);
	} 
	for (int i = 1; i < token.size()+1; i++) {
		ret.at(i).set(Field::px,token.at(i-1).get(Field::px)); //LC  将概率变量设置为匹配成功的三元组的概率
	}
	for (int i = token.size() + 1; i < ret.size(); i++) {
		if ((ret.at(i).get(Field::px)).find(Field::paramPrefix)< (ret.at(i).get(Field::px)).size()) {    // 是否包含变量符号
			ret.at(i).set(Field::px, "1");
		}
	}
	for (int i = 1; i < ret.size(); i++) {  // 判断是否为概率规则，分为Bayesian和全概率规则，并进行相应的操作
		string id = ret.at(i).get(Field::id);
		string attr = ret.at(i).get(Field::attr);
		string value = ret.at(i).get(Field::value);
		string px = ret.at(i).get(Field::px);

		if (attr.find("leadTo") < attr.size()) {
			//			cout << "debug: finded leadTo" << endl;
			leadtoV.push_back(vector<string>{id, value, px});
		}
		if (attr.find("hasProbabilityValueOf") < attr.size()) {
			//			cout << "debug: finded hasProbabilityValueOf" << endl;
			hasProV.push_back(vector<string>{id, value, px});
		}
		if (attr.find("isHappened") < attr.size() && value == "true" && id == leadtoV.at(0).at(1)) {
			//			cout << "debug: ishappend true" << endl;
			isBayesin = true;
		}	
	}
	//一条规则完了统计  若是概率规则，那么计算各个概率
	if (leadtoV.size() != hasProV.size())   cerr << __FILE__ << ": " << __LINE__ << ": error" << endl;
	if (isBayesin) {
		float pden = 0.0;  // 分母部分、
		for (int i = 0; i < leadtoV.size(); i++) {
			pden += atof((leadtoV.at(i).at(2).c_str())) * atof(hasProV.at(i).at(2).c_str());
		}
		for (int i = 0; i < leadtoV.size(); i++) {
			string pb = to_string(
				(atof((leadtoV.at(i).at(2).c_str())) * atof(hasProV.at(i).at(2).c_str())) / pden);
			Condition tmp(leadtoV.at(i).at(0), string(":leadTo"), leadtoV.at(i).at(1), pb);
			ret.push_back(tmp);
		}
	}
	else if (isBayesin == false && leadtoV.size() > 0) {
		float pb = 0.0;
		for (int i = 0; i < leadtoV.size(); i++) {
			auto ret1 = atof((leadtoV.at(i).at(2).c_str()));
			auto ret2 = atof(hasProV.at(i).at(2).c_str());
			pb += ret1*ret2;
		}
		Condition tmp(leadtoV.at(0).at(1), ":hasProbabilityValueOf", to_string(pb), to_string(pb));
		ret.push_back(tmp);
	}


/*	ConditionVector ret;    //LC改
	if(ret.size()==0)
		ret.push_back(infoGetter.at(0));
	vector<vector<string>> leadtoV,hasProV;
	bool isBayesin = false;
	for (auto&& x : token) {
		string id = x.get(Field::id);
		string attr = x.get(Field::attr);
		string value = x.get(Field::value);
		string px = x.get(Field::px);
		Condition tmp(id, attr, value,px);
		ret.push_back(tmp);

		if (attr.find("leadTo")< attr.size()) {
//			cout << "debug: finded leadTo" << endl;
			leadtoV.push_back(vector<string>{id, value, px});
		}
		if (attr.find("hasProbabilityValueOf") < attr.size()) {
//			cout << "debug: finded hasProbabilityValueOf" << endl;
			hasProV.push_back(vector<string>{id, value, px});
		}
		if (attr.find("isHappened") < attr.size()&& value =="true" && id == leadtoV.at(0).at(1)) {
//			cout << "debug: ishappend true" << endl;
			isBayesin = true;
		}
	}
	//一条规则完了统计  若是概率规则，那么计算各个概率
	if (leadtoV.size() != hasProV.size())   cerr << __FILE__ << ": " << __LINE__ << ": error" << endl;
	if (isBayesin) {
		float pden = 0.0;  // 分母部分、
		for (int i = 0; i < leadtoV.size(); i++) {
			pden += atof((leadtoV.at(i).at(2).c_str())) * atof(hasProV.at(i).at(2).c_str());
		}
		for (int i = 0; i < leadtoV.size(); i++) {
			string pb = to_string(
				(atof((leadtoV.at(i).at(2).c_str())) * atof(hasProV.at(i).at(2).c_str())) / pden);
			Condition tmp(leadtoV.at(i).at(0), string(":leadTo"),leadtoV.at(i).at(1),pb );
			ret.push_back(tmp);
		}
	}
	else if(isBayesin == false && leadtoV.size()>0) {
		float pb = 0.0;
		for (int i = 0; i < leadtoV.size(); i++) {
			pb += atof((leadtoV.at(i).at(2).c_str())) * atof(hasProV.at(i).at(2).c_str());
		}
		Condition tmp(leadtoV.at(0).at(1),":hasProbabilityValueOf",to_string(pb), to_string(pb));
		ret.push_back(tmp);
	}*/

	return ret;
}

void ProductionNode::buildIndexer() {
	using namespace Field;
	std::unordered_map<std::string, std::vector<PositionIndexer>> dict;
	for (size_t i = 0; i < infoGetter.size(); ++i) {
		for (auto&& field : { id, attr, value }) {
			auto&& fieldString = infoGetter.at(i).get(field);
			if (Field::isParam(fieldString)) {
				dict[fieldString].emplace_back(i, field);   //这里的模式中各个变量出现的位置，通过模式的序号i和三元组编号确定
			}
		}
	}
	for (size_t i = 0; i < conds.size(); ++i) {
		for (auto&& field : { id, attr, value }) {
			auto&& fieldString = conds.at(i).get(field);
			auto&& it = dict.find(fieldString);
			if (it != dict.end()) {
				for (auto&& positionOfInfoGetter : it->second) {
					indexerForInfoGetter.emplace_back(std::make_pair(i, field), positionOfInfoGetter);
				}
				dict.erase(it);
			}
		}
	}
	myAssert(dict.empty());
}

ProductionNode::ProductionNode(ReteNodePtr leftParent, const ConditionVector & conds, const ConditionVector& infoGetter)
	: ReteNode(false), leftParent(leftParent), conds(conds), infoGetter(infoGetter) {
	buildIndexer();    // LC 删除
}

void ProductionNode::print(int level) const {
	if (!output.empty()) {
		std::cout << "matched productions:" << std::endl;
		VectorPrinter::print(conds, level);
		//std::cout << "matched woking memory elements:" << std::endl;
		//VectorPrinter::print(output, level);
	}
}

const ConditionVector & ProductionNode::getConds() {
	return conds;
}

TokenVector & ProductionNode::getOutput() {
	if (!isReadyForOutput) {
		output = leftParent->getOutput();
		isReadyForOutput = true;


		//if (!output.empty()) {
		//	ConditionVector condsHigherUp;
		//	for (size_t i = 0; i < conds.size(); ++i) {
		//		auto&& c = conds.at(i);
		//		if (c.getType() == Condition::negetive) {
		//			for (auto&& test : ParamTestNode::generate(c, conds)) {
		//				for (size_t j = 0; j < output.size(); ++j) {
		//					for (auto&& fieldName : { Field::id, Field::attr, Field::value }) {
		//						output.at(j).at(i).set(fieldName, c.get(fieldName));
		//					}
		//					output.at(j).at(i).set(test->fieldOfArg1
		//						, output.at(j).at(test->conditionNumberOfArg2).get(test->fieldOfArg2));
		//				}
		//			}
		//			for (size_t j = 0; j < output.size(); ++j) {
		//				output.at(j).at(i).set(Field::attr, c.get(Field::attr) + "_neq");
		//			}
		//		}
		//	}
		//}
	}
	return output;
}

void ProductionNode::clearStatus() {
	if (isReadyForOutput) {
		isReadyForOutput = false;
		output.clear();
		outputInfos.clear();
		leftParent->clearStatus();
	}
}

std::vector<ConditionVector> ProductionNode::getOutputInfos() {
	if (!isReadyForOutput) {
		getOutput();
		outputInfos.reserve(output.size());
		for (auto&& token : output)
			outputInfos.push_back(getOutputInfo(token));
	}
	return outputInfos;
}
