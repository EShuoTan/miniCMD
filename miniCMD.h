/**
 * @version 1.0
 * @file miniCMD.h
 * @link https://github.com/EShuoTan/miniCMD
 * @brief 提供实现miniCMD控制台的接口
 * @author TanES (tan_163mail@163.com)
 */
#ifndef MINICMD_H
#define MINICMD_H
void miniCMD_init(void);
int miniCMD_run(const char *str);
int miniCMD_run_s(char *str, unsigned int size);
#endif
