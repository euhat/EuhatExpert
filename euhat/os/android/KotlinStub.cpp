#include <common/OpCommon.h>
#include <jni.h>
#include "KotlinStub.h"

KotlinStub::KotlinStub()
{

}

KotlinStub::~KotlinStub()
{

}

KotlinStub *gKotlinStub = NULL;

KotlinStub *KotlinStub::getInstance()
{
    if (NULL == gKotlinStub)
        gKotlinStub = new KotlinStub();
    return gKotlinStub;
}

int KotlinStub::init(JNIEnv *env, jobject pThis)
{
    mainActivity_ = (jobject)env->NewGlobalRef(pThis);

    jclass clazz = env->FindClass("com/euhat/euhatexpert/OpFileJni");
    classSaved_ = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    jmethodID construct = env->GetMethodID(classSaved_, "<init>", "()V");

    jobject obj = env->NewObject(classSaved_, construct);
    objSaved_ = (jobject)env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);

    // need not globally ref jmethodID
    methodCallFromCpp_ = env->GetMethodID(classSaved_, "callFromCpp", "(Ljava/lang/String;I)Ljava/lang/String;");
    methodUpdateClipboard_ = env->GetMethodID(classSaved_, "updateClipboard", "(Lcom/euhat/euhatexpert/MainActivity;Ljava/lang/String;)V");

    return 1;
}

void KotlinStub::fini()
{
    JavaVM *vm = savedVm_;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);

    env->DeleteGlobalRef(objSaved_);
    env->DeleteGlobalRef(classSaved_);

    env->DeleteGlobalRef(mainActivity_);

    vm->DetachCurrentThread();
}

void KotlinStub::setVm(JavaVM *vm)
{
    savedVm_ = vm;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    DBG(("I'm loaded==========================\n"));

    KotlinStub::getInstance()->setVm(vm);

    return JNI_VERSION_1_6;
}

string KotlinStub::callFromCpp(const char *str, int i)
{
    JavaVM *vm = savedVm_;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);

    jstring message = env->NewStringUTF("1");
    jstring jResult = (jstring)env->CallObjectMethod(objSaved_, methodCallFromCpp_, message, 1);

    const char *result = env->GetStringUTFChars(jResult, JNI_FALSE);
    env->ReleaseStringUTFChars(jResult, result);

    vm->DetachCurrentThread();
}

void KotlinStub::updateClipboard(const char *str)
{
    JavaVM *vm = savedVm_;
    JNIEnv *env;
    vm->AttachCurrentThread(&env, NULL);

    jstring content = env->NewStringUTF(str);
    env->CallVoidMethod(objSaved_, methodUpdateClipboard_, mainActivity_, content);

    vm->DetachCurrentThread();
}

void whUpdateClipboard(const char *msg)
{
    KotlinStub::getInstance()->updateClipboard(msg);
}