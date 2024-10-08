#ifndef CONNECT_WORKAROUND_H
#define CONNECT_WORKAROUND_H


#include <sys/socket.h>

//resolve o problema do sockets e qt terem uma função com mesmo nome, que é connect
int connect_socket(int __fd, const sockaddr *__addr, socklen_t __len){//evitar erro com o connect do qt
  return connect(__fd, __addr, __len);
}


#endif
