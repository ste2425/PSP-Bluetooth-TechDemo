#include "kernel_functions.h"


// fs_driver functions
int pspIoOpenDir(const char *dirname);
int pspIoReadDir(SceUID dir, SceIoDirent *dirent);
int pspIoCloseDir(SceUID dir);
int pspIoMakeDir(const char *dir, SceMode mode);
int pspIoRemoveDir(const char *path);
int pspIoOpenFile(const char *file, int flags, SceMode mode);
int pspIoReadFile(SceUID file, void *data, SceSize size);
int pspIoWriteFile(SceUID file, void *data, SceSize size);
int pspIoCloseFile(SceUID file);
int pspIoLseek(SceUID file, SceOff offset, int whence);
int pspIoLseek32(SceUID file, SceOff offset, int whence);
int pspIoGetstat(const char *file, SceIoStat *stat);
int pspIoRename(const char *oldname, const char *newname);
int pspIoRemoveFile(const char *file);
int pspIoDevctl(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen);

//sio
void pspUARTInit(int baud);
void pspUARTWrite(int ch);
void pspUARTPrint(const char *str);
int pspUARTRead(void);
void pspUARTTerminate();
void pspUARTHandlers();
void pspUARTWaitForData(unsigned int timeout);
int pspUARTAvailable(void);
void pspUARTResetRingBuffer();
int pspUARTPeek(int index);
void pspUARTWriteBuffer(const char *data, int len);