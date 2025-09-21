#
# stub normaly Kernel to Kernel link
#
	.set noreorder

#include "pspstub.s"

STUB_START "sceSuspendForKernel",0x40010000,0x000d0005 
  STUB_FUNC 0x91a77137,sceKernelRegisterSuspendHandler 
  STUB_FUNC 0xb43d1a8c,sceKernelRegisterResumeHandler
STUB_END 