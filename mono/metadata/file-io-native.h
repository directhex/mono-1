/*
 * file-io.h: File IO internal calls
 *
 * Authors:
 *	Dick Porter (dick@ximian.com)
 *	Dan Lewis (dihlewis@yahoo.co.uk)
 *  Geoff Norton (geoff_norton@playstation.sony.com)
 *
 * (C) 2001 Ximian, Inc.
 * (C) 2012 SCEA, LLC
 */

#ifndef _MONO_METADATA_FILEIO_NATIVE_H_
#define _MONO_METADATA_FILEIO_NATIVE_H_

#include <config.h>
#include <glib.h>

#include <mono/metadata/file-io.h>

G_BEGIN_DECLS

MonoBoolean
native_io_icall_System_IO_MonoIO_CreateDirectory (MonoString *path, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_RemoveDirectory (MonoString *path, gint32 *error) MONO_INTERNAL;

MonoArray *
native_io_icall_System_IO_MonoIO_GetFileSystemEntries (MonoString *path,
						 MonoString *path_with_pattern,
						 gint mask, gint attrs,
						 gint32 *error) MONO_INTERNAL;

MonoString *
native_io_icall_System_IO_MonoIO_FindFirst (MonoString *path,
				      MonoString *path_with_pattern,
				      gint32 *result_mask,
				      gint32 *error,
				      gpointer *handle) MONO_INTERNAL;
MonoString *
native_io_icall_System_IO_MonoIO_FindNext (gpointer handle, gint32 *result_mask, gint32 *error) MONO_INTERNAL;

int
native_io_icall_System_IO_MonoIO_FindClose (gpointer handle) MONO_INTERNAL;

MonoString *
native_io_icall_System_IO_MonoIO_GetCurrentDirectory (gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_SetCurrentDirectory (MonoString *path,
						gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_MoveFile (MonoString *path, MonoString *dest,
				     gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_CopyFile (MonoString *path, MonoString *dest,
				     MonoBoolean overwrite, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_DeleteFile (MonoString *path, gint32 *error) MONO_INTERNAL;

gint32 
native_io_icall_System_IO_MonoIO_GetFileAttributes (MonoString *path, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_SetFileAttributes (MonoString *path, gint32 attrs,
					      gint32 *error) MONO_INTERNAL;

gint32
native_io_icall_System_IO_MonoIO_GetFileType (HANDLE handle, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_GetFileStat (MonoString *path, MonoIOStat *stat,
					gint32 *error) MONO_INTERNAL;

HANDLE 
native_io_icall_System_IO_MonoIO_Open (MonoString *filename, gint32 mode,
				 gint32 access_mode, gint32 share, gint32 options,
				 gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_Close (HANDLE handle, gint32 *error) MONO_INTERNAL;

gint32 
native_io_icall_System_IO_MonoIO_Read (HANDLE handle, MonoArray *dest,
				 gint32 dest_offset, gint32 count,
				 gint32 *error) MONO_INTERNAL;

gint32 
native_io_icall_System_IO_MonoIO_Write (HANDLE handle, MonoArray *src,
				  gint32 src_offset, gint32 count,
				  gint32 *error) MONO_INTERNAL;

gint64 
native_io_icall_System_IO_MonoIO_Seek (HANDLE handle, gint64 offset, gint32 origin,
				 gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_Flush (HANDLE handle, gint32 *error) MONO_INTERNAL;

gint64 
native_io_icall_System_IO_MonoIO_GetLength (HANDLE handle, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_SetLength (HANDLE handle, gint64 length,
				      gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_SetFileTime (HANDLE handle, gint64 creation_time,
					gint64 last_access_time,
					gint64 last_write_time, gint32 *error) MONO_INTERNAL;

HANDLE 
native_io_icall_System_IO_MonoIO_get_ConsoleOutput (void) MONO_INTERNAL;

HANDLE 
native_io_icall_System_IO_MonoIO_get_ConsoleInput (void) MONO_INTERNAL;

HANDLE 
native_io_icall_System_IO_MonoIO_get_ConsoleError (void) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_CreatePipe (HANDLE *read_handle,
				       HANDLE *write_handle) MONO_INTERNAL;

MonoBoolean native_io_icall_System_IO_MonoIO_DuplicateHandle (HANDLE source_process_handle, 
						HANDLE source_handle, HANDLE target_process_handle, HANDLE *target_handle, 
						gint32 access, gint32 inherit, gint32 options) MONO_INTERNAL;

gunichar2 
native_io_icall_System_IO_MonoIO_get_VolumeSeparatorChar (void) MONO_INTERNAL;

gunichar2 
native_io_icall_System_IO_MonoIO_get_DirectorySeparatorChar (void) MONO_INTERNAL;

gunichar2 
native_io_icall_System_IO_MonoIO_get_AltDirectorySeparatorChar (void) MONO_INTERNAL;

gunichar2 
native_io_icall_System_IO_MonoIO_get_PathSeparator (void) MONO_INTERNAL;

MonoArray *
native_io_icall_System_IO_MonoIO_get_InvalidPathChars (void) MONO_INTERNAL;

gint32
native_io_icall_System_IO_MonoIO_GetTempPath (MonoString **mono_name) MONO_INTERNAL;

void native_io_icall_System_IO_MonoIO_Lock (HANDLE handle, gint64 position,
					     gint64 length, gint32 *error) MONO_INTERNAL;
void native_io_icall_System_IO_MonoIO_Unlock (HANDLE handle, gint64 position,
					       gint64 length, gint32 *error) MONO_INTERNAL;

MonoBoolean
native_io_icall_System_IO_MonoIO_ReplaceFile (MonoString *sourceFileName, MonoString *destinationFileName,
					MonoString *destinationBackupFileName, MonoBoolean ignoreMetadataErrors,
					gint32 *error) MONO_INTERNAL;

G_END_DECLS

#endif /* _MONO_METADATA_FILEIO_H_ */
