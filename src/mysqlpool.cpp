#include"mysqlpool.h"
#include"ReadLog.h"
std::atomic_bool mysql_pool=false;
mysqlPool& mysqlPool::instance(){
static mysqlPool pool_;
return pool_;
}
mysqlPool::mysqlPool(){
    maxnum_=1024;
    minnum_=4;
    num_=4;
    for(int i=0;i<num_;i++){
        MySql* mysql=new MySql();
        mysql->settime();
        q.push(mysql);
    }
    std::thread t1(&mysqlPool::destory,this);
    t1.detach();
}

std::shared_ptr<MySql> mysqlPool::pop(){
    MySql* front_mysql;
    {
        std::unique_lock<std::mutex>lock(mtx_);
        if(q.empty()){
        if(num_<maxnum_){
            MySql* mysql=new MySql();
            mysql->settime();
            q.push(mysql);
            num_++;
        }
        else{
            //std::cout<<"以及到达上限"<<std::endl;
            LOG_WARN("连接池已经到达上限");
            return nullptr;
        }
    }
     front_mysql=q.front();
     q.pop();
    }
    

    if(!front_mysql->isAlive()){
            if( front_mysql->reconnection()){
            {
                std::unique_lock<std::mutex>lock(mtx_);
                q.push(front_mysql);
            }
            return pop();
        }
        else{
            delete front_mysql;

        {
            std::lock_guard<std::mutex> lock(mtx_);
            num_--;
        }

        return nullptr;
        }
    }

    // if (!front_mysql->isAlive()) {
    //         delete front_mysql;
    //         {
    //             std::unique_lock<std::mutex> lock(mtx_);
    //             std::cout<<"数据库连接为空\n";
    //             num_--;
    //         }
    //         //std::this_thread::sleep_for(std::chrono::milliseconds(500));           
    //     }
    return std::shared_ptr<MySql>(front_mysql,[this](MySql* q){
        if(mysql_pool){
            delete q;
            q=nullptr;
        }
        else{
        q->settime();
        {
            std::unique_lock<std::mutex>lock(this->mtx_);
            this->q.push(q);
        }
        this->cond_.notify_one();
        }
    });
} 

void mysqlPool::destory(){
    while(1){
        std::unique_lock<std::mutex>lock(mtx_);
        while(num_<=minnum_&&!mysql_pool){
            cond_.wait(lock);
        }
        if (mysql_pool) break;
        if(!q.empty()&&q.front()->gettime()>=300){
            delete q.front();
            q.front()=nullptr;
            q.pop();
                num_--;
        }
    }
}
mysqlPool::~mysqlPool(){
    mysql_pool=true;
    cond_.notify_all();        
    std::lock_guard<std::mutex> lock(mtx_);
    while(!q.empty()){
         delete q.front();
            q.front()=nullptr;
            q.pop();
            num_--;
    }
}