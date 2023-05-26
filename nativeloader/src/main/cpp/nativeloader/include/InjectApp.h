//
// Created by chic on 2023/5/15.
//

#ifndef RXPOSED_INJECTAPP_H
#define RXPOSED_INJECTAPP_H




#include "dobby.h"
#include "tool.h"



class injectApp {

public:

    static injectApp *GetInstance()
    {
        if( instance_ == nullptr )
        {
            instance_ = new injectApp();
        }
        return instance_;
    }


    void LoadExternApk(char *pkgName_arg);
    JNIEnv *Pre_GetEnv();




protected:
    static injectApp  *instance_; //引用性声明

};


#endif //RXPOSED_INJECTAPP_H
