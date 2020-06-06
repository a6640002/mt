#pragma once
#include "sql.c"
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
#include "head.h"
typedef signed int t_s32int;
typedef signed char t_s8int;
typedef float t_s64float;
typedef const char t_c8int;
typedef t_s32int (*callback)(void *, t_s32int, char **, char **);
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

struct comm
{
    t_s32int socket;
    t_s8int name[20];
};
comm comm_data[500];    //套接字信息结构体
order_info order;       //待发送订单结构体
int order_type_flag;    //订单类型 商家 0还是客户1
char name_to_send[20];  //待发订单的人名字
int max = 1024;         //最大socket数
pthread_t conn_ph;      //监听线程id
char comm_message[100]; //发送的信息
struct fun_arg          //线程函数
{
    t_s8int buf[1024];
    t_s32int conn_fd;
};
typedef struct fun_arg fun_arg;
//初始化socket
void init_sockaddr(sockaddr_in *addr)
{
    bzero(addr, sizeof(addr));
    addr->sin_addr.s_addr = inet_addr(IP);
    addr->sin_family = PF_INET;
    addr->sin_port = htons(PORT);
}
//thread函数

//商家登录编号 42
t_s32int rest_longon_server(t_s32int conn, char *name)
{
    menu buf_tmp, buf_recv;
    bzero(&buf_tmp, sizeof(menu));
    if (0 > read(conn, &buf_recv, sizeof(menu)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }

    if (get_rst_sigal(buf_recv.name, &buf_tmp) == FUNC_OK)
    {
        // printf(" find name %s  \n", buf_recv.name);

        if (strncmp(buf_recv.pass, buf_tmp.pass, sizeof(buf_recv.pass)) == 0)
        {
            write(conn, "success", sizeof("success"));
            strncpy(name, buf_recv.name, sizeof(name));
            //scanf("%s", name);
            return FUNC_OK;
        }
        write(conn, "failed", sizeof("failed"));
        INFO_MSG("passcode error\n");
        return FUNC_ERR;
    }
    else
    {
        INFO_MSG(" cant find name\n");
        printf(" %s logoin failed\n", buf_recv.name);
        write(conn, "failed", sizeof("failed"));
        return FUNC_ERR;
    }
}

//商家修改菜单  43
t_s32int update_menu_by_rst(t_s32int conn, char *name)
{
    t_s32int size;
    menu *get = NULL;
    menu buf;
    bzero(&buf, sizeof(buf));
    get = get_menu_by_name_res(name, &size);
    if (get == NULL)
    {
        INFO_MSG("cant find menu list is empty \n");
        int i = 0;
        write(conn, &i, sizeof(int));
        menu find;
        get_rst_sigal(name, &find);
        write(conn, &find, sizeof(menu));
        int get_size;
        menu get_buf;
        read(conn, &get_size, 4);
        for (int i = 0; i < get_size; i++)
        {
            read(conn, &get_buf, sizeof(get_buf));
            //   printf("%s \n", get_buf.food);
            insert_menu(get_buf);
        }

        free(get);
        return FUNC_OK;
    }

    t_s32int get_size;
    write(conn, &size, 4);
    //printf("get menus ;write size %d line 144 \n", size);
    for (int i = 0; i < size; i++)
    {
        write(conn, get + i, sizeof(menu));
    }
    // printf(" write ok  152\n");

    read(conn, &get_size, 4);

    // printf("get_size %d \n", get_size);
    if (menu_del_by_rest_name(name) == FUNC_ERR)
    {
        INFO_MSG("del menu error flag 43\n");
    };
    for (int i = 0; i < get_size; i++)
    {

        read(conn, &buf, sizeof(buf));
        //  printf("get name %s\n", buf.food);
        insert_menu(buf);
    }

    return FUNC_OK;
}
// 商家注册编号41
t_s32int rest_reg_server(t_s32int conn)
{
    menu buf_tmp, buf_recv;
    bzero(&buf_tmp, sizeof(menu));
    bzero(&buf_recv, sizeof(menu));
    if (0 > read(conn, &buf_recv, sizeof(menu)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }

    printf("flag %d \n", buf_recv.flag);
    if (get_rst_sigal(buf_recv.name, &buf_tmp) == FUNC_OK)
    {
        // printf(" find name %s dont insert \n", buf_recv.name);

        write(conn, "failed", 6);
        return FUNC_ERR;
    }
    else
    {
        //  printf(" cant find name %s insert into\n", buf_recv.name);
        insert_res(buf_recv);
        printf("insert ok");
        write(conn, "success", 7);
        return FUNC_OK;
    }
}

//商家修改自己所有的订单 44
t_s32int get_order_info_by_name_by_rst_server(t_s32int conn, char *rst)
{
    t_s32int size;
    order_info *get;
    get = get_order_info_by_res_name(rst, &size);
    for (int i = 0; i < size; i++)
    {
        printf("%s ", get[i].restaurant);
    }
    if (get == NULL)
    {
        write(conn, &size, 4);
        INFO_MSG("rst cant find order \n");
        return FUNC_ERR;
    }

    write(conn, &size, 4);
    for (int i = 0; i < size; i++)
    {
        write(conn, get + i, sizeof(order_info));
    }

    t_s32int get_size;
    order_info buf;
    // read(conn, &get_size, 4);
    order_del_by_rst(rst);
    // printf("%s \n ", rst);
    // printf("size %d", size);
    for (int i = 0; i < size; i++)
    {
        read(conn, &buf, sizeof(order_info));
        if (buf.flag == 3)
        {
            send_mes_by_thread(buf.customer, "you have message to op\n");
        }
        //  printf("get food %s \n", buf.food[0]);
        insert_order(buf);
    }

    free(get);
    return FUNC_OK;
}

//商家修改评论 45
t_s32int get_order_info_by_name_and_id_by_rst_server(t_s32int conn, char *rst)
{
    t_s32int size;
    order_info *get;
    get = get_order_info_by_res_name(rst, &size);
    for (int i = 0; i < size; i++)
    {
        printf("%s ", get[i].restaurant);
    }
    if (get == NULL)
    {
        write(conn, &size, 4);
        INFO_MSG("rst cant find order \n");
        return FUNC_ERR;
    }

    write(conn, &size, 4);
    for (int i = 0; i < size; i++)
    {
        write(conn, get + i, sizeof(order_info));
    }

    t_s32int get_size;
    order_info buf;

    order_del_by_rst(rst);

    for (int i = 0; i < size; i++)
    {
        read(conn, &buf, sizeof(order_info));
        insert_order(buf);
    }

    free(get);
    return FUNC_OK;
}

//用户接口

//用户登录编号 6
t_s32int cus_longon_server(t_s32int conn, char *name)
{
    cus_info buf_tmp, buf_recv;
    bzero(&buf_tmp, sizeof(cus_info));
    if (0 > read(conn, &buf_recv, sizeof(cus_info)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }

    if (get_cus_info_by_name_sigal(buf_recv.name, &buf_tmp) == FUNC_OK)
    {
        //   printf(" find name %s  \n", buf_recv.name);
        //  printf("pass %s \n", buf_recv.password);

        if (strncmp(buf_recv.password, buf_tmp.password, sizeof(buf_recv.password)) == 0)
        {
            write(conn, "success", sizeof("success"));
            fflush(0);
            strncpy(name, buf_recv.name, sizeof(name));
            INFO_MSG("logon succcess\n");
            return FUNC_OK;
        }
        write(conn, "failed", sizeof("failed"));
        printf("pass %s \n", buf_tmp.password);
        INFO_MSG("passcode error\n");
        return FUNC_ERR;
    }
    else
    {
        printf(" cant find name %s logoin failed\n", buf_recv.name);
        write(conn, "failed", sizeof("failed"));
        return FUNC_ERR;
    }
}

//用户获取自己的订单 编号1
t_s32int get_order_by_cus_name_by_cus_server(t_s32int conn)
{
    t_s32int size;
    order_info *get;
    char buf[20];
    printf("/////3 \n");
    if (0 > read(conn, buf, sizeof(buf)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }
    get = get_order_info_by_cust_name(buf, &size);
    printf("/////1 \n");
    if (get == NULL)
    {
        INFO_MSG("cant find order \n");
        return FUNC_ERR;
    }
    printf("/////2 \n");
    t_s32int get_size;
    write(conn, &size, 4);
    for (int i = 0; i < size; i++)
    {
        write(conn, get + i, sizeof(order_info));
        for (int j = 0; j < 20000000; j++)
        {
        }
        printf("%s \n", (get + i)->id);
    }
    //free(get);
    return FUNC_OK;
}

//用户获取商家的菜单列表 编号2
t_s32int get_menu_by_name_by_cus_server(t_s32int conn)
{
    t_s32int size;
    menu *get;
    char name_rst[20];

    if (0 > read(conn, name_rst, 20))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }
    get = get_menu_by_name_cus(name_rst, &size);

    if (get == NULL)
    {
        INFO_MSG("cant find order \n");
        return FUNC_ERR;
    }

    t_s32int get_size;
    write(conn, &size, 4);
    for (int i = 0; i < size; i++)
    {
        write(conn, get + i, sizeof(menu));
        printf("%s \n", get[i].name);
    }
    free(get);
    return FUNC_OK;
}

//用户获取商家列表 编号4
t_s32int get_rst_by_cus_server(t_s32int conn)
{
    t_s32int size;
    menu *get;
    get = get_rst_by_cus(&size);
    for (int i = 0; i < size; i++)
    {
        printf("%s ", get[i].food);
    }
    if (get == NULL)
    {
        INFO_MSG("cant find order \n");
        return FUNC_ERR;
    }

    t_s32int get_size;
    write(conn, &size, 4);
    printf("size:%d \n", size);
    for (int i = 0; i < size; i++)
    {
        write(conn, (get + i)->name, sizeof(get->name));
        printf("get[i].name%s\n ", get[i].name);
    }
    // free(get);
    return FUNC_OK;
}

//用户取消订单 5
t_s32int cus_del_order(t_s32int conn)
{

    order_info get;
    if (0 > read(conn, &get, sizeof(order_info)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }

    if (order_del_by_id(get.id) == FUNC_ERR)
    {
        INFO_MSG("del order error \n");
    }
    write(conn, "success", sizeof("success"));
    return FUNC_OK;
}

// 用户注册编号7
t_s32int cus_reg_server(t_s32int conn)
{
    cus_info buf_tmp, buf_recv;
    bzero(&buf_tmp, sizeof(cus_info));
    bzero(&buf_recv, sizeof(cus_info));
    if (0 > read(conn, &buf_recv, sizeof(cus_info)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }

    printf("name %s \n", buf_recv.name);
    printf("pass %s \n", buf_recv.password);
    if (get_cus_info_by_name_sigal(buf_recv.name, &buf_tmp) == FUNC_OK)
    {
        printf(" find name %s dont insert \n", buf_recv.name);
        write(conn, "failed", 7);
        return FUNC_ERR;
    }
    else
    {
        printf(" cant find name %s insert into\n", buf_recv.name);
        insert_cus(buf_recv.name, buf_recv.password, buf_recv.flag);
        printf("insert ok");
        write(conn, "success", 8);
        return FUNC_OK;
    }
}
// 用户下单编号8
t_s32int order_by_cus(t_s32int conn)
{
    order_info order;
    if (0 > read(conn, &order, sizeof(order_info)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }
    if (insert_order(order) == FUNC_ERR)
    {
        INFO_MSG("order error \n");
    }
    write(conn, "success", sizeof("success"));
    return FUNC_OK;
}
// 用户修改订单 9 10

t_s32int modify_order_by_cus(t_s32int conn)
{
    order_info order;
    if (0 > read(conn, &order, sizeof(order_info)))
    {
        INFO_MSG("canf read orders");
        return FUNC_ERR;
    }
    order_del_by_id(order.id);
    printf("new nums%s \n", order.phone_num);
    insert_order(order);
    printf("提醒商家 信息修改(ง •̀_•́)ง！");
    write(conn, "success", 10);
    INFO_MSG("write success\n");
    return FUNC_OK;
}

//判断是否属于监听线程
t_s32int if_listen_socket(void *arg, t_s32int *i)
{
    int flag = 0;
    char name[20];
    fun_arg arg_in_fun = *(fun_arg *)arg;
    read(arg_in_fun.conn_fd, &flag, 4); //读取标志位

    if (flag == 100)
    {
        strncpy(comm_data[max].name, name, 20);
        comm_data[arg_in_fun.conn_fd].socket = arg_in_fun.conn_fd;
        read(arg_in_fun.conn_fd, name, 20); //读取用户名字 且绑定
        printf("===========comm_data[max - 1].name==============\n");
        for (int i = 0; i < max; i++)
        {
            printf("i:%d name:%s socket%d \n", i, comm_data[i].name, comm_data[i].socket);
        }

        printf("=============================================\n");

        printf("listen client connect  \n");
        return FUNC_OK;
    }
    *i = flag;
    return FUNC_ERR;
}

//信号捕获函数 发信息给所有客户端
void signal_handle(int sig)
{
    printf("=========signal handle================\n");
    for (int i = 0; i < max; i++)
    {
        if (strncmp(comm_data[i].name, name_to_send, 20) == 0)
        {
            int ret = write(comm_data[i].socket, comm_message, 100);
            // int ret = write(comm_data[i].socket, &order, sizeof(order));
            if (ret < 0)
            {
                perror("write");
                printf("send to comm_data[i].socket:%d failed\n", comm_data[i].socket);
            }
            else
            {
                INFO_MSG("write success\n");
            }

            // write(comm_data[i].socket, "success", sizeof("success"));
        }

        printf("i:%d comm_data[i].socket:%d \n", i, comm_data[i].socket);
        printf("write socket:%d  max =%d \n", comm_data[i].socket, max);
        printf("send %d \n", order.price_total);
    }
    printf("=========================\n\n");
}

//发送函数
t_s32int send_mes_by_thread(char *name, char *mess)
{
    strncpy(comm_message, mess, 100);
    strncpy(name_to_send, name, 20);
    pthread_kill(conn_ph, SIGALRM);
    printf("send_mes_by_thread success \n");
}

//监听线程

void *thread_fun_listen(void *arg)
{

    while (1)
    {
        conn_ph = pthread_self();
        signal(SIGALRM, signal_handle);
        pause();

        printf("=========================\n");
        printf(" thread%lld start send message \n", (long long)conn_ph);
        printf("=========================\n");
    }
}

//分线程
void *thread_fun_rest(void *arg)
{

    fun_arg arg_in_fun = *(fun_arg *)arg;
    printf("%d \n", sizeof(order_info));

    printf("new thread:%lld,socket:%d,user connect\n", (long long)pthread_self, arg_in_fun.conn_fd);
    //if_listen_socket(arg, &i);

    //send_mes_by_thread("cc", "send message\n");
    while (1)
    {
        t_s8int rst[20], cus[20];
        t_s8int buf[300];
        t_s32int i = 0;
        read(arg_in_fun.conn_fd, &i, 4);
        printf("get  new flag:%d connfd:%d \n", i, arg_in_fun.conn_fd);
        switch (i)
        {
        case 41:
            rest_reg_server(arg_in_fun.conn_fd);
            break;
        case 43:
            update_menu_by_rst(arg_in_fun.conn_fd, rst);

            break;
        case 42:
            rest_longon_server(arg_in_fun.conn_fd, rst);

            break;
        case 44:
            get_order_info_by_name_by_rst_server(arg_in_fun.conn_fd, rst);
            break;
        case 45:
            get_order_info_by_name_and_id_by_rst_server(arg_in_fun.conn_fd, rst);
            break;
        case 88:
            close(arg_in_fun.conn_fd);
            return NULL;

        case 1:

            get_order_by_cus_name_by_cus_server(arg_in_fun.conn_fd);
            break;
        case 2:
            get_menu_by_name_by_cus_server(arg_in_fun.conn_fd);

            break;
        case 4:
            get_rst_by_cus_server(arg_in_fun.conn_fd);

            break;
        case 6:
            cus_longon_server(arg_in_fun.conn_fd, cus);

            break;
        case 7:
            cus_reg_server(arg_in_fun.conn_fd);
            break;
        case 5:
            cus_del_order(arg_in_fun.conn_fd);

            break;
        case 8:
            order_by_cus(arg_in_fun.conn_fd);

            break;
        case 9:
            modify_order_by_cus(arg_in_fun.conn_fd);

            break;
        case 10:
            modify_order_by_cus(arg_in_fun.conn_fd);
            break;
        case 0:
            INFO_MSG("unknow message! ignor\n");
            printf("  flag =%d socket = %d \n", i, arg_in_fun.conn_fd);
            return NULL;
            //  return NULL;
        }
        if (i < 0 || i > 101)
        {
            INFO_MSG("unknow message! ignor");
            printf("  flag =%d socket = %d \n", i, arg_in_fun.conn_fd);
            return NULL;
        }
    }
    close(arg_in_fun.conn_fd);
}