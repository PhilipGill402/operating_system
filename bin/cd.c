#include <unistd.h>

int main(int argc, char* argv[]) {
    char* path;
    if (argc == 0)
        path = NULL;
    else
        path = argv[1];

    int ret = chdir(path);
    
    return ret;
}
