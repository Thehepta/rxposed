//
// Created by chic on 2022/8/22.
//

#ifndef SUPERSU_SERVICE_H
#define SUPERSU_SERVICE_H
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <climits>
#include <fcntl.h>

#include <termios.h>


#include <android/log.h>
#include <string>


int service_main(std::string REQUESTOR_SOCKET);
std::string client_main(std::string REQUESTOR_SOCKET,uid_t currentUid,std::string AUTHORITY,std::string pkgName);


#endif //SUPERSU_SERVICE_H
