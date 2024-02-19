//
// Created by chic on 2023/5/15.
//

#ifndef RXPOSED_INJECTAPP_H
#define RXPOSED_INJECTAPP_H




#include "tool.h"



class InjectApp {

public:

    static InjectApp *GetInstance()
    {
        if( instance_ == nullptr )
        {
            instance_ = new InjectApp();
        }
        return instance_;
    }


    void LoadExternApk(char *pkgName_arg);




protected:
    static InjectApp  *instance_; //引用性声明

};


bool hookinit(JNIEnv *env);
#endif //RXPOSED_INJECTAPP_H
