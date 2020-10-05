#pragma once

#include <common/OpCommon.h>
#include <common/JyMutex.h>

class EuhatFileHandles
{
public:
    struct FileEntry
    {
        enum Type
        {
            TypeNormalFile,
            TypeClipboard,
            TypePicture,
            TypeVideo,
            TypeDownload
        };
        string key_;
        string mode_;
        int type_;
        FILE *fp_;
    };

    static EuhatFileHandles *getInstance();

    FILE *fopen(const char *filePath, const char *mode);
    string fgetPath(FILE *fp);
    void fclose(FILE *fp);

    JyHistoryList<FileEntry, string> files_;
};

class WhFileGuard
{
public:
    WhFileGuard(FILE *fp)
    {
        fp_ = fp;
    }
    ~WhFileGuard()
    {
        if (NULL != fp_)
            EuhatFileHandles::getInstance()->fclose(fp_);
    }
    FILE *fp_;
};