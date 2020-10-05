#include <jni.h>
#include <string>
#include <common/OpCommon.h>
#include <KotlinStub.h>
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

class EuhatApp
{
public:
    string getDbPath()
    {
        mkdir(EUHAT_PATH_BASE, 0777);
        mkdir(EUHAT_PATH_DB, 0777);
        mkdir(EUHAT_PATH_FILES, 0777);
        return EUHAT_PATH_DB DEFAULT_SQLITE_DB_FILENAME;
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

    KotlinStub::getInstance()->init(env, pThis);

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
