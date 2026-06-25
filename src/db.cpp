#include"db.h"
#include"ReadLog.h"
MySql::MySql() {
    mysql = mysql_init(nullptr);
    if (!mysql) {
        LOG_FATAL("mysql初始化失败");
    }

    std::cout<<"构造函数读取配置"<<std::endl;
    std::string host = "localhost";
    std::string user = "root";
    std::string password = "";
    std::string database = "readbook";
    int port = 3306;

    std::ifstream config("db.txt");
    if (config.is_open()) {
        std::string line;
        while (std::getline(config, line)) {
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            size_t end = line.find_last_not_of(" \t\r\n");
            line = line.substr(start, end - start + 1);

            // 跳过空行和注释
            if (line.empty() || line[0] == '#') continue;

            // 解析 key=value
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);

            if (key == "host") host = value;
            else if (key == "user") user = value;
            else if (key == "password") password = value;
            else if (key == "database") database = value;
            else if (key == "port") port = std::stoi(value);
        }
    } else {
        LOG_WARN("db.txt 未找到，使用默认配置");
    }

    if (!mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0)) {
        LOG_FATAL("数据库连接失败: " + std::string(mysql_error(mysql)));
    }
    mysql_query(mysql, "SET NAMES utf8mb4");
}
MySql::~MySql(){
    mysql_close(mysql);
}
bool MySql::insert(std::string word){
if(word.empty()){
    //std::cout<<"无插入语句\n";
    LOG_WARN("无插入语句");
    return false;
}
int n=mysql_query(mysql,word.c_str());
if(n==0){
    return true;
}
else{
    //std::cout<<"插入失败"<<mysql_error(mysql)<<std::endl;
    LOG_WARN(mysql_error(mysql));
    return false;
}
}

 std::shared_ptr<MYSQL_RES> MySql::find(std::string word){
    if(word.empty()){
    //std::cout<<"无插入语句\n";
    LOG_WARN("无插入语句");
    return nullptr;
}
int n=mysql_query(mysql,word.c_str());
MYSQL_RES* res=mysql_store_result(mysql);
if(n==0){
    std::shared_ptr<MYSQL_RES>res_(res,[](MYSQL_RES* res){
        mysql_free_result(res);
    });
    return res_;
}
else{
    //std::cout<<"更新失败"<<std::endl;
    LOG_WARN("更新失败");
    return nullptr;
}
}

bool MySql::update(std::string word){
if(word.empty()){
    //std::cout<<"\n";
    LOG_WARN("无插入语句");
    return false;
}
int n=mysql_query(mysql,word.c_str());
if(n==0){
    return true;
}
else{
    LOG_WARN(mysql_error(mysql));
    return false;
}
}

std::string MySql::escape(const std::string& input){
    if(input.empty())return "";
    char* escape=new char[input.size()*2+1];
    mysql_real_escape_string(mysql,escape,input.c_str(),input.size());
    std::string result(escape);
    delete[] escape;
    return result;
}

bool MySql::isAlive(){
    return mysql_ping(mysql)==0;
}

bool MySql::reconnection(){
mysql_close(mysql);
mysql = mysql_init(nullptr);
std::string host = "localhost";
    std::string user = "root";
    std::string password = "";
    std::string database = "readbook";
    int port = 3306;

    std::ifstream config("db.txt");
    if (config.is_open()) {
        std::cout<<"读取文件配置"<<std::endl;
        std::string line;
        while (std::getline(config, line)) {
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            size_t end = line.find_last_not_of(" \t\r\n");
            line = line.substr(start, end - start + 1);

           
            if (line.empty() || line[0] == '#') continue;

          
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);

            if (key == "host") host = value;
            else if (key == "user") user = value;
            else if (key == "password") password = value;
            else if (key == "database") database = value;
            else if (key == "port") port = std::stoi(value);
        
        }
    } else {
        LOG_WARN("db.txt 未找到，使用默认配置");
    }
    std::cout<<"开始连接"<<std::endl;
    if (!mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0)) {
        LOG_ERROR("数据库连接失败: " + std::string(mysql_error(mysql)));
        return false;
    }
    std::cout<<"成功：连接"<<std::endl;
    mysql_query(mysql, "SET NAMES utf8mb4");
    return true;
}