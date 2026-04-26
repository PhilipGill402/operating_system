#include <unistd.h>

#include <stdio.h>

void* brk(void* new_addr) {
    void* ret = (void*)__sys1(SYS_BRK, (uint32_t)new_addr);
    
    return ret;
}

uint32_t chdir(const char* path) {
    return __sys1(SYS_CHDIR, (uint32_t)path);
}

uint32_t execve(const char* path, const char* argv[]) { 
    return __sys2(SYS_EXECVE, (uint32_t)path, (uint32_t)argv);
}

__attribute__((noreturn))
void exit(int32_t error_code) {
    __sys1(SYS_EXIT, error_code); 

    for (;;) {}
}

uint32_t fork() {
    return __sys0(SYS_FORK); 
}

uint32_t getcwd(char* buffer, size_t size) {
    return __sys2(SYS_GETCWD, (uint32_t)buffer, size);
}

uint32_t getpid() {
    return __sys0(SYS_GETPID);
}

uint32_t read(uint32_t fd, char* buffer, size_t count) {
    return __sys3(SYS_READ, fd, (uint32_t)buffer, (uint32_t)count);
}

void* sbrk(size_t increment) {
    void* old_break = brk(0);
    uint32_t new_break = (uint32_t)old_break + increment; // get current heap break
    
    if ((increment > 0 && new_break < old_break) || (increment < 0 && new_break > old_break)) {
        return NULL;
    }
    
    void* new_base = brk((void*)new_break);
    if (!new_base) return NULL;

    return old_break;
}

uint32_t write(uint32_t fd, const char* buffer, size_t count) {
    return __sys3(SYS_WRITE, fd, (uint32_t)buffer, (uint32_t)count);
}


