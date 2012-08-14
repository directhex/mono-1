/*
 * io.c: Routines for accessing / installing the virtual path support
 *
 * Authors:
 *   Geoff Norton (geoff_norton@playstaion.sony.com)
 *
 * Copyright 2012 SCEA, LLC (http://playstation.cony.com)
 */

#include <config.h>
#include <stdio.h>
#include <glib.h>
#include "io.h"

#define DEBUG

static MonoIoVirtualCallbacks *callbacks;

#ifdef DEBUG
static char *
virtual_to_real_debug (const char *src)
{
	if (src[0] == '/')
		return g_strdup_printf ("/tmp/virtpath-test%s", src);
	else
		return g_strdup_printf ("/tmp/virtpath-test/%s/%s", g_get_current_dir (), src);
}

static char *
real_to_virtual_debug (const char *src)
{
	if (g_str_has_prefix (src, "/tmp/virtpath-test/"))
		return g_strdup (src + 18);
	else
		return g_strdup (src);
}

static MonoIoPathPermission
virtual_path_permissions_debug (const char *src)
{
	if (strncmp (src, "/Application/", 13) == 0)
		return MONO_IO_PATH_READ | MONO_IO_PATH_WRITE | MONO_IO_PATH_EXECUTE;

	if (strncmp (src, "/Documents/", 11) == 0)
		return MONO_IO_PATH_READ | MONO_IO_PATH_WRITE;
	if (strncmp (src, "/Temp/", 11) == 0)
		return MONO_IO_PATH_READ | MONO_IO_PATH_WRITE;

	return MONO_IO_PATH_READ;
}
#endif

void
mono_io_init ()
{
	callbacks = g_malloc0 (sizeof (MonoIoVirtualCallbacks));

#ifdef DEBUG
	if (g_getenv ("MONO_DEBUG_VPATH") != NULL) {
		callbacks->virtual_to_real = virtual_to_real_debug;
		callbacks->real_to_virtual = real_to_virtual_debug;
		callbacks->virtual_path_permissions = virtual_path_permissions_debug;
		callbacks->free = g_free;
	} else {
		callbacks->virtual_to_real = NULL;
		callbacks->real_to_virtual = NULL;
		callbacks->virtual_path_permissions = NULL;
		callbacks->free = NULL;
	}
#else
	callbacks->virtual_to_real = NULL;
	callbacks->real_to_virtual = NULL;
	callbacks->virtual_path_permissions = NULL;
	callbacks->free = NULL;
#endif
}

void
mono_io_install_virtual_callbacks (MonoIoTranslateVirtualPathToRealPathFunc vtr_func, MonoIoTranslateRealPathToVirtualPathFunc rtv_func, MonoIoGetVirtualPathPermissionFunc vpp_func, MonoIoFreeFunc free)
{
	callbacks->virtual_to_real = vtr_func;
	callbacks->real_to_virtual = rtv_func;
	callbacks->virtual_path_permissions = vpp_func;
	callbacks->free = free;
}

MonoIoVirtualCallbacks *
mono_io_get_virtual_callbacks ()
{
	return callbacks;
}

