#include <cinttypes>
#include <list>
#include <sys/mman.h>
#include <set>
#include <string_view>

#include <android/log.h>
#include <string>
#include <jni.h>
#include <dlfcn.h>
#include <fstream>
#include <unistd.h>
#include <vector>
#define LOG_TAG "Native"
using namespace std;


using namespace std;


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)



extern "C"
JNIEXPORT jboolean JNICALL
 Java_hepta_rxposed_manager_fragment_HomeFragment_get_1rxposed_1activity(JNIEnv *env, jobject thiz) {
    // TODO: implement get_rxposed_activity()
    jboolean re = false;
    char sdk_ver[32];
    memset(sdk_ver,0,32);
    __system_property_get("rxposed_activity", sdk_ver);
    if(!strncmp(sdk_ver,"true", strlen("true"))){
        re = true;
    }
    return re;

}
