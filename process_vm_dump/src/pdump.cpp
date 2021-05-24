#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <android/log.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/uio.h>

#define MAXBUF    8192
#define DEBUG	  1

/* User defined values */
#define MAPPREFIX "map-"
#define MEMPREFIX "mem-"
#define BINSUFFIX ".bin"

#define FILTER      1
#define RWX_FILTER  "rw-"
#define INTERVALS   1
#define SLEEP_MICRO 10000

/* End user defined variables */

#define debug(x...) { \
fprintf(stderr, x); \
fflush(stderr); \
}\

#define fatal(x...) { \
fprintf(stderr, "[-] ERROR: " x); \
exit(1); \
}\

#define LOG_TAG "WR"
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

const char *output_path = "/data/local/tmp/tdump.bin";
FILE *fp = NULL;

/*将大写字母转换成小写字母*/
int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + 'a' - 'A';
    }
    else
    {
        return c;
    }
}

//将十六进制的字符串转换成整数
u_int64_t htoi(char s[])
{
    int i;
    u_int64_t n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}


//多次dump使用 每次从文件尾部写入
extern "C" void dumpToSdcard_fromEnd(char *data,int length)
{
/*
    LOGD("dumpToSdcard_fromEnd 1");
    char *name = "tdump.bin";

    char savename[256]={0};
    sprintf(savename,"/sdcard/Pictures/%s",name);
    FILE* file = fopen(savename, "ab+");
    LOGD("dumpToSdcard_fromEnd 2 %s",savename);
    if (file == NULL) {
        memset(savename,0,256);
        sprintf(savename,"/mnt/sdcard/Pictures/%s",name);
        file = fopen(savename, "wb+");
        if (file == NULL) {
            memset(savename,0,256);
            sprintf(savename,"/storage/sdcard/Pictures/%s",name);
            file = fopen(savename, "wb+");
        }
    }
*/
    LOGD("save data, base=%lx, length=%d=%lx",data, length, length);
    fwrite(data,1,length,fp);
}

extern "C" void dump_memory_s2(FILE *mapfile, pid_t child, void* start, void* end) {
    char *filepath, tmp[MAXBUF], procmem[MAXBUF];
    if(child == 0 || start==0 || end ==0)
    {
        return;
    }
    u_int64_t st, en, len;
    uint64_t rbase = 0;
    int dumpfile, retval;
    len = (u_int64_t)end - (u_int64_t)start;
    fp = fopen(output_path, "wb+");
    if(!fp){
        printf("fopen file failed:%s", output_path);
        return;
    }
    while (fgets(tmp, MAXBUF, mapfile)) {
        char line[MAXBUF], perms[MAXBUF], r, w, x;
        char *writeptr;
        int is_access = 1;

        if (sscanf(tmp, "%lx-%lx %c%c%cp", &st, &en, &r, &w, &x) != 5) {
            LOGD("[!] Parse error in /proc/%d/maps: %s", child, tmp);
            continue;
        }

        if(en<=(uint64_t)start){
            //LOGD("skip piece %lx-%lx", st, en);
            continue;
        }
        
        if(st<=(uint64_t)start && en>=(uint64_t)start){
            LOGD("found first piece, start addr locate at piece %lx-%lx", st, en);
            if(en>=(uint64_t)end){
                LOGD("end addr locate in same piece, so dump %lx-%lx", (uint64_t)start, (uint64_t)end);
                len = (uint64_t)end - (uint64_t)start;
                rbase = (uint64_t)start;
            }
            else{
                LOGD("end addr locate in another piece, so dump %lx-%lx", (uint64_t)start, en);
                len = en - (uint64_t)start;
                rbase = (uint64_t)start;
            }
        }
        else if(st>=(uint64_t)start && en<=(uint64_t)end){
            LOGD("dump normal piece %lx-%lx", st, en);
            len = en - st;
            rbase = st;
        }
        else if(st<=(uint64_t)end && en>=(uint64_t)end){
            LOGD("found last piece, dump %lx-%lx", st, (uint64_t)end);
            len = (uint64_t)end - st;
            rbase = st;
        }
        else if(st >= (uint64_t)end){
            LOGD("break, no need to iterate piece");
            break;
        }
/*
        if(st<(u_int64_t)start)
        {
            continue;
        }

        if(st < (u_int64_t)end )
        {
            printf("%lx %lx\n", st, en);
        } else
        {
            break;
        }

        len = en - st;
*/

        if (FILTER && (r != RWX_FILTER[0] )) {
            printf("[!] line %s %c%c%c\n", line, r, w, x);
            is_access = 0;
        } else
        {
            is_access = 1;
        }

        if(is_access == 1)
        {
            struct iovec local[1];
            local[0].iov_base = calloc(len, sizeof(char));
            local[0].iov_len = len;

            struct iovec remote[1];
            remote[0].iov_base = (void *) rbase;
            remote[0].iov_len = len;
            printf("[!] prepare to process_vm_readv %d (%lx) == %x\n",child,  (unsigned long)remote[0].iov_base, remote[0].iov_len);
            //retval = process_vm_readv(child, local, 1, remote, 1, 0);
            retval = syscall(270, child, local, 1, remote, 1, 0);
            printf("[!] end to process_vm_readv %x %d\n",  retval,errno);
            if (retval < 0) {
                printf("[!] failed to process_vm_readv(%d) == %d\n", retval, errno);
                continue;
            }

            fwrite((char*)local[0].iov_base, 1, len, fp);
            //dumpToSdcard_fromEnd((char*)local[0].iov_base,len);
        } else{
            void * zerobuff = calloc(len, sizeof(char));
            memset(zerobuff,0,len);
			//非wr填0
            printf("[!] not readable write zero %lx %lx\n",st, en);
            fwrite((char*)zerobuff, 1, len, fp);
            //dumpToSdcard_fromEnd((char*)zerobuff,len);
        }

    }
    if(fp){
        fclose(fp);
    }

    fseek(mapfile, 0, SEEK_SET);
}

extern "C" void dump_wrapper2(pid_t child,void* start, void* end) {
    FILE *mapfile;
    int memfd;
    char tmp[MAXBUF];
    int count = 0;

    sprintf(tmp, "/proc/%d/maps", child);
    mapfile = fopen(tmp, "r");

    if (!mapfile) {
        printf("/proc/%d/maps could not be open\n", child);
    }

    dump_memory_s2(mapfile, child, start,end);
}

int main(int argc, char *argv[]){
    pid_t target_pid;

    if(argc != 4){
        printf("Usage: %s <pid> <start_addr> <end_addr>, for example: ./process_vm_dump 987 0xdeadbeaf 0xffffffff\n", argv[0]);
        return -1;
    }

    target_pid = atoi(argv[1]);
    u_int64_t start = htoi(argv[2]);
    u_int64_t end = htoi(argv[3]);
    printf("dump start target_pid=%d, start=0x%lx end=0x%lx total len=0x%lx=%llu, output file is:%s\n", 
        target_pid, start, end, end - start,  end - start, output_path);
    if(start >= end){
        printf("start cannot large than end\n");
        return -1;
    }
    //dump_memory_s(target_pid,(void*)start,(void*)end);
    dump_wrapper2(target_pid,(void*)start,(void*)end);
    //printf("sdump pid: %d start:%llx end:%llx \n", target_pid,start,end);

    printf("dump over \n");
    return 0;
}