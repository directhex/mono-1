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

#include <mono/metadata/file-io.h>
#include <mono/metadata/file-io-native.h>
#include <mono/metadata/file-io-pss.h>
#include <mono/metadata/exception.h>

int mono_io_mode = MONO_IO_MODE_NATIVE;

/* System.IO.MonoIO internal calls */

MonoBoolean
ves_icall_System_IO_MonoIO_CreateDirectory (MonoString *path, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_CreateDirectory (path, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_CreateDirectory (path, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_RemoveDirectory (MonoString *path, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_RemoveDirectory (path, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_RemoveDirectory (path, error);
	}
}

MonoArray *
ves_icall_System_IO_MonoIO_GetFileSystemEntries (MonoString *path,
						 MonoString *path_with_pattern,
						 gint attrs, gint mask,
						 gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetFileSystemEntries (path, path_with_pattern, attrs, mask, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetFileSystemEntries (path, path_with_pattern, attrs, mask, error);
	}
}

MonoString *
ves_icall_System_IO_MonoIO_FindFirst (MonoString *path,
				      MonoString *path_with_pattern,
				      gint32 *result_attr, gint32 *error,
				      gpointer *handle)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_FindFirst (path, path_with_pattern, result_attr, error, handle);
	} else {
		return pss_io_icall_System_IO_MonoIO_FindFirst (path, path_with_pattern, result_attr, error, handle);
	}
}

MonoString *
ves_icall_System_IO_MonoIO_FindNext (gpointer handle, gint32 *result_attr, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_FindNext (handle, result_attr, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_FindNext (handle, result_attr, error);
	}
}

int
ves_icall_System_IO_MonoIO_FindClose (gpointer handle)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_FindClose (handle);
	} else {
		return pss_io_icall_System_IO_MonoIO_FindClose (handle);
	}
}

MonoString *
ves_icall_System_IO_MonoIO_GetCurrentDirectory (gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetCurrentDirectory (error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetCurrentDirectory (error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_SetCurrentDirectory (MonoString *path,
						gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_SetCurrentDirectory (path, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_SetCurrentDirectory (path, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_MoveFile (MonoString *src, MonoString *dest,
				     gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_MoveFile (src, dest, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_MoveFile (src, dest, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_ReplaceFile (MonoString *sourceFileName, MonoString *destinationFileName,
					MonoString *destinationBackupFileName, MonoBoolean ignoreMetadataErrors,
					gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_ReplaceFile (sourceFileName, destinationFileName, destinationBackupFileName, ignoreMetadataErrors, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_ReplaceFile (sourceFileName, destinationFileName, destinationBackupFileName, ignoreMetadataErrors, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_CopyFile (MonoString *src, MonoString *dest,
				     MonoBoolean overwrite, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_CopyFile (src, dest, overwrite, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_CopyFile (src, dest, overwrite, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_DeleteFile (MonoString *path, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_DeleteFile (path, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_DeleteFile (path, error);
	}
}

gint32 
ves_icall_System_IO_MonoIO_GetFileAttributes (MonoString *path, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetFileAttributes (path, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetFileAttributes (path, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_SetFileAttributes (MonoString *path, gint32 attrs,
					      gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_SetFileAttributes (path, attrs, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_SetFileAttributes (path, attrs, error);
	}
}

gint32
ves_icall_System_IO_MonoIO_GetFileType (HANDLE handle, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetFileType (handle, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetFileType (handle, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_GetFileStat (MonoString *path, MonoIOStat *stat,
					gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetFileStat (path, stat, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetFileStat (path, stat, error);
	}
}

HANDLE 
ves_icall_System_IO_MonoIO_Open (MonoString *filename, gint32 mode,
				 gint32 access_mode, gint32 share, gint32 options,
				 gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Open (filename, mode, access_mode, share, options, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Open (filename, mode, access_mode, share, options, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_Close (HANDLE handle, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Close (handle, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Close (handle, error);
	}
}

gint32 
ves_icall_System_IO_MonoIO_Read (HANDLE handle, MonoArray *dest,
				 gint32 dest_offset, gint32 count,
				 gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Read (handle, dest, dest_offset, count, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Read (handle, dest, dest_offset, count, error);
	}
}

gint32 
ves_icall_System_IO_MonoIO_Write (HANDLE handle, MonoArray *src,
				  gint32 src_offset, gint32 count,
				  gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Write (handle, src, src_offset, count, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Write (handle, src, src_offset, count, error);
	}
}

gint64 
ves_icall_System_IO_MonoIO_Seek (HANDLE handle, gint64 offset, gint32 origin,
				 gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Seek (handle, offset, origin, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Seek (handle, offset, origin, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_Flush (HANDLE handle, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_Flush (handle, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_Flush (handle, error);
	}
}

gint64 
ves_icall_System_IO_MonoIO_GetLength (HANDLE handle, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetLength (handle, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetLength (handle, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_SetLength (HANDLE handle, gint64 length,
				      gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_SetLength (handle, length, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_SetLength (handle, length, error);
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_SetFileTime (HANDLE handle, gint64 creation_time,
					gint64 last_access_time,
					gint64 last_write_time, gint32 *error)
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_SetFileTime (handle, creation_time, last_access_time, last_write_time, error);
	} else {
		return pss_io_icall_System_IO_MonoIO_SetFileTime (handle, creation_time, last_access_time, last_write_time, error);
	}
}

HANDLE
ves_icall_System_IO_MonoIO_get_ConsoleOutput ()
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_get_ConsoleOutput ();
	} else {
		return pss_io_icall_System_IO_MonoIO_get_ConsoleOutput ();
	}
}

HANDLE 
ves_icall_System_IO_MonoIO_get_ConsoleInput ()
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_get_ConsoleInput ();
	} else {
		return pss_io_icall_System_IO_MonoIO_get_ConsoleInput ();
	}
}

HANDLE 
ves_icall_System_IO_MonoIO_get_ConsoleError ()
{
	MONO_ARCH_SAVE_REGS;

	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_get_ConsoleError ();
	} else {
		return pss_io_icall_System_IO_MonoIO_get_ConsoleError ();
	}
}

MonoBoolean
ves_icall_System_IO_MonoIO_CreatePipe (HANDLE *read_handle,
				       HANDLE *write_handle)
{
	SECURITY_ATTRIBUTES attr;
	gboolean ret;
	
	MONO_ARCH_SAVE_REGS;

	attr.nLength=sizeof(SECURITY_ATTRIBUTES);
	attr.bInheritHandle=TRUE;
	attr.lpSecurityDescriptor=NULL;
	
	ret=CreatePipe (read_handle, write_handle, &attr, 0);
	if(ret==FALSE) {
		/* FIXME: throw an exception? */
		return(FALSE);
	}
	
	return(TRUE);
}

MonoBoolean ves_icall_System_IO_MonoIO_DuplicateHandle (HANDLE source_process_handle, 
						HANDLE source_handle, HANDLE target_process_handle, HANDLE *target_handle, 
						gint32 access, gint32 inherit, gint32 options)
{
	/* This is only used on Windows */
	gboolean ret;
	
	MONO_ARCH_SAVE_REGS;
	
	ret=DuplicateHandle (source_process_handle, source_handle, target_process_handle, target_handle, access, inherit, options);
	if(ret==FALSE) {
		/* FIXME: throw an exception? */
		return(FALSE);
	}
	
	return(TRUE);
}

gunichar2 
ves_icall_System_IO_MonoIO_get_VolumeSeparatorChar ()
{
#if (defined (TARGET_WIN32) || defined(TARGET_VITA)) && !defined(TARGET_PSS)
	return (gunichar2) ':';	/* colon */
#else
	return (gunichar2) '/';	/* forward slash */
#endif
}

gunichar2 
ves_icall_System_IO_MonoIO_get_DirectorySeparatorChar ()
{
#if defined (TARGET_WIN32) && !defined(TARGET_PSS)
	return (gunichar2) '\\';	/* backslash */
#else
	return (gunichar2) '/';	/* forward slash */
#endif
}

gunichar2 
ves_icall_System_IO_MonoIO_get_AltDirectorySeparatorChar ()
{
#if defined (TARGET_WIN32) && !defined(TARGET_PSS)
	return (gunichar2) '/';	/* forward slash */
#else
	return (gunichar2) '/';	/* slash, same as DirectorySeparatorChar */
#endif
}

gunichar2 
ves_icall_System_IO_MonoIO_get_PathSeparator ()
{
#if defined (TARGET_WIN32) && !defined(TARGET_PSS)
	return (gunichar2) ';';	/* semicolon */
#else
	return (gunichar2) ':';	/* colon */
#endif
}

static const gunichar2
invalid_path_chars [] = {
#if defined (TARGET_WIN32) || defined (TARGET_PSS)
	0x0022,				/* double quote, which seems allowed in MS.NET but should be rejected */
	0x003c,				/* less than */
	0x003e,				/* greater than */
	0x007c,				/* pipe */
	0x0008,
	0x0010,
	0x0011,
	0x0012,
	0x0014,
	0x0015,
	0x0016,
	0x0017,
	0x0018,
	0x0019,
#endif
	0x0000				/* null */
};

MonoArray *
ves_icall_System_IO_MonoIO_get_InvalidPathChars ()
{
	MonoArray *chars;
	MonoDomain *domain;
	int i, n;

	MONO_ARCH_SAVE_REGS;

	domain = mono_domain_get ();
	n = sizeof (invalid_path_chars) / sizeof (gunichar2);
	chars = mono_array_new (domain, mono_defaults.char_class, n);

	for (i = 0; i < n; ++ i)
		mono_array_set (chars, gunichar2, i, invalid_path_chars [i]);
	
	return chars;
}

gint32
ves_icall_System_IO_MonoIO_GetTempPath (MonoString **mono_name)
{
	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		return native_io_icall_System_IO_MonoIO_GetTempPath (mono_name);
	} else {
		return pss_io_icall_System_IO_MonoIO_GetTempPath (mono_name);
	}
}

void ves_icall_System_IO_MonoIO_Lock (HANDLE handle, gint64 position,
				      gint64 length, gint32 *error)
{
	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		native_io_icall_System_IO_MonoIO_Lock (handle, position, length, error);
	} else {
		pss_io_icall_System_IO_MonoIO_Lock (handle, position, length, error);
	}
}

void ves_icall_System_IO_MonoIO_Unlock (HANDLE handle, gint64 position,
					gint64 length, gint32 *error)
{
	if (G_LIKELY (mono_io_mode == MONO_IO_MODE_NATIVE)) {
		native_io_icall_System_IO_MonoIO_Unlock (handle, position, length, error);
	} else {
		pss_io_icall_System_IO_MonoIO_Unlock (handle, position, length, error);
	}
}