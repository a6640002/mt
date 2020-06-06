#include "s.h"

int main(void)
{
    signal(SIGPIPE, SIG_IGN);
    int listen_fd = socket(PF_INET, SOCK_STREAM, 0), conn_fd;
    //setsockopt(listen_fd,SOL_SOCKET,SO_REUSERADDR,)
    if (listen_fd < 0)
    {
        perror("listen_fd");
        return -1;
    }
    sockaddr_in addr;
    init_sockaddr(&addr);
    if (0 > bind(listen_fd, (sockaddr *)&addr, sizeof(sockaddr_in)))
    {
        {
            perror("bind");
            return -1;
        }
    }
    listen(listen_fd, 10);
    //  pthread_create(&conn_ph, 0, thread_fun_listen, NULL);

    while (1)
    {
        conn_fd = accept(listen_fd, NULL, NULL);
        if (conn_fd < 0)
        {
            perror("accept");
            return -1;
        }
        pthread_t thread_rst, thread_custon;
        fun_arg arg_new;
        arg_new.conn_fd = conn_fd;
        // get_menu_by_name_by_cus_server(conn_fd);
        pthread_create(&thread_rst, 0, thread_fun_rest, &arg_new);
    }
}