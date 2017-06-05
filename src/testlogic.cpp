#include <iostream>
#include "logic.h"

using namespace std;

int main(void)
{
	/*
	char sql[INFO_MAX_LEN] ={"\0"};
	GetSql(sql, SQL_INSERT, "1234565", "wangbindeshebei", 1234567890, 1);

	cout << sql << endl;
	*/

	/*
	struct sensor_s info = {
		.devId = "",
		.info= "",
		.ctime = 1234567,
		.flag = 1,
	};

	SplitString("wangbin|hahahahah", &info, "|");
	cout << info.devId << ", " << info.info << ", "<<info.ctime <<", "<< info.flag <<endl;
	*/

	//char str[] = "54422|hahahahah";
	ProcessData("wangbin|hahahahah");
	return 0;
}