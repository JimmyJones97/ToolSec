#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FIFOWriter.h"
#include "logprint.hpp"

FIFOWriter::FIFOWriter(const char *path){
    m_path = path;
}

int FIFOWriter::init_fifo(){
    int fd;

    fd = open(m_path, O_WRONLY | O_NONBLOCK);
    if(fd == -1){
        printf("open error:%s\n", strerror(errno));
        return -1;
    }
    m_fd = fd;
    return 0; // success
}

int FIFOWriter::write_data(char *data, int size){
    int bytes_write = -1;

    bytes_write = write(m_fd, data, size);
    if(bytes_write == -1){
        printf("write error:%s\n", strerror(errno));
        return -1;
    }
    if(bytes_write != size){
        printf("Note: bytes_write=%d less than size=%d\n", bytes_write, size);
    }

    return 0;
}