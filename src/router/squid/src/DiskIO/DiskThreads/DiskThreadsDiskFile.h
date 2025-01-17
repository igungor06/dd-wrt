/*
 * Copyright (C) 1996-2022 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

/* DEBUG: section 79    Disk IO Routines */

#ifndef SQUID_DISKTHREADSDISKFILE_H
#define SQUID_DISKTHREADSDISKFILE_H

#include "cbdata.h"
#include "DiskIO/DiskFile.h"
#include "DiskThreads.h"
#include "typedefs.h" //for DWCB

class DiskThreadsDiskFile : public DiskFile
{
    CBDATA_CLASS(DiskThreadsDiskFile);

public:
    DiskThreadsDiskFile(char const *path);
    ~DiskThreadsDiskFile();
    virtual void open(int flags, mode_t mode, RefCount<IORequestor> callback);
    virtual void create(int flags, mode_t mode, RefCount<IORequestor> callback);
    virtual void read(ReadRequest *);
    virtual void write(WriteRequest *);
    virtual void close();
    virtual bool error() const;
    virtual int getFD() const { return fd;}

    virtual bool canRead() const;
    virtual bool canWrite() const;
    virtual bool ioInProgress() const;

private:
#if ASYNC_READ

    static AIOCB ReadDone;
#else

    static DRCB ReadDone;
#endif
#if ASYNC_WRITE

    static AIOCB WriteDone;
#else

    static DWCB WriteDone;
#endif

    int fd = -1;
    bool errorOccured = false;
    char const *path_ = nullptr;
    size_t inProgressIOs = 0;
    static AIOCB OpenDone;
    void openDone(int fd, const char *buf, int aio_return, int aio_errno);
    RefCount<IORequestor> ioRequestor;
    void doClose();

    void readDone(int fd, const char *buf, int len, int errflag, RefCount<ReadRequest> request);
    void writeDone(int fd, int errflag, size_t len, RefCount<WriteRequest> request);
};

#include "DiskIO/ReadRequest.h"

template <class RT>
class IoResult
{
    CBDATA_CLASS(IoResult);

public:
    IoResult(RefCount<DiskThreadsDiskFile> aFile, RefCount<RT> aRequest) : file(aFile), request(aRequest) {}

    RefCount<DiskThreadsDiskFile> file;
    RefCount<RT> request;
};

template <class RT>
IoResult<RT>
IOResult(RefCount<RT> aRequest, RefCount<DiskThreadsDiskFile> aFile) { return IoResult<RT>(aFile, aRequest);}

#endif /* SQUID_DISKTHREADSDISKFILE_H */

