#ifndef __MONO_IO_H__
#define __MONO_IO_H__

#include <mono/utils/mono-publib.h>

MONO_BEGIN_DECLS

typedef enum {
	MONO_IO_PATH_READ = 0x00000000,
	MONO_IO_PATH_WRITE = 0x00000001,
	MONO_IO_PATH_EXECUTE = 0x00000002
} MonoIoPathPermission;

typedef void (*MonoIoFreeFunc) (void *mem);
typedef char *(*MonoIoTranslateVirtualPathToRealPathFunc) (const char *src);
typedef char *(*MonoIoTranslateRealPathToVirtualPathFunc) (const char *src);
typedef MonoIoPathPermission (*MonoIoGetVirtualPathPermissionFunc) (const char *src);

typedef struct {
	MonoIoTranslateVirtualPathToRealPathFunc virtual_to_real;
	MonoIoTranslateRealPathToVirtualPathFunc real_to_virtual;
	MonoIoGetVirtualPathPermissionFunc virtual_path_permissions;
	MonoIoFreeFunc free;
} MonoIoVirtualCallbacks;


void mono_io_init (void);
void mono_io_install_virtual_callbacks (MonoIoTranslateVirtualPathToRealPathFunc vtr_func, MonoIoTranslateRealPathToVirtualPathFunc rtv_func, MonoIoGetVirtualPathPermissionFunc vpp_func, MonoIoFreeFunc free);
MonoIoVirtualCallbacks *mono_io_get_virtual_callbacks (void);

MONO_END_DECLS

#endif
