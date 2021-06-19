#ifndef REMOTE_UTILS_H
#define REMOTE_UTILS_H
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/uio.h>
#include <stdio.h>
#include <typeinfo>


#include "logprint.hpp"

int readNBytes(pid_t remote_pid, void *rbase, void *lbase, unsigned int n);

template<typename T>
int readData(pid_t remote_pid, void *rbase, T& t){
        int retval = -1;
        size_t len = sizeof(T);

        struct iovec local[1];
        local[0].iov_base = (void*)&t;
        local[0].iov_len = len;

        struct iovec remote[1];
        remote[0].iov_base = rbase;
        remote[0].iov_len = len;
        //retval = syscall(270, remote_pid, local, 1, remote, 1, 0);
        retval = syscall(376, remote_pid, local, 1, remote, 1, 0);
        if (retval < 0) {
            DEBUG_PRINT("[!] failed to process_vm_readv, rbase:%p, retval:%d, errno:%d, errmsg:%s\n", rbase, retval, errno, strerror(errno));
            return -1;
        }
        //DEBUG_PRINT("readData %s rbase:%p lbase:%p len:%d success", typeid(T).name(), rbase, (void*)&t, len);
        return 0; // read success  
}

#endif