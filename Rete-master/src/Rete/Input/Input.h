#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "../ReteNet/Net.h"
#include "../TestNode/TestAtTokenFilterNode.h"
#include "../../Log.h"
#include "../../StringOp.h"
#include "../Utils/VectorPrinter.h"
#include "../../PublicDefine.h"
using namespace std;

class ReadFile {

private:
	fstream fin;


public:
	ReadFile() {};
	~ReadFile();
	pair<vector<ConditionVector>, vector<ConditionVector>>readRulers(string rulerPath);
};


class ParseTTL {

private:
	string pathsource,pathdes;
	

public:
	fstream fin, fout;
	ParseTTL(string pathsource,string pathdes);
	~ParseTTL();
	vector<vector<string>> parseTTLFile();


};