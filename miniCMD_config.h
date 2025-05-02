/**
 * @version 1.0
 * @file miniCMD_config.h
 * @link https://github.com/EShuoTan/miniCMD
 * @brief miniCMD的配置文件，在这个文件中添加命令
 * @note -移植时需要实现宏 CMD_print(...) 用于打印交互信息
 *       -建议 include 目标函数对应的头文件，以提供检查
 * @author TanES (tan_163mail@163.com)
 */
#include "miniCMD_makeCmd.h"
// 实现宏 CMD_print(...) 的示例：
#include "stdio.h"
#define CMD_print(...) printf(__VA_ARGS__)

/********** 在这里使用 MAKE_CMD 向miniCMD加入命令 **********/
// 示例
#include "demo.h"
MAKE_CMD(void, fun1, int, uint8_t, float);
MAKE_CMD(float, fun2, float a, int b, int8_t c);
