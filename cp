
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define SIZE 100
char buf[SIZE];

int main(int argc, char *argv[]) {
    // Write your code here
    // Do not write a main() function. Instead, deal with cp_main() as the main function of your program.
    
    if(argc < 3)
        exit(-99);
    
    int fd = open(argv[1], O_RDONLY);
    
    if(fd < 0)
        exit(-1);
    
    int fd2 = creat(argv[2], 00700);
    
    if(fd2 < 0)
        exit(-1);
        
    int _read, _write;
    while((_read = read(fd, buf, SIZE)) > 0){
        
        _write = write(fd2, buf,_read);
        
        if(_write < _read)
        {
            exit(-2);
        }
    }
    
    return 0;
}
