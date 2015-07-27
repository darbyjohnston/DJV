#define _GNU_SOURCE

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct linux_dirent64
{
   ino64_t        d_ino;
   off64_t        d_off;
   unsigned short d_reclen;
   unsigned char  d_type;
   char           d_name[];
};

int main(int argc, char ** argv)
{
    int    r       = 1;
    char * buf     = 0;
    int    bufSize = 0;
    int    fd      = 0;
    
    do
    {
        char * path = argc > 1 ? argv[1] : ".";
    
        fd = open(path, O_RDONLY | O_DIRECTORY);
        
        if (-1 == fd)
        {
            printf("Cannot open: %s\n", path);
            
            break;
        }
        
        bufSize = 1024 * 1024;
        
        buf = malloc(bufSize);
        
        if (! buf)
        {
            printf("Cannot allocate buffer");
            
            break;
        }
        
        while (1)
        {
            int readCount = 0;
            
            int i;
            
            readCount = syscall(SYS_getdents64, fd, buf, bufSize);
                                    
            if (-1 == readCount)
            {
                printf("Error reading: %s\n", path);
                
                break;
            }
            
            if (0 == readCount)
                break;
            
            for (i = 0; i < readCount;)
            {
                struct linux_dirent64 * de = (struct linux_dirent64 *)(buf + i);
                
                if (de->d_ino != DT_UNKNOWN)
                {
                    printf("%s\n", de->d_name);
                }
                
                i += de->d_reclen;
            }
        }
        
        r = 0;
    }
    while (0);
    
    close(fd);
    free(buf);
    
    return r;
}

