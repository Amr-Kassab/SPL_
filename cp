#include <unistd.h>
#include <fcntl.h>

char buf[100000];
int cp_main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with cp_main() as the main function of your program.
    
    int fd = open(argv[1], O_RDONLY);
    
    if(fd < 0)
        exit(-1);
    
    int fd2 = creat(argv[2], 00700);
    int size = read(fd, buf, 100000);
    write(fd2, buf,size);
    
    return 0;
}