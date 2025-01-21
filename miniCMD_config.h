/** miniCMD V1.0
 * @file miniCMD_config.h UTF-8
 */
#include "miniCMD_makeCmd.h"
/* 移植时需要实现宏 CMD_print(...)
 * CMD_print(...) needs to be implemented for porting
 */
#include "stdio.h"
#define CMD_print(...) printf(__VA_ARGS__)


/* 建议把函数对应的头文件 include 过来，以提供类型检查
 * Suggest including the .h file corresponding to the function, to provide type checking
 * */

// example
#include "demo.h"
MAKE_CMD(void,fun1,int,uint8_t,float);
MAKE_CMD(float,fun2,float a,int b,int8_t c);


