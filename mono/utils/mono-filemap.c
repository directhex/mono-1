/*
 * mono-filemap.c: Unix/Windows implementation for filemap.
 *
 * Author:
 *   Paolo Molaro (lupus@ximian.com)
 *
 * Copyright 2008-2008 Novell, Inc.
 */

#include "config.h"

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if !defined(SN_TARGET_PSP2)
#include <fcntl.h>
#endif
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#if defined(SN_TARGET_PSP2)
#include "bridge.h"
#endif

#include "mono-mmap.h"

MonoFileMap *
mono_file_map_open (const char* name)
{
#ifdef WIN32
	gunichar2 *wname = g_utf8_to_utf16 (name, -1, 0, 0, 0);
	MonoFileMap *result;

	if (wname == NULL)
		return NULL;
	result = (MonoFileMap *) _wfopen ((wchar_t *) wname, L"rb");
	g_free (wname);
	return result;
#elif defined(SN_TARGET_PSP2)
	int fileUid;
	name = g_file_vita_get_full_path (name);
	fileUid = pss_io_open(name, PSS_FREAD, 0);
	g_free (name);
	if(fileUid < 0)
	{
		return NULL;
	}
	return (MonoFileMap *)(fileUid + 1);
#else
	return (MonoFileMap *)fopen (name, "rb");
#endif
}

guint64 
mono_file_map_size (MonoFileMap *fmap)
{
#if defined(SN_TARGET_PSP2)
#if 1
	int fd = (int)fmap - 1;
	guint64 cur_offset = pss_io_lseek (fd, 0, SEEK_CUR);
	guint64 sz = pss_io_lseek (fd, 0, SEEK_END);
	pss_io_lseek (fd, cur_offset, SEEK_SET);
	return sz;
#else
	FILE* fd = (FILE*)fmap;
	fpos_t curr;
	fgetpos(fd, &curr);
	fseek(fd, 0, SEEK_END);
	fpos_t epos;
	fgetpos(fd, &epos);
	fseek(fd, (long)curr._Off, SEEK_SET);
	return (guint64)epos._Off;
#endif
#else
	struct stat stat_buf;
	if (fstat (fileno ((FILE*)fmap), &stat_buf) < 0)
		return 0;
	return stat_buf.st_size;
#endif
}

int
mono_file_map_fd (MonoFileMap *fmap)
{
#if defined(SN_TARGET_PSP2)
	return (int)fmap - 1;
#else
	return fileno ((FILE*)fmap);
#endif
}

int 
mono_file_map_close (MonoFileMap *fmap)
{
#if defined(SN_TARGET_PSP2)
	return pss_io_close ((int)fmap - 1);
#else
	return fclose ((FILE*)fmap);
#endif
}

#if !defined(HAVE_MMAP) && !defined (HOST_WIN32)

static mono_file_map_alloc_fn alloc_fn = (mono_file_map_alloc_fn) malloc;
static mono_file_map_release_fn release_fn = (mono_file_map_release_fn) free;

void
mono_file_map_set_allocator (mono_file_map_alloc_fn alloc, mono_file_map_release_fn release)
{
	alloc_fn = alloc == NULL     ? (mono_file_map_alloc_fn) malloc : alloc;
	release_fn = release == NULL ? (mono_file_map_release_fn) free : release;
}

void *
mono_file_map_file (size_t length, int flags, MonoFileMap *fmap, guint64 offset, void **ret_handle)
{
	int fd = (int)fmap - 1;
	guint64 cur_offset;
	size_t bytes_read;
	void *ptr = (*alloc_fn) (length);
	if (!ptr)
		return NULL;

	cur_offset = pss_io_lseek (fd, 0, SEEK_CUR);
	if (pss_io_lseek (fd, offset, SEEK_SET) != offset) {
		free (ptr);
		return NULL;
	}
	bytes_read = pss_io_read (fd, ptr, length);
	pss_io_lseek (fd, cur_offset, SEEK_SET);

	*ret_handle = NULL;
	return ptr;
}


void *
mono_file_map (size_t length, int flags, int fd, guint64 offset, void **ret_handle)
{
	guint64 cur_offset;
	size_t bytes_read;
	void *ptr = (*alloc_fn) (length);
	if (!ptr)
		return NULL;
#if defined(SN_TARGET_PSP2)
	if (length == 0)
		return ptr;
	cur_offset = pss_io_lseek (fd, 0, SEEK_CUR);
	if (pss_io_lseek (fd, offset, SEEK_SET) != offset) {
		free (ptr);
		return NULL;
	}
	bytes_read = pss_io_read (fd, ptr, length);
	pss_io_lseek (fd, cur_offset, SEEK_SET);
	if(bytes_read != length)
	{
		free (ptr);
		return NULL;
	}
#else
	cur_offset = lseek (fd, 0, SEEK_CUR);
	if (lseek (fd, offset, SEEK_SET) != offset) {
		free (ptr);
		return NULL;
	}
	bytes_read = read (fd, ptr, length);
	lseek (fd, cur_offset, SEEK_SET);
#endif
	*ret_handle = NULL;
	return ptr;
}

int
mono_file_unmap (void *addr, void *handle)
{
	(*release_fn) (addr);
	return 0;
}
#endif
