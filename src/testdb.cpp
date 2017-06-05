#include<iostream>  
#include "DB.h"  
using namespace std;  
  
int main()  
{  
    DB::get_instance()->InitDB("localhost", "root", "w1501226", "IOT_cloud");  
    DB::get_instance()->Select("select * from test");
    return 0;
}