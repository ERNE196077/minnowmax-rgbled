#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main (){

u_int32_t val = 0xFED0C000 + 0x110;

printf("%04x",val);

return 0;
}
