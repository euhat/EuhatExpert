#include <EuhatPreDef.h>
#include "EuhatFileHandles.h"
#include <EuhatPostDef.h>

EuhatFileHandles *gEuhatFileHandles = NULL;

EuhatFileHandles *EuhatFileHandles::getInstance()
{
    if (NULL == gEuhatFileHandles)
        gEuhatFileHandles = new EuhatFileHandles();
    return gEuhatFileHandles;
}

FILE *EuhatFileHandles::fopen(const char *filePath, const char *mode) {

    if (NULL == filePath || filePath[0] == 0)
        return NULL;

    string path = whGetAbsolutePath(filePath);

    DBG(("kotlin fopen [%s]\n", path.c_str()));

    FILE *fp = whFopen(path.c_str(), mode);
    if (NULL == fp)
        return fp;

    WhMutexGuard g(files_.cs_);
    FileEntry *entry = files_.newEntry(path);
    if (strIsStartWith(path.c_str(), EUHAT_PATH_CLIPBOARD))
        entry->type_ = FileEntry::TypeClipboard;
    else if (strIsStartWith(path.c_str(), EUHAT_PATH_DB)) {
        whFClose(fp);
        return NULL;
    } else
        entry->type_ = FileEntry::TypeNormalFile;
    entry->fp_ = fp;
    entry->mode_ = mode;
    return fp;
}

string EuhatFileHandles::fgetPath(FILE *fp)
{
    WhMutexGuard g(files_.cs_);
    list<FileEntry>::iterator it = find_if(files_.list_.begin(), files_.list_.end(), [fp](FileEntry &entry) { return fp == entry.fp_; });
    if (files_.list_.end() == it) {
        DBG(("kotlin stub fgetPath param fp is null.\n"));
        return "";
    }
    return it->key_;
}

void EuhatFileHandles::fclose(FILE *fp)
{
    WhMutexGuard g(files_.cs_);
    list<FileEntry>::iterator it = find_if(files_.list_.begin(), files_.list_.end(), [fp](FileEntry &entry) { return fp == entry.fp_; });
    if (files_.list_.end() == it) {
        DBG(("kotlin stub fclose param fp is null.\n"));
        return;
    }
    whFClose(fp);
    if (it->type_ == FileEntry::TypeClipboard &&
            (strstr(it->mode_.c_str(), "w") != NULL || strstr(it->mode_.c_str(), "a") != NULL)) {
        int64_t size = whGetFileSize(it->key_.c_str());
        if (size > 1024 * 1024 * 5)
        {
            DBG(("kotlin stub fclose clipboard size is too large:%lld.\n", size));
        } else {
            unsigned int len;
            unique_ptr<char[]> buf(memFromWholeFile(it->key_.c_str(), &len));

            whUpdateClipboard(buf.get());
        }
    }
    files_.list_.erase(it);
}