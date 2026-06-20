#pragma once
#include"db.h"
#include<queue>
#include"condition_variable"
#include"mutex"
#include"memory"
#include<thread>
#include<atomic>
class mysqlPool{
public:
static mysqlPool& instance();
std::shared_ptr<MySql> pop();
~mysqlPool();
private:

mysqlPool();
mysqlPool(mysqlPool &p)=delete;
mysqlPool(mysqlPool &&p)=delete;
mysqlPool& operator=(mysqlPool& p)=delete;
int num_;
int maxnum_;
int minnum_;
std::queue<MySql*>q;
std::mutex mtx_;
std::condition_variable cond_;
void destory();
};