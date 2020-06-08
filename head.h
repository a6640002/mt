#pragma once
#include "sql.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/select.h>
#include <pthread.h>
#include <strings.h>
#include <signal.h>
#include "s.h"
#define FUNC_OK 0
#define FUNC_ERR 1

#define DEBUG_MSG(x)                                    \
    printf("[%s][%d][DEBUG]:", __FUNCTION__, __LINE__); \
    printf(x)
#define INFO_MSG(x)                                     \
    printf("[%s][%d][INFO.]:", __FUNCTION__, __LINE__); \
    printf(x)
#define ERROR_MSG(x)                                    \
    printf("[%s][%d][ERROR]:", __FUNCTION__, __LINE__); \
    printf(x)

#define Malloc(type, num) (type *)malloc(sizeof(type) * (num))
#define IP "192.168.2.142"
#define PORT 8888
typedef struct cus_info cus_info;
typedef struct order_info order_info;
typedef struct menu menu;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
typedef struct comm comm;
//初始化socket
void init_sockaddr(sockaddr_in *addr);

//商家登录编号 42
t_s32int rest_longon_server(t_s32int conn, char *name);

//商家修改菜单  43
t_s32int update_menu_by_rst(t_s32int conn, char *name);
// 商家注册编号41
t_s32int rest_reg_server(t_s32int conn);

//商家修改自己所有的订单 44
t_s32int get_order_info_by_name_by_rst_server(t_s32int conn, char *rst);
//商家修改评论 45
t_s32int get_order_info_by_name_and_id_by_rst_server(t_s32int conn, char *rst);

//用户登录编号 6
t_s32int cus_longon_server(t_s32int conn, char *name);
//用户获取自己的订单 编号1
t_s32int get_order_by_cus_name_by_cus_server(t_s32int conn);
//用户获取商家的菜单列表 编号2
t_s32int get_menu_by_name_by_cus_server(t_s32int conn);
//用户获取商家列表 编号4
t_s32int get_rst_by_cus_server(t_s32int conn);
//用户取消订单 5
t_s32int cus_del_order(t_s32int conn);
// 用户注册编号7
t_s32int cus_reg_server(t_s32int conn);
// 用户下单编号8
t_s32int order_by_cus(t_s32int conn);
// 用户修改订单 9 10

t_s32int modify_order_by_cus(t_s32int conn);
//判断是否属于监听线程
t_s32int if_listen_socket(void *arg, t_s32int *i);
//信号捕获函数 发信息给所有客户端
void signal_handle(int sig);

//发送函数
t_s32int send_mes_by_thread(char *name, char *mess);
//监听线程

void *thread_fun_listen(void *arg);
//分线程
void *thread_fun_rest(void *arg);
