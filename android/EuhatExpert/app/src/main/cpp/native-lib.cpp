#include <jni.h>
#include <string>
#include <common/OpCommon.h>
#include <dbop/DbOpIni.h>
#include "macHdd.h"
#include <app/FileMan/common/FileManFileOp.h>
#include <app/FileMan/server/FileManServer.h>
#include <common/JyTcpSelector.h>
#include <linux/if.h>
#include <sys/stat.h>
#include <../config.h>

using namespace std;

#define INI_SECTION_FILE_SERVER "fileServer"

void testFromJava(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/euhat/euhatexpert/OpFileJni");
    jclass classSaved = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    jmethodID construct = env->GetMethodID(classSaved, "<init>", "()V");

    jobject obj = env->NewObject(classSaved, construct);
    jobject objSaved = (jobject)env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);

    // need not global ref jmethodID
    jmethodID methodCallFromCpp = env->GetMethodID(classSaved, "callFromCpp", "(Ljava/lang/String;I)Ljava/lang/String;");

    jstring message = env->NewStringUTF("调用成功了");
    jstring jResult = (jstring)env->CallObjectMethod(objSaved, methodCallFromCpp, message, 1);

    const char *result = env->GetStringUTFChars(jResult, JNI_FALSE);
    env->ReleaseStringUTFChars(jResult, result);

    env->DeleteLocalRef(message);

    env->DeleteGlobalRef(objSaved);

    env->DeleteGlobalRef(classSaved);
}

class EuhatApp
{
public:
    string getDbPath()
    {
        string path = "/sdcard/Android/data/com.euhat.euhatexpert/";
        mkdir(path.c_str(), 0777);
        string dbDir = path + "euhat/";
        mkdir(dbDir.c_str(), 0777);
        string fileDir = path + "files/";
        mkdir(fileDir.c_str(), 0777);
        return dbDir + DEFAULT_SQLITE_DB_FILENAME;
    }

    void openDb()
    {
        if (NULL != db_.get())
            return;

        db_.reset(new DbOpSqlite());

        db_->openFileDbEncrypted(getDbPath().c_str());
    }

    void run(int port, const char *visitCode)
    {
        openDb();

        DbOpIni ini(db_.get());
        fmGenAndWriteCert(ini, JY_TCP_INI_SERVER_CERT_KEY, 512);

        fmSrv_.reset(new FileManServer(port));
        fmSrv_->setDbOp(db_.get());
        fmSrv_->visitCode_ = visitCode;
        fmSrv_->start();
    }

    unique_ptr<DbOpSqlite> db_;
    unique_ptr<FileManServer> fmSrv_;
};

unique_ptr<EuhatApp> theApp;

extern "C" JNIEXPORT jstring JNICALL Java_com_euhat_euhatexpert_MainActivity_getBuildTag(
        JNIEnv* env,
        jobject) {
    return env->NewStringUTF(BUILD_TAG);
}

extern "C" JNIEXPORT jstring JNICALL Java_com_euhat_euhatexpert_MainActivity_getVisitCode(
        JNIEnv* env,
        jobject,
        jstring jMac) {

    const char *mac = env->GetStringUTFChars(jMac, JNI_FALSE);
    string visitCode;

    do {
        theApp.reset(new EuhatApp());

        theApp->openDb();
        DbOpIni ini(theApp->db_.get());

        if (isMacHddChanged(ini, mac)) {
            theApp.reset();
            unlink(theApp->getDbPath().c_str());
        } else {
            int configCount = theApp->db_->selectCount("select count(*) from Config;");
            if (configCount < 0)
            {
                theApp.reset();
                unlink(theApp->getDbPath().c_str());
            } else {
                visitCode = ini.readStr(INI_SECTION_FILE_SERVER, "visitCode", "");
                break;
            }
        }
    } while (1);

    env->ReleaseStringUTFChars(jMac, mac);

    return env->NewStringUTF(visitCode.c_str());
}

extern "C" JNIEXPORT jint JNICALL Java_com_euhat_euhatexpert_MainActivity_startServer(
    JNIEnv *env,
    jobject pThis, jint port, jstring jVisitCode) {

    testFromJava(env);

    theApp.reset(new EuhatApp());
    theApp->openDb();

    const char *visitCode = env->GetStringUTFChars(jVisitCode, JNI_FALSE);

    DbOpIni ini(theApp->db_.get());
    ini.write(INI_SECTION_FILE_SERVER, "visitCode", visitCode);

    theApp->run((int)port, visitCode);
    env->ReleaseStringUTFChars(jVisitCode, visitCode);

    return 1;
}

extern "C" JNIEXPORT jint JNICALL Java_com_euhat_euhatexpert_MainActivity_stopServer(
        JNIEnv *en,
        jobject pThis) {

    theApp.reset();
    return 1;
}
