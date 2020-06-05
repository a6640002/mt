#include "s.h"

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
    char buf[200];
    bzero(buf, 200);
    menu s;
    int size = 0;
    order_info o;
    // write(conn, "ad", 3);
    // read(conn, &size, 4);
    // printf("%d \n", size);
    // for (int i = 0; i < size; i++)
    // {
    //     read(conn, &s, sizeof(s));
    //     printf("%s \n", s.food);
    // }
    // char buff[200];
    // bzero(buff, 200);
    // //read(conn, buff, 200);
    // printf("%s,\n", buff);
}