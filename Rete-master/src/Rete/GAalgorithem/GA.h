#pragma once
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "../ReteNet/Net.h"
#include "../TestNode/TestAtTokenFilterNode.h"
#include "../../Log.h"
#include "../../StringOp.h"
#include "../Utils/VectorPrinter.h"
#include "../../PublicDefine.h"
#include "../Input/Input.h"
#include "../main.h"
#include "../data_cal.h"
using namespace std;
//  对规则的谓语排序排序

class GA {


public:
	int max_iterations,cur_iterations;
	double time_factor;
	double src_time;
	ReadFile* readRuler;
	fstream time_file;
	pair<vector<ConditionVector>, vector<ConditionVector>> sort_ruler(pair<vector<ConditionVector>,vector<ConditionVector>>& condvecs);
	GA(string time_file_path);
	~GA();
	int evaluation(vector<vector<pair<int, string>>>& population,
		vector<pair<vector<ConditionVector>, vector<ConditionVector>>>& sorted_rulesset,
		string srcTTL,
		string desTTL,
		string resultFile,
		vector<double>& ret_evaluation_values
	);
	void GAalgorithm();
//	pair<vector<ConditionVector>, vector<ConditionVector>> readRulers(string rulerFile);  /文件读取用Input类中的函数


};
// GA