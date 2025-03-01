#pragma once

#include <string_view>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/un.h>



class UniqueFd {
    using Fd = int;
public:
    UniqueFd() = default;

    UniqueFd(Fd fd) : fd_(fd) {}

    ~UniqueFd() { if (fd_ >= 0) close(fd_); }

    // Disallow copy
    UniqueFd(const UniqueFd&) = delete;

    UniqueFd& operator=(const UniqueFd&) = delete;

    // Allow move
    UniqueFd(UniqueFd&& other) { std::swap(fd_, other.fd_); }

    UniqueFd& operator=(UniqueFd&& other) {
        std::swap(fd_, other.fd_);
        return *this;
    }

    // Implict cast to Fd
    operator const Fd&() const { return fd_; }

private:
    Fd fd_ = -1;
};


struct Module {
    std::string name;
    int z32 = -1;
    int z64 = -1;

};

namespace zygiskComm {

    enum class SocketAction {
        PingHeartBeat ,
        RequestLogcatFd,
        GetProcessFlags,
        ReadModules,
        RequestCompanionSocket,
        GetModuleDir,
        ZygoteRestart,
        SystemServerStarted,
    };

    void InitRequestorSocket(const char *path);

    std::string GetTmpPath();
    void WriteModules(int fd,std::vector<Module> modules);
    bool PingHeartbeat();

    int RequestLogcatFd();

    std::vector<Module> ReadModules();

    uint32_t GetProcessFlags(uid_t uid);

    int ConnectCompanion(size_t index);

    int GetModuleDir(size_t index);

    void ZygoteRestart();

    void SystemServerStarted();

    void set_sockaddr(struct sockaddr_un &addr);


}
