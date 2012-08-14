/*
 * file-io.c: File IO internal calls
 *
 * Author:
 *  Geoff Norton (geoff_norton@playstation.sony.com)
 *
 * Copyright 2012 SCEA, LLC
 */

#include <config.h>
#include <glib.h>

#include <mono/metadata/exception.h>
#include <mono/metadata/file-io.h>
#include <mono/metadata/file-io-pss.h>
#include <mono/metadata/file-io-pss-glob.h>
#include <mono/metadata/file-io-native.h>

#include "pssdef.h"
#include "file-io-pss-glob.h"

enum {
	NATIVE_HANDLE,
	PSS_HANDLE
} HandleType;

typedef struct {
	int type;
	gchar *path;
	union {
		ScePssHandle pss;
		HANDLE native;
	} handle;
} HandleWrapper;

typedef struct {
	HandleWrapper wrapper;
	MonoDomain *domain;
	gchar *path;
	pss_glob_t glob_data;
	int current_glob_path;
} FindWrapper;

static scePssHandleCloseFunc scePssHandleClose = NULL;

static scePssDirectoryCreateFunc scePssDirectoryCreate = NULL;
static scePssDirectoryRemoveFunc scePssDirectoryRemove = NULL;
static scePssDirectoryOpenFunc scePssDirectoryOpen = NULL;
static scePssDirectoryReadFunc scePssDirectoryRead = NULL;
static scePssDirectoryGetWorkingDirectoryFunc scePssDirectoryGetWorkingDirectory = NULL;
static scePssDirectorySetWorkingDirectoryFunc scePssDirectorySetWorkingDirectory = NULL;

static scePssFileOpenFunc scePssFileOpen = NULL;
static scePssFileDeleteFunc scePssFileDelete = NULL;
static scePssFileGetInformationFunc scePssFileGetInformation = NULL;
static scePssFileReadFunc scePssFileRead = NULL;
static scePssFileWriteFunc scePssFileWrite = NULL;
static scePssFileSeekFunc scePssFileSeek = NULL;
static scePssFileFlushFunc scePssFileFlush = NULL;
static scePssFileGetSizeFunc scePssFileGetSize = NULL;
static scePssFileTruncateFunc scePssFileTruncate = NULL;
static scePssFileCopyFunc scePssFileCopy = NULL;
static scePssFileSetAttributesFunc scePssFileSetAttributes = NULL;
static scePssFileSetTimeFunc scePssFileSetTime = NULL;
static scePssFileGetPathInformationFunc scePssFileGetPathInformation = NULL;

extern int mono_io_mode;

void
pss_io_icall_install_functions (scePssHandleCloseFunc handle_close,
								scePssDirectoryCreateFunc directory_create,
								scePssDirectoryRemoveFunc directory_remove,
								scePssDirectoryOpenFunc directory_open,
								scePssDirectoryReadFunc directory_read,
								scePssDirectoryGetWorkingDirectoryFunc directory_getcwd,
								scePssDirectorySetWorkingDirectoryFunc directory_setcwd,
								scePssFileOpenFunc file_open,
								scePssFileDeleteFunc file_delete,
								scePssFileGetInformationFunc file_getinfo,
								scePssFileReadFunc file_read,
								scePssFileWriteFunc file_write,
								scePssFileSeekFunc file_seek,
								scePssFileFlushFunc file_flush,
								scePssFileGetSizeFunc file_getsize,
								scePssFileTruncateFunc file_truncate,
								scePssFileCopyFunc file_copy,
								scePssFileSetAttributesFunc file_setattr,
								scePssFileSetTimeFunc file_settime,
								scePssFileGetPathInformationFunc file_getpathinfo)
{
	g_assert (handle_close != NULL);
	scePssHandleClose = handle_close;

	g_assert (directory_create != NULL);
	scePssDirectoryCreate = directory_create;
	
	g_assert (directory_remove != NULL);
	scePssDirectoryRemove = directory_remove;
	
	g_assert (directory_open != NULL);
	scePssDirectoryOpen = directory_open;
	
	g_assert (directory_read != NULL);
	scePssDirectoryRead = directory_read;
	
	g_assert (directory_getcwd != NULL);
	scePssDirectoryGetWorkingDirectory = directory_getcwd;
	
	g_assert (directory_setcwd != NULL);
	scePssDirectorySetWorkingDirectory = directory_setcwd;
	
	g_assert (file_open != NULL);
	scePssFileOpen = file_open;
	
	g_assert (file_delete != NULL);
	scePssFileDelete = file_delete;
	
	g_assert (file_getinfo != NULL);
	scePssFileGetInformation = file_getinfo;
	
	g_assert (file_read != NULL);
	scePssFileRead = file_read;
	
	g_assert (file_write != NULL);
	scePssFileWrite = file_write;
	
	g_assert (file_seek != NULL);
	scePssFileSeek = file_seek;
	
	g_assert (file_flush != NULL);
	scePssFileFlush = file_flush;
	
	g_assert (file_getsize != NULL);
	scePssFileGetSize = file_getsize;

	g_assert (file_truncate != NULL);
	scePssFileTruncate = file_truncate;

	g_assert (file_copy != NULL);
	scePssFileCopy = file_copy;

	g_assert (file_setattr != NULL);
	scePssFileSetAttributes = file_setattr;

	g_assert (file_settime != NULL);
	scePssFileSetTime = file_settime;

	g_assert (file_getpathinfo != NULL);
	scePssFileGetPathInformation = file_getpathinfo;

	mono_io_mode = MONO_IO_MODE_PSS;
}

static gint32
convert_error (ScePssResult res)
{
	switch (res) {
	case SCE_PSS_ERROR_NO_ERROR:
		return ERROR_SUCCESS;
	case SCE_PSS_ERROR_OUT_OF_MEMORY:
		return ERROR_NOT_ENOUGH_MEMORY;
	case SCE_PSS_ERROR_INVALID_PARAMETER:
		return ERROR_INVALID_PARAMETER;
	case SCE_PSS_ERROR_PATH_NOT_FOUND:
		return ERROR_PATH_NOT_FOUND;
	case SCE_PSS_ERROR_PATH_ALREADY_EXISTS:
		return ERROR_FILE_EXISTS;
	case SCE_PSS_ERROR_FILE_NOT_FOUND:
		return ERROR_FILE_NOT_FOUND;
	case SCE_PSS_ERROR_TIMEOUT:
		return ERROR_TIMEOUT;
	case SCE_PSS_ERROR_NOT_FOUND:
		return ERROR_NOT_FOUND;
	case SCE_PSS_ERROR_ALREADY_EXISTS:
		return ERROR_ALREADY_EXISTS;
	case SCE_PSS_ERROR_INVALID_HANDLE:
		return ERROR_INVALID_HANDLE;
	case SCE_PSS_ERROR_ACCESS_DENIED:
		return ERROR_ACCESS_DENIED;
	case SCE_PSS_ERROR_NOT_SUPPORTED:
		return ERROR_NOT_SUPPORTED;
	case SCE_PSS_ERROR_DISK_FULL:
		return ERROR_DISK_FULL;
	case SCE_PSS_ERROR_SEEK_FAILED:
		return ERROR_SEEK;
	case SCE_PSS_ERROR_INVALID_DATA:
		return ERROR_INVALID_DATA;
	case SCE_PSS_ERROR_READ_FAILED:
		return ERROR_READ_FAULT;
	case SCE_PSS_ERROR_WRITE_FAILED:
		return ERROR_WRITE_FAULT;
	case SCE_PSS_ERROR_BUSY:
		return ERROR_BUSY;
	case SCE_PSS_ERROR_BASE_ERROR:
	case SCE_PSS_ERROR_ERROR:
	case SCE_PSS_ERROR_OUT_OF_RANGE:
	case SCE_PSS_ERROR_END_OF_STREAM:
	case SCE_PSS_ERROR_BUFFER_FULL:
	case SCE_PSS_ERROR_INVALID_BUFFER:
	case SCE_PSS_ERROR_NOT_INITIALIZED:
	case SCE_PSS_ERROR_ALREADY_INITIALIZED:
	case SCE_PSS_ERROR_NOT_AVAILABLE:
	case SCE_PSS_ERROR_PENDING:
	case SCE_PSS_ERROR_OUT_OF_RESOURCES:
	case SCE_PSS_ERROR_INVALID_CALL:
	case SCE_PSS_ERROR_NOT_IMPLEMENTED:
	default:
		g_warning ("Unmapped error: %d\n", res);
		return ERROR_NOT_SUPPORTED;
	}
}

static gint32
convert_flags (uint32_t flags)
{
	gint32 attr = 0;

	if (flags & SCE_PSS_FILE_FLAG_DIRECTORY) {
		attr |= FILE_ATTRIBUTE_DIRECTORY;
	}
	else {
		attr |= FILE_ATTRIBUTE_NORMAL;
	}
	if (flags & SCE_PSS_FILE_FLAG_READONLY) attr |= FILE_ATTRIBUTE_READONLY;
	if (flags & SCE_PSS_FILE_FLAG_HIDDEN) attr |= FILE_ATTRIBUTE_HIDDEN;
	if (flags & SCE_PSS_FILE_FLAG_COMPRESSED) attr |= FILE_ATTRIBUTE_COMPRESSED;
	if (flags & SCE_PSS_FILE_FLAG_ENCRYPTED) attr |= FILE_ATTRIBUTE_ENCRYPTED;

	return attr;
}

/* System.IO.MonoIO internal calls */

MonoBoolean
pss_io_icall_System_IO_MonoIO_CreateDirectory (MonoString *path, gint32 *error)
{
	ScePssResult res;
	char *s;

	s = mono_string_to_utf8 (path);
	res = scePssDirectoryCreate (s);
	g_free (s);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_RemoveDirectory (MonoString *path, gint32 *error)
{
	ScePssResult res;
	char *s;

	s = mono_string_to_utf8 (path);
	res = scePssDirectoryRemove (s);
	g_free (s);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

/*
 * FIXME: This currently ignores the attributes, mask and pattern.
 */
MonoArray *
pss_io_icall_System_IO_MonoIO_GetFileSystemEntries (MonoString *path,
						 MonoString *path_with_pattern,
						 gint attrs, gint mask,
						 gint32 *error)
{
	MonoArray *result;
	ScePssHandle dir;
	ScePssResult res;
	ScePssFileInformation_t info;
	GPtrArray *names;
	char *utf8_path, *dirname, *pattern;
	int i;
	pss_glob_t glob_data;
	int glob_res;

	utf8_path = mono_string_to_utf8 (path_with_pattern);
	dirname = g_path_get_dirname (utf8_path);
	pattern = g_path_get_basename (utf8_path);
	res = scePssDirectoryOpen (dirname, NULL /* PSS Expects extension, not pattern, so glob ourselves here */, &dir);

	*error = convert_error (res);
	if (*error != ERROR_SUCCESS)
		return NULL;

	glob_res = _pss_glob (dir, pattern, 0, &glob_data, scePssDirectoryRead);
	if (glob_res != 0) {
		// FIXME: *error of some sort
		return NULL;
	}

	names = g_ptr_array_new ();

	for (i = 0; i < glob_data.gl_pathc; i ++) {
		char *name = glob_data.gl_pathv[i];
		if ((name [0] == '.' && name [1] == 0) ||
			(name [0] == '.' && name [1] == '.' && name [2] == 0))
			continue;

//FIXME			if ((info.uFlags & mask) == attrs)
			g_ptr_array_add (names, g_build_filename (dirname, name, NULL));
	}
	
	_pss_globfree (&glob_data);
	res = scePssHandleClose (dir);

	result = mono_array_new (mono_domain_get (), mono_defaults.string_class, names->len);
	for (i = 0; i < names->len; i++) {
		mono_array_setref (result, i, mono_string_new (mono_domain_get (), g_ptr_array_index (names, i)));
	}

	for (i = 0; i < names->len; i++) {
		g_free (g_ptr_array_index (names, i));
	}

	g_ptr_array_free (names, TRUE);
	g_free (utf8_path);
	g_free (dirname);
	g_free (pattern);
	
	return result;
}

MonoString *
pss_io_icall_System_IO_MonoIO_FindFirst (MonoString *path,
				      MonoString *path_with_pattern,
				      gint32 *result_attr, gint32 *error,
				      gpointer *handle)
{
	FindWrapper *wrapper;
	MonoString *result;
	ScePssFileInformation_t info;
	ScePssHandle dir;
	ScePssResult res;
	char *utf8_path, *dirname, *pattern;
	int glob_res;
	pss_glob_t glob_data;
	int i;

	*error = ERROR_SUCCESS;

	utf8_path = mono_string_to_utf8 (path_with_pattern);
	dirname = g_path_get_dirname (utf8_path);
	pattern = g_path_get_basename (utf8_path);
	res = scePssDirectoryOpen (dirname, NULL /* FIXME: PSS Expects extension, not pattern */, &dir);
	*error = convert_error (res);


	if (*error == ERROR_FILE_NOT_FOUND) {
		*error = ERROR_SUCCESS;
		return NULL;
	}

	if (*error != ERROR_SUCCESS)
		return NULL;

	res = _pss_glob (dir, pattern, 0/*FIXME flags*/, &glob_data, scePssDirectoryRead);
	if (res != 0) {
		*error = ERROR_SUCCESS;
		return NULL;
	}

	wrapper = g_new0 (FindWrapper, 1);
	wrapper->domain = mono_domain_get ();
	wrapper->path = g_strdup (dirname);
	wrapper->wrapper.type = PSS_HANDLE;
	wrapper->wrapper.handle.pss = dir;
	memmove (&wrapper->glob_data, &glob_data, sizeof(pss_glob_t));
	wrapper->current_glob_path = 0;
	*handle = wrapper;

	g_free (utf8_path);
	g_free (dirname);
	g_free (pattern);
	
	for (i = wrapper->current_glob_path; i < glob_data.gl_pathc; i ++) {
		gchar *name = glob_data.gl_pathv[i];
		gchar *full_path;

		if ((name [0] == '.' && name [1] == 0) ||
			(name [0] == '.' && name [1] == '.' && name [2] == 0))
			continue;

		full_path = g_build_filename (wrapper->path, name, NULL);
		res = scePssFileGetPathInformation (full_path, &info);
		if (res == SCE_PSS_ERROR_NO_ERROR) {
			*result_attr = convert_flags (info.uFlags);
		}
		else if (res == SCE_PSS_ERROR_NOT_FOUND) {
			g_free (full_path);
			continue;
		}
		result = mono_string_new (wrapper->domain, full_path);
		g_free (full_path);
		wrapper->current_glob_path = i+1;

		return result;
	}

	*error = convert_error (res);
	return NULL;
}

MonoString *
pss_io_icall_System_IO_MonoIO_FindNext (gpointer handle, gint32 *result_attr, gint32 *error)
{
	FindWrapper *wrapper = (FindWrapper *) handle;
	ScePssFileInformation_t info;
	ScePssResult res;
	MonoString *result;
	int i;

	*error = ERROR_SUCCESS;

	for (i = wrapper->current_glob_path; i < wrapper->glob_data.gl_pathc; i ++) {
		gchar *name = wrapper->glob_data.gl_pathv[i];
		gchar *full_path;

		if ((name [0] == '.' && name [1] == 0) ||
			(name [0] == '.' && name [1] == '.' && name [2] == 0))
			continue;

		full_path = g_build_filename (wrapper->path, name, NULL);
		res = scePssFileGetPathInformation (full_path, &info);
		if (res == SCE_PSS_ERROR_NO_ERROR) {
			*result_attr = convert_flags (info.uFlags);
		}
		else if (res == SCE_PSS_ERROR_NOT_FOUND) {
			g_free (full_path);
			continue;
		}
		else {
			*error = convert_error (res);
			break;
		}
		result = mono_string_new (wrapper->domain, full_path);
		g_free (full_path);
		wrapper->current_glob_path = i+1;

		return result;
	}

	return NULL;
}

int
pss_io_icall_System_IO_MonoIO_FindClose (gpointer handle)
{
	FindWrapper *wrapper = (FindWrapper *) handle;
	ScePssResult res;

	res = scePssHandleClose (wrapper->wrapper.handle.pss);
	_pss_globfree (&wrapper->glob_data);
	g_free (wrapper->path);
	g_free (wrapper);

	return convert_error (res);
}

MonoString *
pss_io_icall_System_IO_MonoIO_GetCurrentDirectory (gint32 *error)
{
	ScePssResult res;
	MonoString *result;
	gchar *buf;
	int len;

	len = MAX_PATH + 1;
	buf = g_new (gchar, len);
	
	result = NULL;
	while ((res = scePssDirectoryGetWorkingDirectory (buf, len)) == SCE_PSS_ERROR_BUFFER_FULL) {
		len *= 2;
		g_free (buf);
		buf = g_new (gchar, len);
	}
	
	*error = convert_error (res);

	if (*error == ERROR_SUCCESS) {
		result = mono_string_new (mono_domain_get (), buf);
	}

	g_free (buf);
	return result;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_SetCurrentDirectory (MonoString *path,
						gint32 *error)
{
	ScePssResult res;
	char *s;

	s = mono_string_to_utf8 (path);
	res = scePssDirectorySetWorkingDirectory (s);
	g_free (s);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_MoveFile (MonoString *path, MonoString *dest,
				     gint32 *error)
{
	ScePssResult res;
	char *s, *d;

	s = mono_string_to_utf8 (path);
	d = mono_string_to_utf8 (dest);
	res = scePssFileCopy (s, d, 1);
	g_free (s);
	g_free (d);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_ReplaceFile (MonoString *sourceFileName, MonoString *destinationFileName,
					MonoString *destinationBackupFileName, MonoBoolean ignoreMetadataErrors,
					gint32 *error)
{
	*error = ERROR_NOT_SUPPORTED;
	return FALSE;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_CopyFile (MonoString *path, MonoString *dest,
				     MonoBoolean overwrite, gint32 *error)
{
	ScePssResult res;
	char *s, *d;

	s = mono_string_to_utf8 (path);
	d = mono_string_to_utf8 (dest);
	res = scePssFileCopy (s, d, 0);
	g_free (s);
	g_free (d);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_DeleteFile (MonoString *path, gint32 *error)
{
	ScePssResult res;
	char *s;

	s = mono_string_to_utf8 (path);
	res = scePssFileDelete (s);
	g_free (s);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

gint32 
pss_io_icall_System_IO_MonoIO_GetFileAttributes (MonoString *path, gint32 *error)
{
	ScePssResult res;
	ScePssFileInformation_t info;
	gint32 attr = -1;
	char *s;

	s = mono_string_to_utf8 (path);
	res = scePssFileGetPathInformation (s, &info);

	if (res == SCE_PSS_ERROR_NO_ERROR) {
		attr = convert_flags (info.uFlags);
	} else if (res == SCE_PSS_ERROR_NOT_FOUND) {
		ScePssHandle handle;

		res = scePssDirectoryOpen (s, NULL, &handle);
		if (res == SCE_PSS_ERROR_NO_ERROR) {
			attr = FILE_ATTRIBUTE_DIRECTORY;
			res = scePssHandleClose (handle);
		}
	}

	g_free (s);

	*error = convert_error (res);

	return attr;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_SetFileAttributes (MonoString *path, gint32 attrs,
					      gint32 *error)
{
	*error = ERROR_NOT_SUPPORTED;
	return FALSE;
}

gint32
pss_io_icall_System_IO_MonoIO_GetFileType (HANDLE handle, gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;

	if (wrapper->type == NATIVE_HANDLE)
		return native_io_icall_System_IO_MonoIO_GetFileType (wrapper->handle.native, error);

	//FIXME: Need to differentiate file types
	*error = ERROR_SUCCESS;
	return FILE_TYPE_DISK;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_GetFileStat (MonoString *path, MonoIOStat *stat,
					gint32 *error)
{
	ScePssResult res;
	ScePssFileInformation_t info;
	char *s;

	memset (stat, 0, sizeof (MonoIOStat));
	s = mono_string_to_utf8 (path);

	res = scePssFileGetPathInformation (s, &info);
	if (res == SCE_PSS_ERROR_NO_ERROR) {

		stat->attributes = convert_flags (info.uFlags);
		stat->creation_time = info.tCreationTime;
		stat->last_access_time = info.tLastAccessTime;
		stat->last_write_time = info.tLastWriteTime;
		stat->length = info.uFileSize;
		MONO_STRUCT_SETREF (stat, name, mono_string_new (mono_domain_get (), info.szName));
	}

	g_free (s);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

HANDLE 
pss_io_icall_System_IO_MonoIO_Open (MonoString *filename, gint32 mode,
				 gint32 access_mode, gint32 share, gint32 options,
				 gint32 *error)
{
	HandleWrapper *wrapper;
	ScePssResult res;
	ScePssUInt32 flags;
	ScePssFileInformation_t info;
	char *s;

	flags = SCE_PSS_FILE_OPEN_FLAG_BINARY;

	s = mono_string_to_utf8 (filename);

	switch (access_mode) {
	case FileAccess_Read: flags |= SCE_PSS_FILE_OPEN_FLAG_READ; break;
	case FileAccess_Write: flags |= SCE_PSS_FILE_OPEN_FLAG_WRITE; break;
	case FileAccess_ReadWrite: flags |= SCE_PSS_FILE_OPEN_FLAG_READ|SCE_PSS_FILE_OPEN_FLAG_WRITE; break;
	default: g_warning ("Unknown access mode: 0x%d\n", access_mode); flags |= SCE_PSS_FILE_OPEN_FLAG_READ; break;
	}

	switch (mode) {
	case FileMode_CreateNew: flags |= SCE_PSS_FILE_OPEN_FLAG_NOREPLACE; break;
	case FileMode_Create: flags |= SCE_PSS_FILE_OPEN_FLAG_ALWAYS_CREATE; break;
	case FileMode_Append: flags |= SCE_PSS_FILE_OPEN_FLAG_APPEND; break;
	case FileMode_Open: flags |= SCE_PSS_FILE_OPEN_FLAG_NOTRUNCATE; break;
	case FileMode_OpenOrCreate: {
		res = scePssFileGetPathInformation (s, &info);
		if (res == SCE_PSS_ERROR_NOT_FOUND)
			flags |= SCE_PSS_FILE_OPEN_FLAG_WRITE;
		else
			flags |= SCE_PSS_FILE_OPEN_FLAG_NOTRUNCATE;
		break; 
								}
	case FileMode_Truncate:
	default: g_warning ("Unhandled file mode: 0x%d\n", mode); break;
	}

	if (options != 0) {
		g_warning ("FileOpen options were specified: 0x%d\n", options);
		*error = ERROR_NOT_SUPPORTED;
		return INVALID_HANDLE_VALUE;
	}

	wrapper = (HandleWrapper *) g_new0 (HandleWrapper, 1);
	wrapper->type = PSS_HANDLE;
	wrapper->path = g_strdup (s);
	res = scePssFileOpen (s, flags, &wrapper->handle.pss);
	g_free (s);

	*error = convert_error (res);

	if (*error == ERROR_SUCCESS)
		return (HANDLE) wrapper;

	return INVALID_HANDLE_VALUE;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_Close (HANDLE handle, gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;
	
	g_assert (wrapper->type == PSS_HANDLE);

	res = scePssHandleClose (wrapper->handle.pss);
	g_free (wrapper->path);
	g_free (wrapper);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

gint32 
pss_io_icall_System_IO_MonoIO_Read (HANDLE handle, MonoArray *dest,
				 gint32 dest_offset, gint32 count,
				 gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	guchar *buffer;
	ScePssResult res;
	guint32 n;

	if (wrapper->type == NATIVE_HANDLE)
		return native_io_icall_System_IO_MonoIO_Read (wrapper->handle.native, dest, dest_offset, count, error);

	MONO_CHECK_ARG_NULL (dest);
	
	if (dest_offset + count > mono_array_length (dest))
		return 0;

	buffer = mono_array_addr (dest, guchar, dest_offset);

	res = scePssFileRead (wrapper->handle.pss, buffer, count, &n); 

	*error = convert_error (res);

	return (gint32)n;
}

gint32 
pss_io_icall_System_IO_MonoIO_Write (HANDLE handle, MonoArray *src,
				  gint32 src_offset, gint32 count,
				  gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	guchar *buffer;
	ScePssResult res;
	guint32 n;

	if (wrapper->type == NATIVE_HANDLE)
		return native_io_icall_System_IO_MonoIO_Write (wrapper->handle.native, src, src_offset, count, error);

	MONO_CHECK_ARG_NULL (src);
	
	if (src_offset + count > mono_array_length (src))
		return 0;
	
	buffer = mono_array_addr (src, guchar, src_offset);

	res = scePssFileWrite (wrapper->handle.pss, buffer, count, &n);

	*error = convert_error (res);

	return (gint32)n;
}

gint64 
pss_io_icall_System_IO_MonoIO_Seek (HANDLE handle, gint64 offset, gint32 origin,
				 gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;
	ScePssFileSeekType_t seek = SCE_PSS_FILE_SEEK_TYPE_BEGIN;

	if (wrapper->type == NATIVE_HANDLE)
		return native_io_icall_System_IO_MonoIO_Seek (wrapper->handle.native, offset, origin, error);

	if (offset > G_MAXINT32) {
		*error = ERROR_NOT_SUPPORTED;
		return -1;
	}

	switch (origin) {
	case SeekOrigin_Begin: seek = SCE_PSS_FILE_SEEK_TYPE_BEGIN; break;
	case SeekOrigin_Current: seek = SCE_PSS_FILE_SEEK_TYPE_CURRENT; break;
	case SeekOrigin_End: seek = SCE_PSS_FILE_SEEK_TYPE_END; break;
	default: g_warning ("SeekOrigin has an unknown value 0x%x\n", origin); break;
	}

	res = scePssFileSeek (wrapper->handle.pss, (int32_t) offset, seek);

	*error = convert_error (res);

	return offset;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_Flush (HANDLE handle, gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;

	if (wrapper->type == NATIVE_HANDLE)
		return native_io_icall_System_IO_MonoIO_Flush (wrapper->handle.native, error);

	res = scePssFileFlush (wrapper->handle.pss);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

gint64 
pss_io_icall_System_IO_MonoIO_GetLength (HANDLE handle, gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;
	guint32 len;

	g_assert (wrapper->type == PSS_HANDLE);

	res = scePssFileGetSize (wrapper->handle.pss, &len);

	*error = convert_error (res);

	return (gint64) len;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_SetLength (HANDLE handle, gint64 length,
				      gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;

	g_assert (wrapper->type == PSS_HANDLE);

	if (length < 0 || length > 0xffffffffU) {
		*error = ERROR_NOT_SUPPORTED;
		return FALSE;
	}

	res = scePssFileTruncate (wrapper->handle.pss, (uint32_t) length);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

MonoBoolean
pss_io_icall_System_IO_MonoIO_SetFileTime (HANDLE handle, gint64 creation_time,
					gint64 last_access_time,
					gint64 last_write_time, gint32 *error)
{
	HandleWrapper *wrapper = (HandleWrapper *) handle;
	ScePssResult res;

	g_assert (wrapper->type == PSS_HANDLE);

	res = scePssFileSetTime (wrapper->path, (const uint64_t *)&creation_time, (const uint64_t *)&last_access_time, (const uint64_t *)&last_write_time);

	*error = convert_error (res);

	return *error == ERROR_SUCCESS;
}

gint32
pss_io_icall_System_IO_MonoIO_GetTempPath (MonoString **mono_name)
{
	mono_gc_wbarrier_generic_store ((gpointer) mono_name, (MonoObject *) mono_string_new (mono_domain_get (), "/Temp"));

	return 5;
}

void pss_io_icall_System_IO_MonoIO_Lock (HANDLE handle, gint64 position,
				      gint64 length, gint32 *error)
{
	*error = ERROR_NOT_SUPPORTED;
	return;
}

void pss_io_icall_System_IO_MonoIO_Unlock (HANDLE handle, gint64 position,
					gint64 length, gint32 *error)
{
	*error = ERROR_NOT_SUPPORTED;
	return;
}

HANDLE
pss_io_icall_System_IO_MonoIO_get_ConsoleOutput ()
{
	HandleWrapper *wrapper = g_new0 (HandleWrapper, 1);

	wrapper->type = NATIVE_HANDLE;
	wrapper->handle.native = GetStdHandle (STD_OUTPUT_HANDLE);

	return wrapper;
}

HANDLE 
pss_io_icall_System_IO_MonoIO_get_ConsoleInput ()
{
	HandleWrapper *wrapper = g_new0 (HandleWrapper, 1);

	wrapper->type = NATIVE_HANDLE;
	wrapper->handle.native = GetStdHandle (STD_INPUT_HANDLE);

	return wrapper;
}

HANDLE 
pss_io_icall_System_IO_MonoIO_get_ConsoleError ()
{
	HandleWrapper *wrapper = g_new0 (HandleWrapper, 1);

	wrapper->type = NATIVE_HANDLE;
	wrapper->handle.native = GetStdHandle (STD_ERROR_HANDLE);

	return wrapper;
}