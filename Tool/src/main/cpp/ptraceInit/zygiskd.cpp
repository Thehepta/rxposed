//
// Created by chic on 2024/12/4.
//

#include "zygiskd.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <vector>
#include <linux/in.h>
#include "socket_utils.h"
#include "RootImp.h"
#include <thread>
#include <fcntl.h>
#include <sys/epoll.h>

#define EPOLL_SIZE 10




void Zygiskd::foreach_module(int fd,dirent *entry,int modfd){
    if (faccessat(modfd, "disable", F_OK, 0) == 0) {
        return;
    }
    Module info;
    LOGD("enable_module:%s",entry->d_name);
    if (faccessat(modfd, "zygisk/armeabi-v7a.so", F_OK, 0) != 0) {
        return;
    }
    info.z32 = openat(modfd, "zygisk/armeabi-v7a.so", O_RDONLY | O_CLOEXEC);
    if (faccessat(modfd, "zygisk/arm64-v8a.so", F_OK, 0) != 0) {
        return;
    }
    info.z64 = openat(modfd, "zygisk/arm64-v8a.so", O_RDONLY | O_CLOEXEC);
    info.name = entry->d_name;

    module_list.push_back(info);

}

void Zygiskd::collect_modules(){
    moduleRoot = MODULEROOT;
    auto dir = opendir(MODULEROOT);
    if (!dir)
        return;
    int dfd = dirfd(dir);
    for (dirent *entry; (entry = readdir(dir));) {
        if (entry->d_type == DT_DIR && (0 != strcmp(entry->d_name, "."))&& (0 != strcmp(entry->d_name, ".."))) {
            int modfd = openat(dfd, entry->d_name, O_RDONLY | O_CLOEXEC);
            foreach_module(dfd, entry, modfd);
            close(modfd);
        }
    }
}

void Zygiskd::load_modules() {


}

void handle_daemon_action(int cmd,int fd) {
    LOGD("handle_daemon_action");

    switch (cmd) {
        case (uint8_t)zygiskComm::SocketAction::RequestLogcatFd:{
            LOGD("RequestLogcatFd");
            while (1){
                std::string msg =socket_utils::read_string(fd);
            }
            break;
        }

        case (uint8_t)zygiskComm::SocketAction::GetProcessFlags:{
            uid_t uid =socket_utils::read_u32(fd);
            int flags =  RootImp::getInstance().getProcessFlags(uid);
            socket_utils::write_u32(fd,flags);
            LOGD("GetProcessFlags");
            break;
        }
        case (uint8_t)zygiskComm::SocketAction::ReadModules:
            LOGD("ReadModules");
            zygiskComm::WriteModules(fd,Zygiskd::getInstance().getModule_list());
            break;
        case (uint8_t)zygiskComm::SocketAction::RequestCompanionSocket:
            LOGD("RequestCompanionSocket");
            break;
        case (uint8_t)zygiskComm::SocketAction::GetModuleDir:
            LOGD("GetModuleDir");
            size_t index = socket_utils::read_usize(fd);
            auto moduleDir = Zygiskd::getInstance().getModul_by_index(index);
            int send_fd = open(moduleDir.c_str(),O_RDONLY);
            socket_utils::send_fd(fd,send_fd);
            break;
    }

    close(fd);
}

void zygiskd_handle(int client_fd){
    uint8_t cmd =  socket_utils::read_u8(client_fd);
    switch (cmd) {
        case (uint8_t)zygiskComm::SocketAction::PingHeartBeat:
            LOGD("HandleEvent PingHeartBeat");
            break;
        case (uint8_t)zygiskComm::SocketAction::ZygoteRestart:
            LOGD("HandleEvent ZygoteRestart");
            break;

        case (uint8_t)zygiskComm::SocketAction::SystemServerStarted:
            LOGD("HandleEvent SystemServerStarted");
            break;
        default:
            LOGD("HandleEvent default");
            int new_fd = dup(client_fd);
            std::thread ZygiskThread(handle_daemon_action,cmd, new_fd);
            ZygiskThread.detach();
    }
}

void zygiskd_main(const char* requestSocketPath){

    int server_fd;
    int client_fd;
    int epfd;

    zygiskComm::InitRequestorSocket(requestSocketPath);
    Zygiskd::getInstance().collect_modules();
    server_fd = socket(AF_UNIX,SOCK_STREAM,0);

    if (server_fd < 0){
        LOGE("Create Socket Errors, %d",server_fd);
        exit(server_fd);
    }
    struct sockaddr_un addr;
    zygiskComm::set_sockaddr(addr);
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOGE("bind error");
        return ;
    }

    if (listen(server_fd, 10)<0)
    {
        LOGE("listen error");
        return ;
    }

    epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0)
    {
        LOGE("Epoll Create");
        exit(-1);
    }

    struct epoll_event ev;
    struct epoll_event events[EPOLL_SIZE];
    ev.data.fd = server_fd;
    ev.events = EPOLLIN ;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFD, 0)| O_NONBLOCK);

    while (1)
    {
        int nfds = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if (nfds == -1)
        {
            if(errno != EINTR)
                LOGE("epoll failed");
            continue;
        }
        for (int i=0;i < nfds;i++)
        {
            if (events[i].data.fd==server_fd&&(events[i].events & EPOLLIN))
            {
                struct sockaddr_in remote_addr;

                socklen_t socklen = sizeof(struct sockaddr_in);
                client_fd = accept(server_fd, (struct sockaddr *)&remote_addr, &socklen);
                if (client_fd > 0)
                {
                    zygiskd_handle(client_fd);
                }
                close(client_fd);
            }else{
                LOGE("unknow  error else");
            }
        }
    }
    close(server_fd);
    close(epfd);
    return ;
}


