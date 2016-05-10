#ifndef __lib_h__
#define __lib_h__

int vsnprintf(char *buf, int n, const char *fmt, ...);
int printf(const char *format,...);
int scr_clr(void);
int fork();
void p_exit();
void sleep(int);
void createSem(int);
void lockSem(int);
void unlockSem(int);
void destorySem(int);
#endif
