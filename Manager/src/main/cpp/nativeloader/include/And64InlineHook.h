//
// Created by thehepta on 2023/6/8.
//

#ifndef RXPOSED_AND64INLINEHOOK_H
#define RXPOSED_AND64INLINEHOOK_H


#pragma once
#define A64_MAX_BACKUPS 256

#ifdef __cplusplus
extern "C" {
#endif

void A64HookFunction(void *const symbol, void *const replace, void **result);
void *A64HookFunctionV(void *const symbol, void *const replace,
                       void *const rwx, const uintptr_t rwx_size);

#ifdef __cplusplus
}
#endif

#endif //RXPOSED_AND64INLINEHOOK_H
