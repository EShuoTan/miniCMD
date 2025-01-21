#include "demo.h"
#include "stdio.h"

// These two commands are added in miniCMD_config.h
void fun1(int a, uint8_t b, float c)
{
    printf("fun1: %d, %d, %f\n", a, b, c);
}
float fun2(float a, int b, int8_t c)
{
    printf("fun2: %f, %d, %d\n", a, b, c);
    return a;
}

// Example of a command being added to an external file
#include "miniCMD_makeCmd.h"
int fun3(void)
{
    printf("fun3\n");
    return 114514;
}
MAKE_CMD(int,fun3,void);


#include "miniCMD.h"
char input_buf[200];
int main()
{
    miniCMD_init();
    
    char* test_list[]={
        "fun1(1,2,3)",
        "fun2(1.1, 2.2, 3.3)",
        " fun3  ( ) ;  ",
        "help()"};
    for (int i=0; i<sizeof(test_list)/sizeof(char*); ++i)
    {
        printf("\n%s\n",test_list[i]);
        miniCMD_run(test_list[i]);
    }

    while (1)
    {
        gets(input_buf);
        miniCMD_run_s(input_buf, sizeof(input_buf)); 
    }
    return 0;
}
