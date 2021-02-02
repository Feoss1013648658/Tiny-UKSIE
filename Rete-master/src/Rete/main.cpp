#include <iostream>
#include <ctime> 
#include "ReteNet/Net.h"
#include "TestNode/TestAtTokenFilterNode.h"
#include "../Log.h"
#include "../StringOp.h"
#include "Utils/VectorPrinter.h"
#include "../PublicDefine.h"
#include <ctime>
#include "../Rete/Input/Input.h"
#include "../Rete/GAalgorithem/GA.h"
#include <fstream>
#include "./main.h"
#include "../Rete/GAalgorithem/GA.h"
//#include "IO.h"
#include <unistd.h>
using namespace std;


#define TRUE 1
#define FALSE 0

void   Delay1(int time)//time*1000Ϊ���� 
{
	clock_t   now = clock();
	while (clock() - now < time);
}




double test(pair<vector<ConditionVector>, vector<ConditionVector>>& rulers,
	string srcTTL,string desTTL,string resultFile) {

	//net.addProduction()��˳���������Ĺ���˳���Ŵ��㷨������˳��������˳��
	Net net;
	clock_t startTime, endTime;
	/*result ���*/
	fstream fresult(resultFile,ios_base::out|ios_base::trunc);

	/* �����ļ���ȡ  */
/*	ReadFile readRuler(rulerFile);
	auto&& allConditions = readRuler.readRulers();
	int i = 0;
	// GA �Ż�RETE����
	GA exGA;
	auto&& sortedRulers = exGA.sort_ruler(allConditions); */
	//debug
/*	int tt = 0;
    //	�������������ļ�
	for (auto& ruler : sortedRulers.first) {
		fout << "PRS:" << endl;
		for (auto& cond : ruler) {
			fout << cond.get(Field::id) << "," << cond.get(Field::attr) << "," << cond.get(Field::value) << "," << cond.get(Field::px) << endl;
		}
		fout << "CNS:" << endl;
		for (int j = 0; j < sortedRulers.second.at(tt).size(); j++) {
			auto& gett = sortedRulers.second.at(tt).at(j);
			fout << gett.get(Field::id) << "," << gett.get(Field::attr) << "," << gett.get(Field::value) << "," << gett.get(Field::px) << endl;
		}
		tt++;
	}*/
/*	//���δ��������ļ�
	tt = 0;
	for (auto& ruler : allConditions.first) {
		fout2 << "PRS:" << endl;
		for (auto& cond : ruler) {
			fout2 << cond.get(Field::id) << "," << cond.get(Field::attr) << "," << cond.get(Field::value) << "," << cond.get(Field::px) << endl;
		}
		fout2 << "CNS:" << endl;
		for (int j = 0; j < allConditions.second.at(tt).size(); j++) {
			auto& gett = allConditions.second.at(tt).at(j);
			fout2 << gett.get(Field::id) << "," << gett.get(Field::attr) << "," << gett.get(Field::value) << "," << gett.get(Field::px) << endl;
		}
		tt++;
	}
	fout.close();
	fout2.close(); */
	int i = 0;
	for (auto& ruler : rulers.first) {
		net.addProduction(ruler, rulers.second.at(i++));
	}

/*	else {       //�����Ĺ���˳��
		for (auto& ruler : allConditions.first) {
			net.addProduction(ruler, allConditions.second.at(i++)
			);
		}
	}*/
	/* ���ݱ�ע��֪ʶ�⹹��*/
/*	//��� �������TTL�ļ�
	fstream foutttl("foutttl",ios_base::out);
	if (!foutttl.is_open()) cerr << " open foutttl err" << endl; */
	ParseTTL parseTTL(srcTTL, desTTL);
	int num = 0;
	startTime = clock();//��ʱ��ʼ
	while (num <10) {
		auto&& wmes = parseTTL.parseTTLFile();
/*		for (int i = 0; i < wmes.size(); i++) {
			for (auto& ele : wmes.at(i))
				foutttl << ele << ",";
			foutttl << endl;
		}
		foutttl.close();*/
		for (auto& wme : wmes) {
			net.addWME({ wme.at(0), wme.at(1), wme.at(2), wme.at(3) });
		}
		//���򼤻�
		auto && sb = cout.rdbuf(fresult.rdbuf());  //�ض��������
		auto&& ret = net.invoke();
		for (auto&& condVec : ret) {
			for (auto&& cond : condVec) {
				cond.print(0);
			}
			cout << endl;
		}
		cout << "finish once\n" << endl;
		cout.rdbuf(sb);    //ȡ���ض���
		net.clearStatus();
		cout << "wait next inference" << endl;
		// ���������ļ�������
//		parseTTL.fin.clear();
//		parseTTL.fout.clear();
//		parseTTL.fin.seekg(0, ios::beg);
//		parseTTL.fout.seekp(0,ios::beg);
//		Delay1(1000);
		num++;
	}

	fresult.close();
	endTime = clock();//��ʱ����
	double consum_time = ((endTime - startTime)+0.001) / CLOCKS_PER_SEC;
//	cout << "The run time is:" << consum_time << "s" << endl;

	return consum_time;
}

int main() {

	string rulerFile = "./last_GA_ruleset_bak.rule";
//	string rulerFile = "./myruler2.ruler";
	string time_consum_file = "./time_consuming.txt";
	string srcTTL = "./smartFactory.ttl";
	string desTTL = "./smartFactory_Des.ttl";
	string resultFile = "./result";
	fstream rule_file;
	GA exeGA(time_consum_file);
	bool is_need_GA = false;
	if (-1 == access(rulerFile.c_str(),0)) {
		cout << "no GA_rule_file" << endl;
		is_need_GA = true;
	}
	else {
		rule_file.open(rulerFile,ios_base::in);
		char c;
		rule_file >> c;
		if (rule_file.eof())
		{
			is_need_GA = true;
		}
		is_need_GA = false;
		cout << "exits GA_rule_file" << endl;
	}
	if (is_need_GA) {	 // ��Ҫʹ��GA�������Ź���
		exeGA.GAalgorithm();
	}
	else {    //ֱ������
		ReadFile read;
		auto&& allConditions = read.readRulers(rulerFile);
		auto&& sortedRulers = exeGA.sort_ruler(allConditions);
		double unorded_time = test(sortedRulers, srcTTL, desTTL, resultFile);
	}
	return 0;
}
