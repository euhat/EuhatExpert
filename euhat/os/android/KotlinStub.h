#pragma once

#include <common/JyMutex.h>

class KotlinStub
{
    JavaVM *savedVm_;
    jobject mainActivity_;

    jclass classSaved_;
    jobject objSaved_;
    jmethodID methodCallFromCpp_;
    jmethodID methodUpdateClipboard_;

public:

    KotlinStub();
    ~KotlinStub();

    static KotlinStub *getInstance();

    int init(JNIEnv *env, jobject pThis);
    void fini();

    void setVm(JavaVM *vm);
    string callFromCpp(const char *str, int i);
    void updateClipboard(const char *str);


};