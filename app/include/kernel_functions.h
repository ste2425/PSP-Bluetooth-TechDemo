#pragma once

#if defined (__cplusplus)
extern "C" {
#endif

#include <pspctrl.h>
#include <pspiofilemgr.h>

// Kernel function prototypes

// fs_driver functions
extern int pspIoOpenDir(const char *dirname);
extern int pspIoReadDir(SceUID dir, SceIoDirent *dirent);
extern int pspIoCloseDir(SceUID dir);
extern int pspIoMakeDir(const char *dir, SceMode mode);
extern int pspIoRemoveDir(const char *path);
extern int pspIoOpenFile(const char *file, int flags, SceMode mode);
extern int pspIoReadFile(SceUID file, void *data, SceSize size);
extern int pspIoWriteFile(SceUID file, void *data, SceSize size);
extern int pspIoCloseFile(SceUID file);
extern int pspIoLseek(SceUID file, SceOff offset, int whence);
extern int pspIoLseek32(SceUID file, SceOff offset, int whence);
extern int pspIoGetstat(const char *file, SceIoStat *stat);
extern int pspIoRename(const char *oldname, const char *newname);
extern int pspIoRemoveFile(const char *file);
extern int pspIoDevctl(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

//sio
void pspUARTInit(int baud);
void pspUARTWrite(int ch);
void pspUARTPrint(const char *str);
int pspUARTRead(void);
void pspUARTTerminate();
void pspUARTWaitForData(unsigned int timeout);
int pspUARTAvailable(void);
void pspUARTResetRingBuffer();
void pspUARTWriteBuffer(const char *data, int len);

#if defined (__cplusplus)
}
#endif
