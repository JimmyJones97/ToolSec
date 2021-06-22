#include <stdio.h>
#include <unistd.h>

#include "FIFOReader.h"

int main(int argc, char* argv[]){
    FIFOReader fifo_reader("/data/local/tmp/test_fifo");
    if(0 != fifo_reader.init_fifo()){
        printf("init reader fifo failed\n");
        return -1;
    }
    printf("init reader fifo ok\n");
    while (true)
    {
        printf("read data...\n");
        // todo: 阻塞
        if(0 != fifo_reader.read_data()){
            printf("read data failed\n");
            break;
        }
        sleep(1);
    }

    printf("while break\n");
    // unlink("/data/local/tmp/test_fifo");

    return 0;
    
}