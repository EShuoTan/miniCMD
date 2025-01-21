/** miniCMD V1.0
 * @file miniCMD.h
 */
#ifndef MINICMD_H
#define MINICMD_H
extern void miniCMD_init(void);
int miniCMD_run(const char *str);
int miniCMD_run_s(char *str, unsigned int size);
#endif
