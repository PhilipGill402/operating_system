#include <unistd.h>
#include <stdio.h>

void* brk(void* new_addr) {
    int32_t ret = __sys1(SYS_BRK, (uint32_t)new_addr);

    if (ret < 0) return NULL;
    
    return (void*)ret;
}

int32_t chdir(const char* path) {
    return (uint32_t)__sys1(SYS_CHDIR, (uint32_t)path);
}

int32_t execve(const char* path, const char* argv[]) { 
    return (uint32_t)__sys2(SYS_EXECVE, (uint32_t)path, (uint32_t)argv);
}

__attribute__((noreturn))
void exit(int32_t error_code) {
    __sys1(SYS_EXIT, error_code); 

    for (;;) {}
}

int32_t fork() {
    return (uint32_t)__sys0(SYS_FORK); 
}

int32_t getcwd(char* buffer, size_t size) {
    return (uint32_t)__sys2(SYS_GETCWD, (uint32_t)buffer, size);
}

int32_t getpid() {
    return (uint32_t)__sys0(SYS_GETPID);
}

int32_t read(uint32_t fd, char* buffer, size_t count) {
    return (uint32_t)__sys3(SYS_READ, fd, (uint32_t)buffer, (uint32_t)count);
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

int32_t write(uint32_t fd, const char* buffer, size_t count) {
    return (uint32_t)__sys3(SYS_WRITE, fd, (uint32_t)buffer, (uint32_t)count);
}


int32_t open(const char* name, uint32_t flags, mode_t mode) {
    return (uint32_t)__sys3(SYS_OPEN, (uint32_t)name, flags, (uint32_t)mode);
}

int32_t getdents(uint32_t fd, dirent_t* dents, uint32_t count) {
    return (uint32_t)__sys3(SYS_GETDENTS, fd, (uint32_t)dents, count);
}

int32_t waitpid(uint32_t pid, int* status, uint32_t options) {
    return __sys3(SYS_WAITPID, pid, (uint32_t)status, options);
}

int32_t close(uint32_t fd) {
    return __sys1(SYS_CLOSE, fd);
}

int32_t lseek(uint32_t fd, uint32_t offset) {
    return (int)__sys2(SYS_LSEEK, fd, offset);
}

int32_t kill(uint32_t pid, int32_t sig) {
    return (int)__sys2(SYS_KILL, pid, sig);
}

void* mmap(void* addr, uint32_t length, int32_t prot, int32_t flags, int32_t fd, uint32_t offset) {
    return (void*)__sys6(SYS_MMAP, addr, length, prot, flags, fd, offset);
}

int32_t munmap(void* addr, uint32_t length) {
    return __sys2(SYS_MUNMAP, (uint32_t)addr, length);
}

int32_t fb_info(fb_info_t* info) {
    return __sys1(SYS_FB_INFO, (uint32_t)info);
}

int32_t fb_flush(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    return __sys4(SYS_FB_FLUSH, x, y, width, height);
}

int32_t yield() {
    return __sys0(SYS_YIELD);
}

int32_t dup(uint32_t old_fd) {
    return __sys1(SYS_DUP, old_fd);
}

int32_t dup2(uint32_t old_fd, uint32_t new_fd) {
    return __sys2(SYS_DUP2, old_fd, new_fd);
}

int32_t mkdir(const char* path, uint32_t mode) {
    return __sys2(SYS_MKDIR, (uint32_t)path, mode);
}

