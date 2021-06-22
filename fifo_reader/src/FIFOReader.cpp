#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FIFOReader.h"

FIFOReader::FIFOReader(const char *path){
    m_path = path;
}

int FIFOReader::init_fifo(){
    int fd;

    fd = open(m_path, O_RDONLY | O_NONBLOCK);
    if(fd == -1){
        printf("open error:%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int FIFOReader::read_data(){
    char buffer[BUF_SIZE];
    int bytes_read = -1;

    memset(buffer, 0, BUF_SIZE);
    bytes_read = read(m_fd, buffer, BUF_SIZE);
    if(bytes_read == -1){
        printf("read error:%s\n", strerror(errno));
        return -1;
    }
    printf("data:%s\n", buffer);
    
    return 0;    
}