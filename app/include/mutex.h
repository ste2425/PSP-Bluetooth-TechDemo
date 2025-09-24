#pragma once

/*


static SceUID client_list_mtx = 0;

    client_list_mtx = sceKernelCreateMutex("ftpsp_list_mutex", PSP_MUTEX_ATTR_FIFO, 0, nullptr);
    DEBUG("Client list mutex UID: 0x%08X\n", client_list_mtx);


        sceKernelDeleteMutex(client_list_mtx);


     Add the client at the front of the client list 
    sceKernelLockMutex(client_list_mtx, 1, nullptr);


    sceKernelUnlockMutex(client_list_mtx, 1);
*/


#define PSP_MUTEX_ATTR_FIFO 0

#ifdef __cplusplus
extern "C" {
#endif

int sceKernelCreateMutex(const char *name, uint attributes, int initial_count, void *options);
int sceKernelDeleteMutex(int mutexId);
int sceKernelLockMutex(int mutexId, int count, uint *timeout);
int sceKernelUnlockMutex(int mutexId, int count);

#ifdef __cplusplus
}
#endif
