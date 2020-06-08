
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

typedef struct order_info order_info;
typedef signed int t_s32int;
typedef signed char t_s8int;
typedef float t_s64float;
typedef const char t_c8int;
typedef t_s32int (*callback)(void *, t_s32int, char **, char **);
typedef struct cus_info cus_info;
typedef struct order_info order_info;
typedef struct menu menu;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;
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
struct order_info
{
    t_s8int id[30];           //订单号
    t_s8int restaurant[20];   //餐厅名字
    t_s8int customer[20];     //客户名字
    t_s8int food[10][20];     //点菜信息，最多10道
    t_s32int price[10];       //每道菜的价格
    t_s32int price_total;     //订单总价格
    t_s8int cus_addr[30];     //客户地址
    t_s8int book_time[30];    //点菜时间
    t_s8int remark[128];      //备注
    t_s8int phone_num[12];    //手机号
    t_s8int cust_review[128]; //用户评价
    t_s8int res_review[128];  //商家评价
    t_s32int flag;            //完成操作的函数
};
struct fun_arg
{
    t_s8int buf[1024];
    t_s32int conn_fd;
};

//初始化socket
void init_sockaddr(sockaddr_in *addr)
{
    bzero(addr, sizeof(sockaddr_in));
    addr->sin_addr.s_addr = inet_addr(IP);
    addr->sin_family = PF_INET;
    addr->sin_port = htons(PORT);
}
void *thread_fun_rest(void *arg)
{
    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn < 0)
    {
        perror("conn");
        return NULL;
    }
    sockaddr_in serv;
    init_sockaddr(&serv);

    if (0 > connect(conn, (sockaddr *)&serv, sizeof(sockaddr_in)))
    {
        perror("conn");
        return NULL;
    }
    int i = 100;
    write(conn, &i, 4);
    //char buf[20];
    write(conn, "cc", 2);
    printf(" pthread start list ..\n");
    while (1)
    {
        printf("thread read...\n");
        order_info order;
        char buf[100];
        bzero(buf, 100);
        bzero(&order, sizeof(order));
        // sleep(1);
        // read(conn, &order, sizeof(order));
        read(conn, buf, 100);
        printf("recv from message %s \n", buf);
        //  printf("recv from message %d \n", order.price_total);
    }
}

int main(void)
{

    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn < 0)
    {
        perror("conn");
        return -1;
    }
    sockaddr_in serv;
    init_sockaddr(&serv);

    if (0 > connect(conn, (sockaddr *)&serv, sizeof(sockaddr_in)))
    {
        perror("conn");
        return -1;
    }
    char buf[20];
    bzero(buf, 20);
    char buf1[20];
    bzero(buf1, 20);
    //write(conn, "cc", sizeof("cc"));
    pthread_t rec;
    pthread_create(&rec, 0, thread_fun_rest, &conn);
    while (1)
    {

        fflush(0);
        read(conn, buf, 20);
        printf("%s \n", buf);
    }
}
