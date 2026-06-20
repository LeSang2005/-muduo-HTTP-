#pragma once
#include"mysql/mysql.h"
#include<string>
#include<iostream>
#include<memory>
#include"Timestamp.h"
class MySql{
public:
MySql();
~MySql();
bool insert(std::string word);
std::shared_ptr<MYSQL_RES> find(std::string word);
bool update(std::string word);
void settime(){time.setnow();}
long long gettime(){return time.alive();}
MYSQL* outinit(){return mysql;}
std::string escape(const std::string& input);
private:
MySql(const MySql&) = delete;
MySql& operator=(const MySql&) = delete;
MYSQL* mysql;
Timestamp time;
};