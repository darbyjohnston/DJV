#include <stdio.h>

int main(int argc, char ** argv)
{
    int r = 1;
    
    do
    {
        if (argc != 5)
        {
            printf("Usage: sequence (name) (start) (end) (extension)\n");
            
            break;
        }

        char * name      = argv[1];
        int    start     = atoi(argv[2]);
        int    end       = atoi(argv[3]);
        char * extension = argv[4];
        
        int i;
        
        for (i = start; i <= end; ++i)
        {
            FILE * f = 0;
            char   buf [256];
            
            snprintf(buf, 256, "%s.%d.%s", name, i, extension);
        
            f = fopen(buf, "w");
            
            if (! f)
            {
                printf("Error writing: %s\n", buf);
                
                break;
            }
            
            fclose(f);
        }
        
        r = 0;
    }
    while (0);
    
    return r;
}

