#include <linux/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include "daemon.h"
#include "socket_utils.h"


namespace zygiskComm {

    static std::string TMP_PATH;



    void InitRequestorSocket(const char *path) {
        TMP_PATH = path;
    }

    std::string GetRequestorSocket() {
        return TMP_PATH;
    }

    int Connect(uint8_t retry) {
        int fd = socket( AF_UNIX, SOCK_STREAM, 0 );
        struct sockaddr_un remote_addr; //服务器端网络地址结构体
        set_sockaddr(remote_addr);

        while (retry--) {
            int r = connect(fd, reinterpret_cast<struct sockaddr*>(&remote_addr), sizeof(remote_addr));
            if (r == 0) return fd;
            if (retry) {
                PLOGE("Retrying to connect to zygiskd, sleep 1s\n");
                sleep(1);
            }
        }

        close(fd);
        return -1;
    }

    void set_sockaddr(struct sockaddr_un &addr ){
        memset(&addr,0,sizeof(addr));
        addr.sun_family=AF_UNIX;
        strcpy(addr.sun_path+1, GetRequestorSocket().c_str());
        addr.sun_path[0]='\0';
    }

    bool PingHeartbeat() {
        UniqueFd fd = Connect(5);
        if (fd == -1) {
            PLOGE("Connect to zygiskd");
            return false;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::PingHeartBeat);
        return true;
    }

    int RequestLogcatFd() {
        int fd = Connect(1);
        if (fd == -1) {
            PLOGE("RequestLogcatFd");
            return -1;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::RequestLogcatFd);
        return fd;
    }

    uint32_t GetProcessFlags(uid_t uid) {
        UniqueFd fd = Connect(1);
        if (fd == -1) {
            PLOGE("GetProcessFlags");
            return 0;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::GetProcessFlags);
        socket_utils::write_u32(fd, uid);
        return socket_utils::read_u32(fd);
    }
//这个函数会编译进注入库，同时被32为和64位使用，请求不同架构的文件描述符
    std::vector<Module> ReadModules() {
        std::vector<Module> modules;
        UniqueFd fd = Connect(1);
        if (fd == -1) {
            PLOGE("ReadModules");
            return modules;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::ReadModules);
        uint8_t arch=0;
#if defined(__LP64__)
        arch=1;
#endif
        socket_utils::write_u8(fd, arch);

        size_t len = socket_utils::read_usize(fd);
        for (size_t i = 0; i < len; i++) {
            Module md;
            std::string name = socket_utils::read_string(fd);
            int module_fd = socket_utils::recv_fd(fd);
            md.name = name;
            if(arch == 1){
                md.z64=module_fd;
            } else{
                md.z32=module_fd;
            }
            modules.emplace_back(md);
        }
        return modules;
    }
//这个函数只会在zygiskd 进程运行，并且只会运行64位，所有需要对两个架构做处理
    void WriteModules(int fd,std::vector<Module> modules){

        uint8_t arch = socket_utils::read_u8(fd);

        size_t len = modules.size();
        socket_utils::write_usize(fd,len);

        for (size_t i = 0; i < len; i++) {
            socket_utils::write_string(fd,modules[i].name);
            if(arch == 1){
                socket_utils::send_fd(fd,modules[i].z64);
            } else{
                socket_utils::send_fd(fd,modules[i].z32);

            }
        }

    }

    int ConnectCompanion(size_t index) {
        int fd = Connect(1);
        if (fd == -1) {
            PLOGE("ConnectCompanion");
            return -1;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::RequestCompanionSocket);
        socket_utils::write_usize(fd, index);
        if (socket_utils::read_u8(fd) == 1) {
            return fd;
        } else {
            close(fd);
            return -1;
        }
    }

    int GetModuleDir(size_t index) {
        UniqueFd fd = Connect(1);
        if (fd == -1) {
            PLOGE("GetModuleDir");
            return -1;
        }
        socket_utils::write_u8(fd, (uint8_t) SocketAction::GetModuleDir);
        socket_utils::write_usize(fd, index);
        return socket_utils::recv_fd(fd);
    }



    void ZygoteRestart() {
        UniqueFd fd = Connect(1);
        if (fd == -1) {
            if (errno == ENOENT) {
                LOGD("Could not notify ZygoteRestart (maybe it hasn't been created)");
            } else {
                LOGD("Could not notify ZygoteRestart");
            }
            return;
        }
        if (!socket_utils::write_u8(fd, (uint8_t) SocketAction::ZygoteRestart)) {
            LOGD("Failed to request ZygoteRestart");
        }
    }

    void SystemServerStarted() {
        UniqueFd fd = Connect(1);
        if (fd == -1) {
            PLOGE("Failed to report system server started");
        } else {
            if (!socket_utils::write_u8(fd, (uint8_t) SocketAction::SystemServerStarted)) {
                PLOGE("Failed to report system server started");
            }
        }
    }
}
