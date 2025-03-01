//
// Created by chic on 2024/12/4.
//

#ifndef ZYGISKNEXT_ZYGISKD_H
#define ZYGISKNEXT_ZYGISKD_H

#include <dirent.h>
#include "daemon.h"

#define SECURE_DIR      "/data/adb"
#define MODULEROOT      SECURE_DIR "/modules"





void zygiskd_main(const char *);


class Zygiskd  {

public:
    void  load_modules();
    void collect_modules();
    void foreach_module(int fd,dirent *entry,int modfd);
    static Zygiskd& getInstance(){
        static Zygiskd instance;
        return instance;
    }
    std::vector<Module> getModule_list(){
        return module_list;
    }

    std::string getModul_by_index(int index){
        return module_list.at(index).name;
    }

    Zygiskd(const Zygiskd&)= delete;
    Zygiskd& operator=(const Zygiskd)=delete;

private:
    Zygiskd(){
    }
    ~Zygiskd() {
    }

    std::vector<Module> module_list;
    std::string moduleRoot;
    bool running = false;
};






#endif //ZYGISKNEXT_ZYGISKD_H
