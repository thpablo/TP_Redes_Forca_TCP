#ifndef CONNECT_WORKAROUND_H
#define CONNECT_WORKAROUND_H


#include <sys/socket.h>

int connect_socket(int __fd, const sockaddr *__addr, socklen_t __len){//evitar erro com o connect do qt
  return connect(__fd, __addr, __len);
}

#endif
