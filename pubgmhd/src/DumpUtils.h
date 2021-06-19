#ifndef DUMP_UTILS_H
#define DUMP_UTILS_H

#ifndef DUMP_PUBGMHD_CLASS_H
#define DUMP_PUBGMHD_CLASS_H

#include <map>
#include <vector>
#include <string>

#include "EngineClasses.h"

class DumpPubgmhdClass {
private:
    DumpPubgmhdClass(pid_t pid, unsigned long base)
        :remote_pid(pid),
        libUE4_base(base)
    {}
    
    // C++ 03
    // ========
    // Don't forget to declare these two. You want to make sure they
    // are unacceptable otherwise you may accidentally get copies of
    // your singleton appearing.
    DumpPubgmhdClass(DumpPubgmhdClass const&);          // Don't Implement
    void operator=(DumpPubgmhdClass const&);      // Don't Implement
    
    pid_t remote_pid;
    unsigned long libUE4_base;
    // <className: dumped>
    std::map<std::string, int> mDumpedClass;
    
    
public:
    static DumpPubgmhdClass& getInstance(pid_t pid, unsigned long base){
        static DumpPubgmhdClass instance(pid, base); // 单例
        return instance;
    }
    
    // 层序遍历，不要深层次的去导出每一个ObjectProperty，有的太多坑
    int sequence_traverse_dump(UObject* q, int dig_lvl=0);

    // dump一个指定的UObject
    int dump_specified_uobject(UObject *p_UObj);
    
    
};

#endif

#endif