//
// Created by thehepta on 2024/2/19.
//

#include <android/log.h>
#include "lsplt/include/lsplt.hpp"
#include <cinttypes>
#include <sys/sysmacros.h>
#include <asm-generic/mman.h>
#include <unistd.h>
#include "vector"
#include "array"

#define LOG_TAG "DrmIdHook"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


struct MapInfo {
    /// \brief The start address of the memory region.
    uintptr_t start;
    /// \brief The end address of the memory region.
    uintptr_t end;
    /// \brief The permissions of the memory region. This is a bit mask of the following values:
    /// - PROT_READ
    /// - PROT_WRITE
    /// - PROT_EXEC
    uint8_t perms;
    /// \brief Whether the memory region is private.
    bool is_private;
    /// \brief The offset of the memory region.
    uintptr_t offset;
    /// \brief The device number of the memory region.
    /// Major can be obtained by #major()
    /// Minor can be obtained by #minor()
    dev_t dev;
    /// \brief The inode number of the memory region.
    ino_t inode;
    /// \brief The path of the memory region.
    std::string path;


};


std::vector<MapInfo> Scan(pid_t pid) {
    constexpr static auto kPermLength = 5;
    constexpr static auto kMapEntry = 7;
    std::vector<MapInfo> info;
    auto path = "/proc/" + std::to_string(pid) + "/maps";
    auto maps = std::unique_ptr<FILE, decltype(&fclose)>{fopen(path.c_str(), "r"), &fclose};
    if (maps) {
        char *line = nullptr;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, maps.get())) > 0) {
            line[read - 1] = '\0';
            uintptr_t start = 0;
            uintptr_t end = 0;
            uintptr_t off = 0;
            ino_t inode = 0;
            unsigned int dev_major = 0;
            unsigned int dev_minor = 0;
            std::array<char, kPermLength> perm{'\0'};
            int path_off;
            if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s %" PRIxPTR " %x:%x %lu %n%*s", &start,
                    &end, perm.data(), &off, &dev_major, &dev_minor, &inode,
                    &path_off) != kMapEntry) {
                continue;
            }
            while (path_off < read && isspace(line[path_off])) path_off++;
            auto ref = MapInfo{start, end, 0, perm[3] == 'p', off,
                               static_cast<dev_t>(makedev(dev_major, dev_minor)),
                               inode, line + path_off};
            if (perm[0] == 'r') ref.perms |= PROT_READ;
            if (perm[1] == 'w') ref.perms |= PROT_WRITE;
            if (perm[2] == 'x') ref.perms |= PROT_EXEC;
            info.emplace_back(ref);
        }
        free(line);
    }
    return info;
}


char * replace_drm;
int length = -1;
void * (* old_getDeviceUniqueId)(void * arg1,intptr_t * drmid);

void * new_getDeviceUniqueId(void * arg1, intptr_t * drmid){
    void * ret = old_getDeviceUniqueId(arg1,drmid);
    LOGE("new_getDeviceUniqueId %ld\n",drmid[0]);
    LOGE("new_getDeviceUniqueId %ld\n",drmid[1]);
    LOGE("new_getDeviceUniqueId %lx\n",drmid[2]);
    memcpy(reinterpret_cast<void *>(drmid[2]), replace_drm, length);
    return ret;
}


// 将16进制字符串转换为字节数组
std::vector<uint8_t> hex_string_to_bytes(std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_string = hex.substr(i, 2);
        auto byte = static_cast<uint8_t>(stoi(byte_string, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

void DrmIdHook(const char* AUTHORITY){
    LOGE("DrmIdHook start1 %s\n",AUTHORITY);
    std::string tmp_string = strdup(AUTHORITY);
    std::vector<uint8_t> memory = hex_string_to_bytes(tmp_string);
    length = tmp_string.length()/2;
    replace_drm = static_cast<char *>(malloc(length));
    memcpy(replace_drm,memory.data(),length);
    ino_t art_inode = 0;
    dev_t art_dev = 0;
    for (auto &map : Scan(getpid())) {
        if (map.path.find("libwvhidl.so") != std::string_view::npos) {
            LOGE("found libwvhidl\n");

            art_inode = map.inode;
            art_dev = map.dev;
            break;
        }
    }
    char *symbol = "_ZN5wvdrm8hardware3drm4V1_48widevine11WVDrmPlugin20CdmIdentifierBuilder17getDeviceUniqueIdEPNSt3__112basic_stringIcNS6_11char_traitsIcEENS6_9allocatorIcEEEE";
    if (!lsplt::RegisterHook(art_dev, art_inode, symbol,(void* )new_getDeviceUniqueId, (void **)&old_getDeviceUniqueId)) {
        LOGE("Failed to register plt_hook getDeviceUniqueId \n");
        return;
    }

    lsplt::CommitHook();

}


