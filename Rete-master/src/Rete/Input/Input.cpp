#include "Input.h"
#include <sstream>
#include <unordered_map>


/*ReadFile::ReadFile() {
   
}*/

ReadFile::~ReadFile() {

}
pair<vector<ConditionVector>, vector<ConditionVector>> ReadFile:: readRulers(string rulerPath) {
	
	fin.open(rulerPath, ios_base::in | ios_base::out);
	auto ret = fin.is_open();
	if (!(ret))  cerr << __FILE__ << __LINE__ << "open err" << endl;
	else cout << "open " << rulerPath << " sucessfully" << endl;

	stringstream ss;
	ConditionVector conditions, getter;
	vector<ConditionVector> allAonditions, allGetter;
	while (1) {
		bool isPRS = true;
		string line;
		getline(fin, line);
		if (line.find("START") > line.size()) cerr << __FILE__ << __LINE__ << " read ruler err" << endl;
		getline(fin, line);
		if (line.find("PRS") > line.size()) cerr << __FILE__ << __LINE__ << " read ruler err" << endl;
		while (1) {
			string  ele[4];
			getline(fin, line);
			if (line.find("END") < line.size()) break;
			else if (line.find("CNS") < line.size()) {
				isPRS = false;
				getline(fin,line);
			}
			line = CStringOp::trim(line, "\t");
			if (line.size() == 0) 
				cerr << __FILE__ << __LINE__ << " no condition" << endl;
			for (auto& ch : line) {
				if (ch == '\"' || ch == ',') ch = ' ';
				else if (ch == ' ') ch = '_';
			}
			ss << line;
			ss >> ele[0] >> ele[1] >> ele[2] >> ele[3];
			if (isPRS)
				conditions.push_back({ ele[0],ele[1],ele[2],ele[3] });
			else
				getter.push_back({ ele[0],ele[1],ele[2],ele[3] });
			ss.str(std::string());
			ss.clear();
		}
		allAonditions.push_back(conditions);
		allGetter.push_back(getter);
		conditions.clear();
		getter.clear();

		if (fin.eof()) break;
		if (fin.peek() == EOF) break;
	}
//	cout << "  " << endl;  LC
	fin.close();
	return make_pair(allAonditions, allGetter);
}


ParseTTL::ParseTTL(string pathsource,string pathdes) {
	this->pathsource = pathsource;
	this->pathdes = pathdes;
	

}
ParseTTL::~ParseTTL() {

}

int retNotation(string& s) {
	if (s.at(0) == '"') return 0;
	if (s.size() == 0)
		return -1;
	if (s.at(0) != '<') {
		s = s.substr( s.find(':')+1);
		return 0;
	}
	else {
		s = s.substr(s.find('#')+1);
		s.erase(s.end()-1);
		return 0;
	}
	return -1;
}
vector<vector<string>> ParseTTL::parseTTLFile() {

	fin.open(this->pathsource, ios_base::in | ios_base::out);
	auto ret = fin.is_open();
	if (!(ret))  cerr << __FILE__ << __LINE__ << "open ttl err" << endl;
//	else cout << "open " << this->pathsource << " ttl open sucessfully" << endl;

	fout.open(this->pathdes, ios_base::in | ios_base::out);
	ret = fout.is_open();
	if (!(ret))  cerr << __FILE__ << __LINE__ << "open desttl err" << endl;
//	else cout << "open " << this->pathdes << " desttl open sucessfully" << endl;

	string in;
	vector<vector<string>> wmes;
	string sub, pre, obj,pb;
	stringstream ss;
	unordered_map<string, int> hmap;
//	unordered_map<string, int> hasPmap;   //add  1.26  想对hasProbabilityValue插入序号，但是行不通，因为wmes中的leadto 和hasP的先后顺序不定
	while (getline(fin, in)) {
		if (in.size()>0 && in.at(0) == '.')
			break;
	}
	getline(fin,in);
	do {
		ss << in;
		ss >> sub;
		retNotation(sub);

		while (1) {
			ss.str(std::string());
			ss.clear();
			getline(fin, in);
			if (in.at(0) == '.') break;
			ss << in;
			ss >> pre >> obj;
			if (pre.find("label") < pre.size()) {
				ss.str(std::string());
				ss.clear();
				continue;
			} 
			retNotation(pre);
			pre.insert(pre.begin(),':');
			retNotation(obj);
			if (pre.find("leadTo") < pre.size()) {
				if (hmap.find(obj) != hmap.end()) {
					++hmap[obj];
				}
				else {
					hmap.insert(make_pair(obj,1));
				}
				pre.append(to_string(hmap[obj]));
//				hasPmap.insert(make_pair(sub,hmap[obj]));
			}
			if (pre.find("hasProbabilityValueOf") < pre.size()) {
				obj = obj.substr(1,obj.size()-2);
				pb = obj;
//				pre.append(to_string(hasPmap[sub]));   //add 1.26  加这个会出问题，因为不知道leadto和hasP出现的先后
			}
			else if (pre.find("leadTo") < pre.size()) {
				pb = "0.01";                     //  这里需要根据概率表设置概率值
			}
			else if(pre.find("isHappened") < pre.size()){
				//obj 为 bool量时，这里可以处理下
				pb = "1";
			}
			else {
				pb = "1";
			}

			wmes.push_back({ sub,pre,obj,pb });
//			fout << sub << "  " << pre << "  " << obj <<"  "<<pb<< endl;
		}
//		fout << endl;
	} while(getline(fin, in));

	if (fin.is_open())
		fin.close();
	if (fout.is_open())
		fout.close();

	return wmes;
};
