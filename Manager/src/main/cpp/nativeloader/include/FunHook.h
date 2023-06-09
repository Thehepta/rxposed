//
// Created by thehepta on 2023/6/8.
//

#ifndef RXPOSED_FUNHOOK_H
#define RXPOSED_FUNHOOK_H




void * get__system_property_get_addr();
void * get_AndroidRuntimeGetEnv_addr();
void * get_getUidForName_addr();
void * get_android_os_Process_setArgV0_addr();
#endif //RXPOSED_FUNHOOK_H
