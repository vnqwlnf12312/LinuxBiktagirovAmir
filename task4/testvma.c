#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    int fd;
    
    fd = open("/proc/mmaneg", O_WRONLY);
    
    char* data;
    asprintf(&data, "listvma");
    write(fd, data, strlen(data));
    
    long unsigned a = 0;
    long unsigned* b = malloc(sizeof(long unsigned));
    *b = 0;

    asprintf(&data, "writeval %lx 228", &a);
    write(fd, data, strlen(data));

    asprintf(&data, "writeval %lx 229", b);
    write(fd, data, strlen(data));

    printf("a = %lu, b = %lu\n", a, *b);

    close(fd);
    return 0;
}