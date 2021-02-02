#include "GA.h"
#include <map>
#include <cstdlib>// Header file needed to use rand
#include <set>
#include <ctime>
#include <fstream>
#include<unistd.h>
#include<stdlib.h>
#include "../main.h"
#include <cmath>

GA::GA(string time_file_path) {
	this->readRuler = new ReadFile;
	max_iterations = 200;
	cur_iterations = 0;
	this->time_factor = 0.989;
	time_file.open(time_file_path, ios_base::out | ios_base::trunc);
	if (!time_file.is_open()) {
		cerr << " last_GA_ruleset file not oppen" << endl;
		exit(0);
	}
}

GA::~GA(){
	if(this->readRuler != NULL){
		delete this->readRuler;		
	}
	
}

pair<vector<ConditionVector>, vector<ConditionVector>> GA::sort_ruler(pair<vector<ConditionVector>, vector<ConditionVector>>& condvecs) {

	vector<ConditionVector>& conditionvectors = condvecs.first;
	vector<ConditionVector>& gett = condvecs.second;
	unordered_map<string, int> conditions_map;
	for (auto& ruler : conditionvectors) {
		int n = 1;
		for (auto& condition : ruler) {
			string s = condition.get(Field::attr);
			if (conditions_map.find(s) != conditions_map.end()) {
				conditions_map[s]++;
			}
			else {
				conditions_map.insert(make_pair(s,1));
			}
		}
	}
	vector<ConditionVector> sorted_rulers;
	vector<ConditionVector> sorted_gett;
	int k = 0;
	for (auto& ruler : conditionvectors) {
		int n = 1;
		vector<pair<Condition, int>> tmp;
		vector<Condition> tmp2;         //add
		int t = 1;
		int t_hasp = 1;
		for (auto& condition: ruler) {
			auto ret = condition.get(Field::attr);
			auto it = conditions_map.find(ret);
			if (it != conditions_map.end()) {
				tmp.push_back(make_pair(condition, it->second));
			}
			// 对未排序的leadto规则插入序号
			if (ret.find("leadTo") < ret.size()) {
				ret.append(to_string(t++));
				condition.set(Field::attr, ret);
			}
			//对位排序的hasProbability规则插入序号   //不能对这个插入序号，在导入wme时会出错，因为在wmes中leadto 和hasP出现的顺序不定
/*			if (ret.find("hasProbabilityValueOf") < ret.size()) {
				ret.append(to_string(t_hasp++));
				condition.set(Field::attr, ret);
			}*/
		}
		sort(tmp.begin(), tmp.end(), [](pair<Condition, int>& a, pair<Condition, int>& b) {
			return a.second > b.second;
			});
		ConditionVector aSortedRuler;
		tmp2.push_back(gett.at(k).at(0));   //add
		t_hasp = 1;
		for (auto cond : tmp) {
			string ret = cond.first.get(Field::attr);
			if (ret.find("leadTo") < ret.size()) {
				ret.append(to_string(n));
				n ++ ;
				cond.first.set(Field::attr,ret);
			}
			//对排序的hasProbability规则插入序号
/*			if (ret.find("hasProbabilityValueOf") < ret.size()) {
				ret.append(to_string(t_hasp));
				t_hasp++;
				cond.first.set(Field::attr, ret);
			}*/
			aSortedRuler.push_back(cond.first);
			tmp2.push_back(cond.first);   //
		}
		for (int i = 1 + tmp.size(); i < gett.at(k).size(); i++) {    //add
			tmp2.push_back(gett.at(k).at(i));
		}
		sorted_rulers.push_back(aSortedRuler);
		sorted_gett.push_back(tmp2);
		k++;
	}
	auto ret = make_pair(sorted_rulers, sorted_gett);
	return ret ;
} 

/* 规则文件读取  */
/*pair<vector<ConditionVector>, vector<ConditionVector>> GA::readRulers(string rulerFile) {
	auto&& allConditions = readRuler->readRulers();
	return allConditions;
}*/
int creatRandNum(int scope) {
	unsigned long seed = time(NULL);
	srand(seed);

	return rand() % scope;
}

void cross(map<int, int>& canCross_map, vector<vector<pair<int, string>>>& population) {

	int cross_num = population.size() * 0.5;  // 默认交叉率0.5
	vector<vector<int>> cross_index;
	for (int i = 0; i < cross_num/2; i++) {
		vector<int> cross_pair;
		cross_pair.push_back(creatRandNum(population.size()));
		int tmp;
		do {
			tmp = creatRandNum(population.size());
		} while (cross_pair.at(0) == tmp);
		cross_pair.push_back(tmp);
		//选择一个交叉点
		int tt = creatRandNum(canCross_map.size());
		int x = 0;
		for (auto it = canCross_map.begin(); it != canCross_map.end(); it++) {
			if (x < tt) {
				x++;
				continue;
			}
			cross_pair.push_back(it->second);
		}
		cross_index.push_back(cross_pair); // 一个cross_pair中包含了第一条染色体，第二条染色体和交叉点
	}
	for (int i = 0; i < cross_index.size(); i++) {
		vector<pair<int, string>>& chr1 = population.at(cross_index.at(i).at(0));
		vector<pair<int, string>>& chr2 = population.at(cross_index.at(i).at(1));
		int cross_point = cross_index.at(i).at(2);
		vector<pair<int, string>> tmp1(chr1.begin()+ cross_point,chr1.end());
		vector<pair<int, string>> tmp2(chr2.begin() + cross_point, chr2.end());
		auto it1 = chr1.erase(chr1.begin() + cross_point, chr1.end());
		auto it2 = chr2.erase(chr2.begin() + cross_point, chr2.end());
		chr1.insert(it1,tmp2.begin(),tmp2.end());
		chr2.insert(it2,tmp1.begin(), tmp1.end());
	}
	
}
 
void mutation_one(map<int, vector<int>>& canMutation_map, vector<pair<int, string>>& preVector) {  //变异一条染色体
	
	int scope = canMutation_map.size();
	int randNum = creatRandNum(scope);
	vector<int> MutationEle;
	int MutationPoint1 = -1, MutationPoint2 = -1;
	int tt = 0;
	for (auto it = canMutation_map.begin(); it != canMutation_map.end(); it++) {
		if (tt < randNum) {
			tt++;
			continue;
		}
		else {
			MutationEle = it->second;
			MutationPoint1 = MutationEle[creatRandNum(MutationEle.size())];
			MutationPoint2 = -1;
			do {
				MutationPoint2 = MutationEle[creatRandNum(MutationEle.size())];
			} while (MutationPoint1 == MutationPoint2);
			break;
		}
	}
	string tmp = preVector.at(MutationPoint1).second;
	preVector.at(MutationPoint1).second = preVector.at(MutationPoint2).second;
	preVector.at(MutationPoint2).second = tmp;
}

void mutation(map<int, vector<int>>& canMutation_map, vector<vector<pair<int, string>>>& population) {
	
	float mut_rate = 0.25;  //设置一条变异
	int mut_num = population.size() * mut_rate;
	for (int i = 0; i < mut_num;i++) {
		int mut_index = creatRandNum(population.size());
		mutation_one(canMutation_map,population.at(mut_index));
	}
}
pair<vector<ConditionVector>, vector<ConditionVector>>  ruleset_redefine_once(vector<pair<int, string>>& population_i,
	pair<vector<ConditionVector>, vector<ConditionVector>>& condvecs
	) {  //根据染色体顺序，重排一次规则集
	unordered_map<string, int> pre_map;
	vector<ConditionVector> sorted_condvec;
	vector<ConditionVector> sorted_condvec_getter;
	for (int i = 0; i < population_i.size(); i++) {
		pre_map.insert(make_pair(population_i.at(i).second,population_i.at(i).first));
	}


	int k = 0;
	for (auto& rule : condvecs.first) {
		ConditionVector aRule;
		ConditionVector aRule_getter;
		vector<pair<int, Condition>> tmp;
		for (auto& conditon : rule) {
			string pre = conditon.get(Field::attr);
			unordered_map<string, int>::iterator it;
			if (pre.find("leadTo") < pre.size()) {
				 it = pre_map.find("leadTo");
			}
			else {
			   it = pre_map.find(pre);
			}
			if (it != pre_map.end()) {
				tmp.push_back(make_pair(it->second, conditon));
			}else{
				cerr << __FILE__ <<__LINE__ << ":find err " << endl;
				exit(0);
			}
		}
		sort(tmp.begin(), tmp.end(), [](pair<int, Condition>& a, pair<int, Condition>& b) {
				return a.first > b.first;
			});
		aRule_getter.push_back(condvecs.second.at(k).at(0));
		for (auto x : tmp) {
			aRule.push_back(x.second);
			aRule_getter.push_back(x.second);
		}
		for (int i = 1 + tmp.size(); i < condvecs.second.at(k).size(); i++) {    //add
			aRule_getter.push_back(condvecs.second.at(k).at(i));
		}
		k++;
		sorted_condvec.push_back(aRule);
		sorted_condvec_getter.push_back(aRule_getter);
		//这里先不对 getter(匹配到以后的规则)中的内容进行排序
	}
	return make_pair(sorted_condvec, sorted_condvec_getter);   //返回一个规则集
}
vector<pair<vector<ConditionVector>, vector<ConditionVector>>>  ruleset_redefine(vector<vector<pair<int, string>>>& population,  // 返回染色体们对应的规则集
	pair<vector<ConditionVector>, vector<ConditionVector>>& condvecs) {

	vector<pair<vector<ConditionVector>, vector<ConditionVector>>>sorted_rulesset;
	for (int i = 0; i < population.size(); i++) {
		sorted_rulesset.push_back( ruleset_redefine_once(population.at(i), condvecs));
	}
	return sorted_rulesset;
}



int GA::evaluation(vector<vector<pair<int, string>>>& population,
	vector<pair<vector<ConditionVector>, vector<ConditionVector>>>& sorted_rulesset,
	string srcTTL,
	string desTTL,
	string resultFile,
	vector<double>& ret_evaluation_values
	) {
	vector<double> evaluation_values;
	for (int i = 0; i < population.size(); i++) {
		double ret = test(sorted_rulesset.at(i), srcTTL, desTTL, resultFile);
		ret = time_statistic(0.0,this->src_time*1.9,0.06,this->cur_iterations,this->max_iterations);
		evaluation_values.push_back(ret);
		time_file << ret << "  ";
	}
	time_file << endl;
	int min_idex = -1;
	double min = 100000.000;
	for (int i = 0; i < evaluation_values.size(); i++) {
		if (min -evaluation_values.at(i) >1e-5 ) {
			min = evaluation_values.at(i);
			min_idex = i;
		}
	}
	time_file << "min time" << min << endl;
	return min_idex;   //返回最少时间个体的下标
}

vector<vector<pair<int, string>>> select(vector<double>& evaluation_values,
	vector<vector<pair<int, string>>>& population
	) {
	float pass_rate = 0.5;
	int pass_num = population.size() * pass_rate;
	double total_time = 0.0;
	vector<int> lucky_pat;
	vector<vector<pair<int, string>>> new_population;
	//开始选择
	for (auto x : evaluation_values) {
		total_time += x;
	}
	int acumalate_value = 0;
	for (auto x : evaluation_values) {
		int tmp = acumalate_value + int(x / total_time * 1000);
		acumalate_value = tmp;
		lucky_pat.push_back(tmp);
	}
	vector<vector<pair<int, string>>>::iterator it;
	vector<int> willdelete;
	for (int i = 0; i < pass_num; i++) {
		int randnum = creatRandNum(1000);
		for (int j = 0; j < lucky_pat.size(); j++) {
			if (j == 0 && randnum <= lucky_pat.at(j)) {
				willdelete.push_back(j);
			}
			else if (randnum > lucky_pat.at(j - 1) && randnum <= lucky_pat.at(j)) {				 
					willdelete.push_back(j);
					break;
			}
		}
	}
	for (int i = 0; i < population.size(); i++) {
		if (find(willdelete.begin(), willdelete.end(), i) == willdelete.end()) {
			new_population.push_back(population.at(i));
		}
	}
	//复制操作
	int complement_chr = population.size() - new_population.size();
	for (int i = 0; i < complement_chr; i++){                   
		int ret = creatRandNum(population.size());
		new_population.push_back(population.at(ret));
	}
	return new_population;
}
vector<vector<pair<int, string>>> population_generate(map<int, vector<int>> & canMutation_map,vector<pair<int, string>>& preVector) {
	int gen_num = 4;
	vector<vector<pair<int, string>>> population(4);
	vector<pair<int, string>> preVectortmp = preVector;
	for (int i = 0; i < gen_num; i++) {
		if (i == 0) {
			population.at(i) = preVectortmp;
			continue;
		}
		else {
			mutation_one(canMutation_map, preVectortmp);
		}
		population.at(i)= preVectortmp;
	}
	return population;
}

void GA::GAalgorithm() {

//	string rulerFile = "./last_GA_ruleset_bak.rule";
	string rulerFile = "./myruler2.ruler";
	string srcTTL = "./smartFactory.ttl";
	string desTTL = "./smartFactory_Des.ttl";
	string resultFile = "./result";
	string last_GA_ruleset_file = "./last_GA_ruleset.rule";
	fstream last_rule_out(last_GA_ruleset_file,ios_base::out|ios_base::trunc);
	double unorderd_time_factor = 2.9;
	if (!last_rule_out.is_open()) {
		cerr << " last_GA_ruleset file not oppen" << endl;
		exit(0);
	}
	

	auto&& allConditions = readRuler->readRulers(rulerFile);
	auto&& sortedRulers = sort_ruler(allConditions);
	double unorded_time = test(allConditions, srcTTL, desTTL, resultFile);   //未排序之前的推理
	this->src_time = unorded_time;
	time_file << "unordered ruleset" << unorded_time*unorderd_time_factor << endl;

//	int num = INT_MAX;
//	while (num--);
//	test(sortedRulers, srcTTL, desTTL, resultFile);    //排序之后的推理
	//遗传算法优化阶段
	unordered_map<string, int> conditions_map;
	vector<pair<int, string>> preVector;
	for (auto& ruler : sortedRulers.first) { //统计谓语出现的次数
		int n = 1;
		for (auto& condition : ruler) {
			string s = condition.get(Field::attr);
			if (s.find("leadTo") < s.size()) {
				s = "leadTo";
			}
			if (conditions_map.find(s) != conditions_map.end()) {
				conditions_map[s] += 100;
			}
			else {
				conditions_map.insert(make_pair(s,100));
			}
		}
	}
	unordered_map<int, int> tmp_hash;
	for (auto it = conditions_map.begin(); it != conditions_map.end(); it++) {
		auto it_hash = tmp_hash.find(it->second);
		if (it_hash == tmp_hash.end()) {
			preVector.push_back(make_pair(it->second, it->first));
			tmp_hash.insert(make_pair(it->second,1));
		}
		else {
			preVector.push_back(make_pair((it->second+it_hash->second), it->first));
			it_hash->second += 1;
		}
	}
	sort(preVector.begin(), preVector.end(), [](pair<int, string>& a, pair<int, string>& b) {  //按谓语出现次数降序排列
		return a.first > b.first;
		});
	map<int, vector<int>> canMutation_map;  //生成可变异的谓语的序号范围map
	set<int> canMutation_index_set;
	map<int, int> canCross_map; //生成可交叉的序号map <出现次数，在preVector中的序号>
	for (int i = 0; i < preVector.size(); i++) {
		auto it = canMutation_map.find((preVector.at(i).first / 100));
		if ( it == canMutation_map.end()) {
			canMutation_map.insert(make_pair(preVector.at(i).first / 100, vector<int>{i}));
			canMutation_index_set.insert(preVector.at(i).first / 100);
		}
		else {
			it->second.push_back(i);
		}
	}
	for (auto it = canMutation_map.begin(); it != canMutation_map.end();) {
		if (it->second.size() == 1) {
			canCross_map.insert(make_pair(it->first,it->second.at(0)));
			canMutation_index_set.erase(it->first);
			it = canMutation_map.erase(it);			
			continue;
		}
		it++;
	}
	//
	vector<double> evaluation_values;
	cout << "population initial" << endl;
	auto populations = population_generate(canMutation_map, preVector);
	int min_index = 0;
	while (this->cur_iterations <this->max_iterations) {
		cout << "cur_iterations: " << this->cur_iterations <<" max_iterations: "<<this->max_iterations<< endl;
		auto GA_rulesset = ruleset_redefine(populations, sortedRulers);
		cout << "finished ruleset_redefine" << endl;
		min_index = evaluation(populations, GA_rulesset, srcTTL, desTTL, resultFile, evaluation_values);
		cout << "finished evaluation" << endl;
		populations = select(evaluation_values, populations);
		cout << "finished select" << endl;
		cross(canCross_map, populations);
		cout << "finished cross " << endl;
		mutation(canMutation_map, populations);
		cout << "finished mutation " << endl;
		this->cur_iterations++;
	}
	auto last_GA_ruleset = ruleset_redefine_once(populations.at(min_index), sortedRulers);
	int tt = 0;
	for (auto& ruler : last_GA_ruleset.first) {
		last_rule_out << "START" << endl;
		last_rule_out << "\tPRS:" << endl;
		string ss;
		for (auto& cond : ruler) {
			ss = cond.get(Field::attr);
			if (ss.find("leadTo") < ss.size()) {
				ss = ":leadTo";
			}
			last_rule_out << "\t"<<"\""<<cond.get(Field::id)<<"\"" << "," << "\""<<ss << "\"" << "," << "\"" << cond.get(Field::value) << "\"" << "," << "\"" << cond.get(Field::px) << "\"" << endl;
		}
		last_rule_out << "\tCNS:" << endl;
		for (int j = 0; j < last_GA_ruleset.second.at(tt).size(); j++) {
			auto& gett = last_GA_ruleset.second.at(tt).at(j);
			ss = gett.get(Field::attr);
			if (ss.find("leadTo") < ss.size()) {
				ss = ":leadTo";
			}
			last_rule_out  <<"\t" << "\"" << gett.get(Field::id) << "\"" << "," << "\"" << ss << "\"" << "," << "\"" << gett.get(Field::value) << "\"" << "," << "\"" << gett.get(Field::px) << "\"" << endl;
		}
		last_rule_out << "END:" << endl;
		tt++;
	}
	last_rule_out.close();
	system("cp ./last_GA_ruleset.rule  ./last_GA_ruleset_bak.rule");
	system("cp ./time_consuming.txt  ./time_consuming_bak.txt");
	system("cp ./result  ./result_bak");

}