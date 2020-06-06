#pragma once

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
#include <sqlite3.h>
#include "list.h"

typedef signed int t_s32int;
typedef signed char t_s8int;
typedef float t_s64float;
typedef const char t_c8int;
typedef t_s32int (*callback)(void *, t_s32int, char **, char **);
#define init_order_info "create table order_info(id integer,restaurant char, customer char, \
food1 char, food2 char, food3 char, food4 char, food5 char, food6 char, food7 char,\
 food8 char, food9 char, food10 char, price1 integer, price2 integer, price3 integer,\
  price4 integer, price5 integer, price6 integer, price7 integer, price8 integer,\
   price9 integer, price10 integer, price_total integer, cus_addr char ,book_time char, \
   remark char, phone_num char, cust_review char, res_review char, flag integer);"
#define init_menu_info "create table menu(name char,pass char,food char, price integer,falg integer);"
#define init_cus_info "create table cus(name char,pass char,flag integer);"
#define init_res_info "create table res(name char,pass char,food char, price integer,falg integer);"

//客户发送的订单结构体
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

//商家提供的菜单结构体
struct menu
{
    t_s8int name[20]; //账户
    t_s8int pass[20]; //密码
    t_s8int food[20]; //菜名
    t_s32int price;   //价格
    t_s32int flag;    //完成操作的函数
};

//顾客用户信息
struct cus_info
{
    t_s8int name[20];     //姓名
    t_s8int password[20]; //密码
    t_s32int flag;        //完成操作的函数
};

typedef struct cus_info cus_info;
typedef struct menu menu;
typedef struct order_info order_info;
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
//sql命令函数
t_s32int sql(t_s8int *sql, callback GetData)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    t_s32int rc;

    rc = sqlite3_open("t.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return FUNC_ERR;
    }
    rc = sqlite3_exec(db, sql, GetData, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return FUNC_OK;
}
//根据商家获取菜单信息 //单独一个
t_s32int get_menu_by_name_sigal(t_s8int *name, struct menu *user_find)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return FUNC_ERR;
    }
    sprintf(buf, "select * from menu where name='%s';", name);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);

    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things\n");
        return FUNC_ERR;
    }
    strncpy(user_find->name, dbResult[5], sizeof(user_find->name));
    strncpy(user_find->pass, dbResult[6], sizeof(user_find->pass));
    strncpy(user_find->food, dbResult[7], sizeof(user_find->food));

    user_find->price = atoi(dbResult[8]);
    user_find->price = atoi(dbResult[9]);

    sqlite3_free_table(dbResult);
    return FUNC_OK;
}

//获取单独的商家
t_s32int get_rst_sigal(t_s8int *name, struct menu *user_find)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return FUNC_ERR;
    }
    sprintf(buf, "select * from res where name='%s';", name);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);

    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things\n");
        return FUNC_ERR;
    }
    strncpy(user_find->name, dbResult[5], sizeof(user_find->name));
    strncpy(user_find->pass, dbResult[6], sizeof(user_find->pass));
    strncpy(user_find->food, dbResult[7], sizeof(user_find->food));

    user_find->price = atoi(dbResult[8]);
    user_find->price = atoi(dbResult[9]);

    sqlite3_free_table(dbResult);
    return FUNC_OK;
}
//根据名字获取用户信息
t_s32int get_cus_info_by_name_sigal(t_s8int *name, struct cus_info *user_find)
{

    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return -1;
    }
    sprintf(buf, "select * from cus where name='%s';", name);
    //printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things");
        return FUNC_ERR;
    }
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return -1;
    }

    if (SQLITE_OK == ret)

    {
        cus_info data[nRow];
        bzero(data, sizeof(data));
        index = nColumn;

        // for (t_s32int i = 0; i < nRow; i++)

        // {
        //     for (t_s32int j = 0; j < nColumn; j++)

        //     {

        //         //  printf("字段值:%s,%d %d %d\n", dbResult[index], index, nRow, nColumn);
        //         switch (j)
        //         {
        //         case 0:
        //             sprintf(data[i].name, "%s", dbResult[index]);
        //             break;
        //         case 1:
        //             sprintf(data[i].password, "%s", dbResult[index]);
        //             break;
        //         case 2:
        //             data[i].flag = atoi(dbResult[index]);
        //             ;
        //             break;
        //         }

        //         ++index;
        //     }

        //     // printf("------- \n");
        // }
        sprintf(user_find->name, "%s", dbResult[3]);
        //strncpy(user_find->name, dbResult[3], sizeof(user_find->name));
        sprintf(user_find->password, "%s", dbResult[4]);
        user_find->flag = atoi(dbResult[5]);
        // * /
        //     for (int i = 0; i < nRow; i++)
        // {
        //     printf("%s \n", data[i].name);
        // }
    }
    sqlite3_free_table(dbResult);
    return 0;
}
//根据name获取订单
t_s32int get_order_info_by_name_sigal(t_s8int *name, struct order_info *user_find)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return -1;
    }
    sprintf(buf, "select * from order_info where customer='%s';", name);
    printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things");
        return FUNC_ERR;
    }
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return -1;
    }

    /* if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {

                printf("字段值:%s,%d \n", dbResult[index], index);

                ++index;
            }

            printf("------- \n");
        } */
    /*restaurant, customer, food1, food2,
            food3, food4, food5, food6, food7, food8,
            food9, food10, price1, price2, price3, price4,
            price5, price6, price7, price8, price9,
            price10, price_total, cus_addr, book_time, remark,
            phone_num, cust_review, res_review, flag)*/
    // 30-59 下标
    strncpy(user_find->restaurant, dbResult[30], sizeof(user_find->restaurant));
    strncpy(user_find->customer, dbResult[31], sizeof(user_find->customer));
    strncpy(user_find->food[0], dbResult[32], sizeof(user_find->food[0]));
    strncpy(user_find->food[1], dbResult[33], sizeof(user_find->food[1]));
    strncpy(user_find->food[2], dbResult[34], sizeof(user_find->food[2]));
    strncpy(user_find->food[3], dbResult[35], sizeof(user_find->food[3]));
    strncpy(user_find->food[4], dbResult[36], sizeof(user_find->food[4]));
    strncpy(user_find->food[5], dbResult[37], sizeof(user_find->food[5]));
    strncpy(user_find->food[6], dbResult[38], sizeof(user_find->food[6]));
    strncpy(user_find->food[7], dbResult[39], sizeof(user_find->food[7]));
    strncpy(user_find->food[8], dbResult[40], sizeof(user_find->food[8]));
    strncpy(user_find->food[9], dbResult[41], sizeof(user_find->food[9]));
    user_find->price[0] = atoi(dbResult[42]);
    user_find->price[1] = atoi(dbResult[43]);
    user_find->price[2] = atoi(dbResult[44]);
    user_find->price[3] = atoi(dbResult[45]);
    user_find->price[4] = atoi(dbResult[46]);
    user_find->price[5] = atoi(dbResult[47]);
    user_find->price[6] = atoi(dbResult[48]);
    user_find->price[7] = atoi(dbResult[49]);
    user_find->price[8] = atoi(dbResult[50]);
    user_find->price[9] = atoi(dbResult[51]);
    user_find->price_total = atoi(dbResult[52]);
    strncpy(user_find->cus_addr, dbResult[53], sizeof(user_find->cus_addr));
    strncpy(user_find->book_time, dbResult[54], sizeof(user_find->book_time));
    strncpy(user_find->remark, dbResult[55], sizeof(user_find->remark));
    strncpy(user_find->phone_num, dbResult[56], sizeof(user_find->phone_num));
    strncpy(user_find->cust_review, dbResult[57], sizeof(user_find->cust_review));
    strncpy(user_find->res_review, dbResult[58], sizeof(user_find->res_review));
    user_find->flag = atoi(dbResult[59]);
    sqlite3_free_table(dbResult);
    return 0;
}
//根据商家获取菜单数组 商家用
menu *get_menu_by_name_res(t_s8int *name, t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列

    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from menu where name='%s';", name);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        //  INFO_MSG("find nothing \n");
        return NULL;
    }
    menu data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    menu *ret_data = malloc(nRow * sizeof(menu));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {
                switch (j)
                {
                case 0:
                    strncpy(data[i].name, dbResult[index], sizeof(data[i].name));

                    break;

                case 1:
                    strncpy(data[i].pass, dbResult[index], sizeof(data[i].pass));

                    break;
                case 2:
                    strncpy(data[i].food, dbResult[index], sizeof(data[i].food));
                    break;
                case 3:
                    data[i].price = atoi(dbResult[index]);
                    break;
                case 4:
                    data[i].flag = atoi(dbResult[index]);
                    break;
                }

                ///   printf("字段值:%s,%d \n", dbResult[index], index);

                ++index;
            }
        }

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
            // printf("%s \n", data[i].name);
        }

        return ret_data;
    }

    sqlite3_free_table(dbResult);
}
//获取所有注册的商家列表
menu *get_rst_by_cus(t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列

    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from res ;");
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things");
        return NULL;
    }
    menu data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    menu *ret_data = malloc(nRow * sizeof(menu));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)
            {
                switch (j)
                {
                case 0:
                    strncpy(data[i].name, dbResult[index], sizeof(data[i].name));

                    break;

                    /*   case 1:
                    strncpy(data[i].pass, dbResult[index], sizeof(data[i].pass));

                    break;
               case 2:
                    strncpy(data[i].food, dbResult[index], sizeof(data[i].food));
                    break;
                case 3:
                    data[i].price = atoi(dbResult[index]);
                    break;
                case 4:
                    data[i].flag = atoi(dbResult[index]); */
                    break;
                }

                //  printf("字段值:%s,%d \n", dbResult[index], index);

                ++index;
            }
        }

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
            // printf("%s \n", data[i].name);
        }

        return ret_data;
    }

    sqlite3_free_table(dbResult);
}
//根据商家获取菜单数组 用户用
menu *get_menu_by_name_cus(t_s8int *name, t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列

    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from menu where name='%s';", name);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things");
        printf("name: %s\n", name);
        return NULL;
    }
    menu data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    menu *ret_data = malloc(nRow * sizeof(menu));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {
                switch (j)
                {

                case 0:
                    strncpy(data[i].name, dbResult[index], sizeof(data[i].name));

                    break;

                    //  case 1:
                    //      strncpy(data[i].pass, dbResult[index], sizeof(data[i].pass));

                    break;
                case 2:
                    strncpy(data[i].food, dbResult[index], sizeof(data[i].food));
                    break;
                case 3:
                    data[i].price = atoi(dbResult[index]);
                    break;
                case 4:
                    data[i].flag = atoi(dbResult[index]);
                    break;
                }

                ++index;
            }
        }

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
            // printf("%s \n", data[i].name);
        }

        return ret_data;
    }

    sqlite3_free_table(dbResult);
}

//根据用户名获取用户信息数组
cus_info *get_cus_info_by_name(t_s8int *name, t_s32int *size)
{

    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from cus where name='%s';", name);
    printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if (0 == nRow || 0 == nColumn)
    {
        INFO_MSG("cant find things");
        return NULL;
    }
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }
    cus_info data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {

                //  printf("字段值:%s,%d %d %d\n", dbResult[index], index, nRow, nColumn);
                switch (j)
                {
                case 0:
                    sprintf(data[i].name, "%s", dbResult[index]);
                    break;
                case 1:
                    sprintf(data[i].password, "%s", dbResult[index]);
                    break;
                case 2:
                    data[i].flag = atoi(dbResult[index]);
                    ;
                    break;
                }

                ++index;
            }
        }

        for (int i = 0; i < nRow; i++)
        {
            printf("%s \n", data[i].name);
        }
        cus_info *data_ret = malloc(sizeof(cus_info) * (*size));

        for (int i = 0; i < (*size); i++)
        {
            data_ret[i] = data[i];
        }
        return data_ret;
    }
    sqlite3_free_table(dbResult);
}
//根据客户获取订单数组
order_info *get_order_info_by_cust_name(t_s8int *name, t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from order_info where customer='%s';", name);
    //printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if ((0 == nRow) || (0 == nColumn))
    {
        INFO_MSG("cant find things\n");
        return NULL;
    }
    // DEBUG_MSG("find ting\n");
    order_info data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    order_info *ret_data = malloc(nRow * sizeof(order_info));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {

                switch (j - 1)
                {
                case -1:
                    strncpy(data[i].id, dbResult[index], sizeof(data[i].id));
                    break;
                case 0:
                    strncpy(data[i].restaurant, dbResult[index], sizeof(data[i].restaurant));
                    break;
                case 1:
                    strncpy(data[i].customer, dbResult[index], sizeof(data[i].customer));

                    break;
                case 2:
                    strncpy(data[i].food[0], dbResult[index], sizeof(data[i].food[0]));

                    break;
                case 3:
                    strncpy(data[i].food[1], dbResult[index], sizeof(data[i].food[1]));

                    break;
                case 4:
                    strncpy(data[i].food[2], dbResult[index], sizeof(data[i].food[2]));

                    break;
                case 5:
                    strncpy(data[i].food[3], dbResult[index], sizeof(data[i].food[3]));

                    break;
                case 6:
                    strncpy(data[i].food[4], dbResult[index], sizeof(data[i].food[4]));

                    break;
                case 7:
                    strncpy(data[i].food[5], dbResult[index], sizeof(data[i].food[5]));

                    break;
                case 8:
                    strncpy(data[i].food[6], dbResult[index], sizeof(data[i].food[6]));

                    break;
                case 9:
                    strncpy(data[i].food[7], dbResult[index], sizeof(data[i].food[7]));

                    break;
                case 10:
                    strncpy(data[i].food[8], dbResult[index], sizeof(data[i].food[8]));

                    break;
                case 11:
                    strncpy(data[i].food[9], dbResult[index], sizeof(data[i].food[9]));

                    break;
                case 12:
                    data[i].price[0] = atoi(dbResult[index]);

                    break;
                case 13:
                    data[i].price[1] = atoi(dbResult[index]);

                    break;
                case 14:
                    data[i].price[2] = atoi(dbResult[index]);

                    break;
                case 15:
                    data[i].price[3] = atoi(dbResult[index]);

                    break;
                case 16:
                    data[i].price[4] = atoi(dbResult[index]);

                    break;
                case 17:
                    data[i].price[5] = atoi(dbResult[index]);

                    break;
                case 18:
                    data[i].price[6] = atoi(dbResult[index]);

                    break;
                case 19:
                    data[i].price[7] = atoi(dbResult[index]);

                    break;
                case 20:
                    data[i].price[8] = atoi(dbResult[index]);

                    break;
                case 21:
                    data[i].price[9] = atoi(dbResult[index]);

                    break;
                case 22:
                    data[i].price_total = atoi(dbResult[index]);

                    break;
                case 23:
                    strncpy(data[i].cus_addr, dbResult[index], sizeof(data[i].cus_addr));

                    break;
                case 24:
                    strncpy(data[i].book_time, dbResult[index], sizeof(data[i].book_time));

                    break;
                case 25:
                    strncpy(data[i].remark, dbResult[index], sizeof(data[i].remark));

                    break;
                case 26:
                    strncpy(data[i].phone_num, dbResult[index], sizeof(data[i].phone_num));

                    break;
                case 27:
                    strncpy(data[i].cust_review, dbResult[index], sizeof(data[i].cust_review));

                    break;
                case 28:
                    strncpy(data[i].res_review, dbResult[index], sizeof(data[i].res_review));

                    break;
                case 29:
                    data[i].flag = atoi(dbResult[index]);
                    break;
                }

                ++index;
            }
        }

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
        }

        sqlite3_free_table(dbResult);
        return ret_data;
    }
}
//根据商家获取订单 数组
order_info *get_order_info_by_res_name(t_s8int *res, t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from order_info where restaurant='%s';", res);
    //printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if ((0 == nRow) || (0 == nColumn))
    {
        INFO_MSG("cant find things\n");
        return NULL;
    }
    DEBUG_MSG("find thing\n");
    order_info data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    order_info *ret_data = malloc(nRow * sizeof(order_info));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {

                switch (j - 1)
                {
                case -1:
                    strncpy(data[i].id, dbResult[index], sizeof(data[i].id));
                    break;
                case 0:
                    strncpy(data[i].restaurant, dbResult[index], sizeof(data[i].restaurant));
                    break;
                case 1:
                    strncpy(data[i].customer, dbResult[index], sizeof(data[i].customer));

                    break;
                case 2:
                    strncpy(data[i].food[0], dbResult[index], sizeof(data[i].food[0]));

                    break;
                case 3:
                    strncpy(data[i].food[1], dbResult[index], sizeof(data[i].food[1]));

                    break;
                case 4:
                    strncpy(data[i].food[2], dbResult[index], sizeof(data[i].food[2]));

                    break;
                case 5:
                    strncpy(data[i].food[3], dbResult[index], sizeof(data[i].food[3]));

                    break;
                case 6:
                    strncpy(data[i].food[4], dbResult[index], sizeof(data[i].food[4]));

                    break;
                case 7:
                    strncpy(data[i].food[5], dbResult[index], sizeof(data[i].food[5]));

                    break;
                case 8:
                    strncpy(data[i].food[6], dbResult[index], sizeof(data[i].food[6]));

                    break;
                case 9:
                    strncpy(data[i].food[7], dbResult[index], sizeof(data[i].food[7]));

                    break;
                case 10:
                    strncpy(data[i].food[8], dbResult[index], sizeof(data[i].food[8]));

                    break;
                case 11:
                    strncpy(data[i].food[9], dbResult[index], sizeof(data[i].food[9]));

                    break;
                case 12:
                    data[i].price[0] = atoi(dbResult[index]);

                    break;
                case 13:
                    data[i].price[1] = atoi(dbResult[index]);

                    break;
                case 14:
                    data[i].price[2] = atoi(dbResult[index]);

                    break;
                case 15:
                    data[i].price[3] = atoi(dbResult[index]);

                    break;
                case 16:
                    data[i].price[4] = atoi(dbResult[index]);

                    break;
                case 17:
                    data[i].price[5] = atoi(dbResult[index]);

                    break;
                case 18:
                    data[i].price[6] = atoi(dbResult[index]);

                    break;
                case 19:
                    data[i].price[7] = atoi(dbResult[index]);

                    break;
                case 20:
                    data[i].price[8] = atoi(dbResult[index]);

                    break;
                case 21:
                    data[i].price[9] = atoi(dbResult[index]);

                    break;
                case 22:
                    data[i].price_total = atoi(dbResult[index]);

                    break;
                case 23:
                    strncpy(data[i].cus_addr, dbResult[index], sizeof(data[i].cus_addr));

                    break;
                case 24:
                    strncpy(data[i].book_time, dbResult[index], sizeof(data[i].book_time));

                    break;
                case 25:
                    strncpy(data[i].remark, dbResult[index], sizeof(data[i].remark));

                    break;
                case 26:
                    strncpy(data[i].phone_num, dbResult[index], sizeof(data[i].phone_num));

                    break;
                case 27:
                    strncpy(data[i].cust_review, dbResult[index], sizeof(data[i].cust_review));

                    break;
                case 28:
                    strncpy(data[i].res_review, dbResult[index], sizeof(data[i].res_review));

                    break;
                case 29:
                    data[i].flag = atoi(dbResult[index]);
                    break;
                }

                //  printf("字段值:%s,%d \n", dbResult[index], index);

                ++index;
            }
        }
        /*restaurant, customer, food1, food2,
            food3, food4, food5, food6, food7, food8,
            food9, food10, price1, price2, price3, price4,
            price5, price6, price7, price8, price9,
            price10, price_total, cus_addr, book_time, remark,
            phone_num, cust_review, res_review, flag)*/
        // 30-59 下标

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
            // printf("%s \n", data[i].phone_num);
        }

        sqlite3_free_table(dbResult);
        return ret_data;
    }
}

//根据商家获取订单 flag为2
order_info *get_order_info_by_res_by_flag_name(t_s8int *res, t_s32int *size)
{
    t_s8int buf[200];
    //char *name = "x";
    sqlite3 *db;
    char *errmsg = NULL; //用来存储错误信息字符串
    t_s8int ret = 0;
    t_s32int index;
    char **dbResult;
    t_s32int nRow = 0, nColumn = 0; //nRow 查找出的总行数,nColumn 存储列
    ret = sqlite3_open("t.db", &db);
    if (1 == ret) //数据库创建未成功
    {
        fprintf(stderr, "Can't open this database: %s\n", sqlite3_errmsg(db)); //用sqlite3_errmsg()得到错误字符串
        sqlite3_close(db);
        return NULL;
    }
    sprintf(buf, "select * from order_info where restaurant='%s' and flag = 2;", res);
    //printf("%s \n", buf);
    ret = sqlite3_get_table(db, buf, &dbResult, &nRow, &nColumn, &errmsg);
    if ((0 == nRow) || (0 == nColumn))
    {
        INFO_MSG("cant find things\n");
        return NULL;
    }
    DEBUG_MSG("find thing\n");
    order_info data[nRow];
    bzero(data, sizeof(data));
    *size = nRow;
    order_info *ret_data = malloc(nRow * sizeof(order_info));
    if (NULL != errmsg)
    {
        sqlite3_free_table(dbResult);
        printf("cant find things\n");
        errmsg = NULL;
        return NULL;
    }

    if (SQLITE_OK == ret)

    {

        index = nColumn;

        for (t_s32int i = 0; i < nRow; i++)

        {
            for (t_s32int j = 0; j < nColumn; j++)

            {

                switch (j - 1)
                {
                case -1:
                    strncpy(data[i].id, dbResult[index], sizeof(data[i].id));
                    break;
                case 0:
                    strncpy(data[i].restaurant, dbResult[index], sizeof(data[i].restaurant));
                    break;
                case 1:
                    strncpy(data[i].customer, dbResult[index], sizeof(data[i].customer));

                    break;
                case 2:
                    strncpy(data[i].food[0], dbResult[index], sizeof(data[i].food[0]));

                    break;
                case 3:
                    strncpy(data[i].food[1], dbResult[index], sizeof(data[i].food[1]));

                    break;
                case 4:
                    strncpy(data[i].food[2], dbResult[index], sizeof(data[i].food[2]));

                    break;
                case 5:
                    strncpy(data[i].food[3], dbResult[index], sizeof(data[i].food[3]));

                    break;
                case 6:
                    strncpy(data[i].food[4], dbResult[index], sizeof(data[i].food[4]));

                    break;
                case 7:
                    strncpy(data[i].food[5], dbResult[index], sizeof(data[i].food[5]));

                    break;
                case 8:
                    strncpy(data[i].food[6], dbResult[index], sizeof(data[i].food[6]));

                    break;
                case 9:
                    strncpy(data[i].food[7], dbResult[index], sizeof(data[i].food[7]));

                    break;
                case 10:
                    strncpy(data[i].food[8], dbResult[index], sizeof(data[i].food[8]));

                    break;
                case 11:
                    strncpy(data[i].food[9], dbResult[index], sizeof(data[i].food[9]));

                    break;
                case 12:
                    data[i].price[0] = atoi(dbResult[index]);

                    break;
                case 13:
                    data[i].price[1] = atoi(dbResult[index]);

                    break;
                case 14:
                    data[i].price[2] = atoi(dbResult[index]);

                    break;
                case 15:
                    data[i].price[3] = atoi(dbResult[index]);

                    break;
                case 16:
                    data[i].price[4] = atoi(dbResult[index]);

                    break;
                case 17:
                    data[i].price[5] = atoi(dbResult[index]);

                    break;
                case 18:
                    data[i].price[6] = atoi(dbResult[index]);

                    break;
                case 19:
                    data[i].price[7] = atoi(dbResult[index]);

                    break;
                case 20:
                    data[i].price[8] = atoi(dbResult[index]);

                    break;
                case 21:
                    data[i].price[9] = atoi(dbResult[index]);

                    break;
                case 22:
                    data[i].price_total = atoi(dbResult[index]);

                    break;
                case 23:
                    strncpy(data[i].cus_addr, dbResult[index], sizeof(data[i].cus_addr));

                    break;
                case 24:
                    strncpy(data[i].book_time, dbResult[index], sizeof(data[i].book_time));

                    break;
                case 25:
                    strncpy(data[i].remark, dbResult[index], sizeof(data[i].remark));

                    break;
                case 26:
                    strncpy(data[i].phone_num, dbResult[index], sizeof(data[i].phone_num));

                    break;
                case 27:
                    strncpy(data[i].cust_review, dbResult[index], sizeof(data[i].cust_review));

                    break;
                case 28:
                    strncpy(data[i].res_review, dbResult[index], sizeof(data[i].res_review));

                    break;
                case 29:
                    data[i].flag = atoi(dbResult[index]);
                    break;
                }

                //  printf("字段值:%s,%d \n", dbResult[index], index);

                ++index;
            }
        }
        /*restaurant, customer, food1, food2,
            food3, food4, food5, food6, food7, food8,
            food9, food10, price1, price2, price3, price4,
            price5, price6, price7, price8, price9,
            price10, price_total, cus_addr, book_time, remark,
            phone_num, cust_review, res_review, flag)*/
        // 30-59 下标

        for (int i = 0; i < (*size); i++)
        {
            ret_data[i] = data[i];
            // printf("%s \n", data[i].phone_num);
        }

        sqlite3_free_table(dbResult);
        return ret_data;
    }
}

//插入订单信息
t_s32int insert_order(order_info info)
{

    t_s8int buf[500];
    sprintf(buf, "insert into order_info values('%s','%s','%s','%s','%s','%s','%s','%s',\
    '%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',\
    '%s','%s','%s','%s','%s',%d);",
            info.id, info.restaurant, info.customer, info.food[0], info.food[1],
            info.food[2], info.food[3], info.food[4], info.food[5], info.food[6], info.food[7],
            info.food[8], info.food[9], info.price[0], info.price[1], info.price[2], info.price[3],
            info.price[4], info.price[5], info.price[6], info.price[7], info.price[8],
            info.price[9], info.price_total, info.cus_addr, info.book_time, info.remark,
            info.phone_num, info.cust_review, info.res_review, info.flag);

    sql(buf, NULL);
    // printf("%s \n", buf);
}

//插入菜单信息
t_s32int insert_menu(menu info)
{
    t_s8int buf[200];
    sprintf(buf, "insert into menu values('%s','%s','%s',%d,%d);", info.name, info.pass, info.food, info.price, info.flag);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}

//插入商家账户信息
void insert_res(menu info)
{
    t_s8int buf[200];
    sprintf(buf, "insert into res values('%s','%s','%s',%d,%d);", info.name, info.pass, info.food, info.price, info.flag);
    sql(buf, NULL);
}

//插入用户信息
void insert_cus(t_s8int *name, t_s8int *pass, t_s32int flag)
{
    t_s8int buf[200];
    sprintf(buf, "insert into cus values('%s','%s',%d);", name, pass, flag);
    sql(buf, NULL);
}

//根据订单号删除订单
t_s32int order_del_by_id(char *id)
{
    t_s8int buf[200];
    sprintf(buf, "delete from order_info where id='%s';", id);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_OK;
}

//根据菜品删除菜单
t_s32int menu_del_by_food(char *food, char *name)
{
    t_s8int buf[200];
    sprintf(buf, "delete from menu where food='%s' and name='%s'  ;", food, name);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}

//根据商家名字删除订单
t_s32int order_del_by_rst(char *name)
{
    t_s8int buf[200];
    sprintf(buf, "delete from order_info where restaurant='%s' ;", name);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_OK;
}
//根据商家名字删除菜单
t_s32int menu_del_by_rest_name(char *name)
{
    t_s8int buf[200];
    sprintf(buf, "delete from menu where  name='%s'  ;", name);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_OK;
}
//根据用户名删除用户
t_s32int user_del_by_name(char *name)
{
    t_s8int buf[200];
    sprintf(buf, "delete from cus where name='%s';", name);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}

//修改用户评价
t_s32int update_cust_review(char *id, char *review)
{

    t_s8int buf[200];
    sprintf(buf, "update order_info set cust_review ='%s' where id='%s';", review, id);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}

//修改商家菜名
t_s32int update_menu_food(char *name, char *food)
{

    t_s8int buf[200];
    sprintf(buf, "update menu set food ='%s' where name='%s';", food, name);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}
//修改商家评价
t_s32int update_res_review(char *id, char *review)
{

    t_s8int buf[200];
    sprintf(buf, "update order_info set res_review ='%s' where id='%s';", review, id);
    if (FUNC_OK != sql(buf, NULL))
    {
        return FUNC_ERR;
    }
    return FUNC_ERR;
}

t_s32int main1(void)
{

    order_info or ;
    bzero(& or, sizeof(or));

    strncpy(or.restaurant, "ad", sizeof(or.restaurant));
    strncpy(or.food[0], "food1", sizeof(or.restaurant));
    strncpy(or.food[1], "food2", sizeof(or.restaurant));
    strncpy(or.food[2], "food3", sizeof(or.restaurant));

    for (int i = 0; i < 2; i++)
    {
        insert_order(or);
    }
}
