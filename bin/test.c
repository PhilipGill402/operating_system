#include <stdint.h>

int main() {
    char buffer[128];
    int32_t ret = read(0, buffer, 128);
}
