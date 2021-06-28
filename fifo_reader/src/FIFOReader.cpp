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

    if(mkfifo(m_path, O_CREAT | O_EXCL) < 0 && (errno != EEXIST)){
        printf("cannot create fifo file\n");
        return -1;
    }
    printf("mkfifo success:%s\n", m_path);

    fd = open(m_path, O_RDONLY | O_NONBLOCK);
    if(fd == -1){
        printf("open error:%s\n", strerror(errno));
        return -1;
    }
    m_fd = fd;
    return 0;
}

int FIFOReader::read_data(){
    char buffer[BUF_SIZE];
    int bytes_read = -1;

    

    FILE *fp = fdopen(m_fd, "r");
    if(!fp){
        printf("fdopen failed:%s\n", strerror(errno));
        return -1;
    }
    memset(buffer, 0, BUF_SIZE);
    fgets(buffer, BUF_SIZE, fp);
    bytes_read = strlen(buffer);
    printf("bytes_read:%d\n", bytes_read);
    //bytes_read = read(m_fd, buffer, BUF_SIZE);
    //printf("read return %d\n", bytes_read);
    // if(bytes_read == -1){
    //     printf("read error:%s\n", strerror(errno));
    //     return -1;
    // }
    printf("data:%s\n", buffer);
    
    return 0;    
}