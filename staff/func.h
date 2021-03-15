/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：func.h
*   创 建 者：cx
*   创建日期：2021年03月11日
*   描    述：小菜鸟
*
================================================================*/
#ifndef FUNC_H__
#define FUNC_H__

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sqlite3.h>
#include <errno.h> 
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <signal.h>

//定义协议
#define USER_LOGIN 0 //用户注册
#define ROOT_LOGIN 1 //管理员注册
#define SUCCESS 2 //注册成功
#define USER_EXIT 3 //用户已存在
#define ROOT_EXIT 4 //管理员已被注册


//登陆协议
#define ID_LOGIN 5 //用户登录
#define LOGIN_SUCCESS 6 //用户登陆成功
#define USER_FAIL 7 //用户已登陆
#define PSWD_FAIL 8 //用户名或密码错误
#define USER_UNFIND 9 //用户不存在


//查看信息协议
#define USER_FIND 10 //普通用户查看
#define USER_FIND_FAIL 11 //没有权限

#define ROOT_ADD 12 //增加管理员
#define ROOT_ADD_SUCCESS 13 //增加成功
#define ADD_FAIL 14  //增加失败

#define ROOT_DEL 15 //删除管理员
#define ROOT_DEL_SUCCESS 16 //删除成功
#define ROOT_DEL_FAIL 17 //删除失败

#define ROOT_CHANGE 18 //管理员修改
#define CHANGE_SUCCESS 19 //修改成功
#define CHANGE_FAIL 20 //修改失败
#define CHANGE_NO 21 //没有权限

#define ROOT_FIND 22 //管理员查看
#define ALL 27
#define FIND_SUCCESS 23 //查看成功
#define FIND_FAIL 24 //查看失败

//退出登陆
#define EXIT 25 //退出登陆
#define EXIT_SUCCESS1  26 //退出成功

//网络地址信息（端口和IP）
#define PORT 2222
#define IP "0"

//客户端使用
#define LOGIN 1
#define ENTER 2
#define EXITOUT 3

#define LOGIN_1 1
#define EXITOUT_1 2

#define ENTER_1 1
#define EXITOUT_2 2


typedef struct info{
	char Name[20];
	char WorkId[5];
	char Sex;
	char Phone[20];
	float Salary;
}INfo;
INfo INFO;


typedef struct INFOPROCOTOL{
	int Type;
	char Id[20];
	char Password[20];
	struct info INFO;
	char Res[20];   //备用
}__attribute__((packed)) InfoP;

sqlite3* db = NULL;

struct sockaddr_in Recv;//接收
struct sockaddr_in Send;//发送

struct epoll_event events; //epoll监控的事件
struct epoll_event revent[20];//epoll监控返回的事件

int num = 1;
char cmdbuf[256]="";
int ret = 0;
int i = 0;
int fd = 0;
char **pazRes = NULL;  //查询数据库返回的指针
int Colum;
int Row;
char *Errmsg = NULL;
char *errmsg = NULL;
char ID[20] = "";
int j = 0;
int k = 0;
int l = 0;

/*****************************************************/

//服务器函数
//导入账号信息表
int Import(sqlite3* db);

//导入员工信息表
int ImportInfo(sqlite3* db);

//打印客户端连接信息
void printf_info(struct sockaddr_in Recv);

//用户退出客户端
int exit_(int epfd, struct epoll_event* events, int fd, InfoP Recvmsg, sqlite3 *db);

//普通用户注册
int UserLogin(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd);

//管理员用户注册
int RootLogin(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd);

//用户登录
int IdLogin(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd);

//添加信息
void AddMsg(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd);

//删除信息
void DelMsg(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd);

//修改信息
void ChangeMsg(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd, int flag);

//查看信息
void LookMsg(sqlite3* db, InfoP Recvmsg, InfoP Sendmsg, int fd, int flag);

//发送信息
InfoP SendAll(char** pazRes, InfoP Sendmsg, int k);

//用户退出登录
void exit_out(sqlite3* db, InfoP Recvmsg);

/**********************************************************************/

//客户端注册函数
int CliLogin(int cfd, InfoP Sendmsg, InfoP Recvmsg);

//客户端登录函数
int CliEnter(int cfd, InfoP Sendmsg, InfoP Recvmsg);

//客户端界面
int CliInter(int cfd, InfoP Sendmsg, InfoP Recvmsg);

//查看信息
void Findmsg(int cfd, InfoP Sendmsg, InfoP Recvmsg);

//修改信息
void Changeinfo(int cfd, InfoP Sendmsg, InfoP Recvmsg);

#endif //FUNC_H__

