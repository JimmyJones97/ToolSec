#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/uio.h>
#include <stdio.h>

#include "RemoteUtils.h"
#include "EngineClasses.h"
#include "logprint.hpp"


int readNBytes(pid_t remote_pid, void *rbase, void *lbase, unsigned int n){
        int retval = -1;

        struct iovec local[1];
        local[0].iov_base = lbase;
        local[0].iov_len = n;

        struct iovec remote[1];
        remote[0].iov_base = rbase;
        remote[0].iov_len = n;
        //printf("[!] prepare to process_vm_readv %d (%lx) == %x\n",child,  (unsigned long)remote[0].iov_base, remote[0].iov_len);
        //retval = process_vm_readv(child, local, 1, remote, 1, 0);

        //retval = syscall(270, remote_pid, local, 1, remote, 1, 0);

        retval = syscall(376, remote_pid, local, 1, remote, 1, 0);

        //printf("[!] end to process_vm_readv %x %d\n",  retval,errno);
        if (retval < 0) {
            DEBUG_PRINT("[!] failed to process_vm_readv, rbase:%p, retval:%d, errno:%d, errmsg:%s\n", rbase, retval, errno, strerror(errno));
            return -1;
        }
        
        return 0; // read success    
}

