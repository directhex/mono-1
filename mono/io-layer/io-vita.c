#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include <glib.h>
#include <mono/io-layer/io-layer.h>
#include <mono/io-layer/wapi-private.h>
#include <mono/io-layer/wapi_glob.h>

#include "bridge.h"
char* mono_unicode_to_external (const gunichar2 *name);

//--- debug -------------------------------------------------------------------
#if 0
#define PRINTF(...)			printf( "[io-vita] " __VA_ARGS__ )
#define ERR_PRINTF(...)		printf( "[io-vita:ERROR] " __VA_ARGS__ )
#define TRACE				printf( "[io-vita:TRACE] %s %d %s\n", __FILE__, __LINE__, __FUNCTION__ )
#else
#define PRINTF(...)
#define ERR_PRINTF(...)
#define TRACE
#endif
//-----------------------------------------------------------------------------

typedef struct _CryptoContext {
	int handle;
	int valid;
	int size;
	int type;
} CryptoContext;

#if defined(TARGET_VITA)
#include "bridge.h"
#define PSS_USE_CRYPTO
#endif

#undef PSS_USE_CRYPTO

/*
 * The IO layer assumes that file handles are small integers numbered from 0. So we store our
 * data in an array of structures, and use the array index as the handle.
 */
typedef struct _VitaFile {
	FILE *file;
	/* If != -1, this represents a directory, in which case 'file' is invalid */
	int dir_fd;
	char *name;
	guint32 attrs;
	CryptoContext *context;
} VitaFile;

static WapiGetCryptoPolicy get_crypto_policy = NULL;

void wapi_set_crypto_policy_callback (WapiGetCryptoPolicy cp_func)
{
	get_crypto_policy = cp_func;
}

#define MAX_FILES 1024

#define STDIN_HANDLE 0
#define STDOUT_HANDLE 1
#define STDERR_HANDLE 2

static VitaFile files [MAX_FILES];

static gboolean
check_handle (gpointer handle)
{
	TRACE;
	int fd = (int)handle;

	if (fd > MAX_FILES)
		return FALSE;
	return  TRUE;
}

static VitaFile*
get_handle_data (gpointer handle)
{
	TRACE;
	int fd = (int)handle;

	g_assert (fd >= 0 && fd < MAX_FILES);
	if (!files [fd].file) {
		printf ("FD: %d\n", fd);
		g_assert (files [fd].file);
	}

	return &files [fd];
}	

/* Return the FILE pointer for the io layer handle HANDLE */
static FILE*
get_file_for_handle (gpointer handle)
{
	TRACE;
	VitaFile *f = get_handle_data (handle);

	g_assert (f->dir_fd == -1);
	
	return f->file;
}

static char*
get_name_for_handle (gpointer handle)
{
	TRACE;
	return get_handle_data (handle)->name;
}	

static guint32
get_attrs_for_handle (gpointer handle)
{
	TRACE;
	return get_handle_data (handle)->attrs;
}	

static gpointer
create_handle (void)
{
	int i, handle;
	TRACE;

	// FIXME: locking, speed
	handle = -1;
	for (i = 16; i < MAX_FILES; ++i) {
		if (!files [i].file) {
			handle = i;
			break;
		}
	}
	g_assert (handle != -1);

	return (gpointer)handle;
}
	
static void
delete_handle (gpointer handle)
{
	TRACE;
	int fd = (int)handle;

	g_assert (fd >= 0 && fd < MAX_FILES);
	g_assert (files [fd].file);

	files [fd].file = NULL;
	g_free (files [fd].name);
}

static int
win32_file_error_from_errno (int err)
{
	TRACE;
	switch (err) {
	case PSS_ERROR_ERRNO_EEXIST:
		return ERROR_FILE_EXISTS;
	case PSS_ERROR_ERRNO_ENOENT:
		return ERROR_FILE_NOT_FOUND;
	case PSS_ERROR_ERRNO_EACCES:
		return ERROR_ACCESS_DENIED;
	default:
		//printf ("Unmapped error code: %x\n", err);
		return ERROR_NOT_SUPPORTED;
	}
}

static void
set_error_from_vita_errno (int err)
{
	TRACE;
	SetLastError (win32_file_error_from_errno (err));
}

char *current_dir;

static char*
get_fullname (const gunichar2 *name)
{
	TRACE;
	char *filename, *fullname;

	filename = mono_unicode_to_external (name);
	fullname = g_file_vita_get_full_path (filename);
	g_free (filename);

	return fullname;
}

gboolean CreateDirectory (const gunichar2 *name,
			  WapiSecurityAttributes *security)
{
	TRACE;
	char *fullname;
	int res;

	fullname = get_fullname (name);

	// FIXME: access
	res = pss_io_mkdir (fullname, PSS_STM_RWU);
	g_free (fullname);

	if (res != 0)
		set_error_from_vita_errno (res);

	return res == 0;
}

gboolean RemoveDirectory (const gunichar2 *name)
{
	TRACE;
	char *d = get_fullname (name);
	int res;

	res = pss_io_rmdir (d);
	g_free (d);
	if (res != 0) {
		set_error_from_vita_errno (res);
	}

	return res == 0;
}

typedef struct FindData {
	char *path;
	wapi_glob_t glob_state;
	int pos;
} FindData;

gpointer FindFirstFile (const gunichar2 *pattern, WapiFindData *find_data)
{
	TRACE;
	char *path = get_fullname (pattern);
	char *s, *p;
	int fd, res, len;
	FindData *data;
	GDir *dir;
	wapi_glob_t glob_state;

	s = strrchr (path, '/');
	if (!s) {
		printf ("FindFirstFile (): unsupported pattern '%s'\n", path);
		SetLastError (ERROR_NOT_SUPPORTED);
		return INVALID_HANDLE_VALUE;
	}
	p = s + 1;
	*s = '\0';

	/* Convert *.* to * */
	len = strlen (p);
	if (len > 2 && p [len - 2] == '.' && p [len - 1] == '*')
		p [len - 2] = '*';

	dir = g_dir_open (path, 0, NULL);
	if (!dir) {
		/* Get error code */
		fd = pss_io_dopen (path);
		g_assert (fd < 0);
		set_error_from_vita_errno (fd);
		return INVALID_HANDLE_VALUE;
	}
	res = _wapi_glob (dir, p, 0, &glob_state);
	g_dir_close (dir);

	if (res) {
		if (res == WAPI_GLOB_NOMATCH)
			SetLastError (ERROR_NO_MORE_FILES);
		else
			SetLastError (ERROR_NOT_SUPPORTED);
		return INVALID_HANDLE_VALUE;
	}
			
	if (!glob_state.gl_pathc) {
		_wapi_globfree (&glob_state);
		SetLastError (ERROR_NO_MORE_FILES);
		return INVALID_HANDLE_VALUE;
	}

	data = g_malloc0 (sizeof (FindData));
	data->path = path;
	memcpy (&data->glob_state, &glob_state, sizeof (glob_state));

	if (!FindNextFile (data, find_data)) {
		FindClose (data);
		SetLastError (ERROR_NO_MORE_FILES);
		data = INVALID_HANDLE_VALUE;
	}

	return data;
}

gboolean FindNextFile (gpointer handle, WapiFindData *find_data)
{
	TRACE;
	FindData *data = (FindData*)handle;
	int res;
	char *fname, *filename;
	glong ulen;
	PssIoStat stat;
	gunichar2 *chars;

	if (data->pos == data->glob_state.gl_pathc) {
		SetLastError (ERROR_NO_MORE_FILES);
		return FALSE;
	}

	fname = data->glob_state.gl_pathv [data->pos];
	filename = g_build_filename (data->path, fname, NULL);

	res = pss_io_getstat (filename, &stat);
	g_assert (res == 0);

	memset (find_data, 0, sizeof (*find_data));
	g_assert (strlen (fname) < MAX_PATH);

	chars = g_utf8_to_utf16 (fname, strlen (fname), NULL, &ulen, NULL);
	memcpy (find_data->cFileName, chars, ulen * 2);
	g_free (chars);

	if (stat.st_mode & PSS_STM_FDIR)
		find_data->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

	data->pos ++;

	return TRUE;
}

gboolean FindClose (gpointer handle)
{
	TRACE;
	FindData *data = (FindData*)handle;

	_wapi_globfree (&data->glob_state);

	g_free (data->path);

	return TRUE;
}

char* g_file_vita_get_current_dir (void);
char* g_file_vita_set_current_dir (char *dir);
gunichar2 * mono_unicode_from_external (const gchar *in, gsize *bytes);

guint32 GetCurrentDirectory (guint32 length, gunichar2 *buffer)
{
	TRACE;
	const gchar *dir;
	gsize bytes;
	gsize count;
	gunichar2 *utf16_path;

	dir = g_file_vita_get_current_dir ();
	utf16_path = mono_unicode_from_external (dir, &bytes);

	count = bytes / 2;

	if (length < count + 1) {
		g_free (utf16_path);
		return count + 1;
	}

	memcpy (buffer, utf16_path, bytes);
	memset (buffer + count, 0, 2);

	g_free (utf16_path);

	return count + 1;
}

gboolean SetCurrentDirectory (const gunichar2 *path)
{
	TRACE;
	char *p = mono_unicode_to_external (path);

	g_file_vita_set_current_dir (p);
	g_free (p);
	return TRUE;
}

gboolean MoveFile (const gunichar2 *name, const gunichar2 *dest_name)
{
	TRACE;
	char *srcname = get_fullname (name);
	char *dstname = get_fullname (dest_name);
	int res;

	if (!strcmp (srcname, dstname)) {
		g_free (srcname);
		g_free (dstname);
		return TRUE;
	}

	// FIXME: Moving between devices
	res = pss_io_rename (srcname, dstname);
	g_free (srcname);
	g_free (dstname);

	if (res < 0) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	return TRUE;
}

gboolean ReplaceFile (const gunichar2 *replacedFileName, const gunichar2 *replacementFileName,
			     const gunichar2 *backupFileName, guint32 replaceFlags, 
			     gpointer exclude, gpointer reserved)
{
	TRACE;
	SetLastError (ERROR_NOT_SUPPORTED);
	return FALSE;
}

gboolean CopyFile (const gunichar2 *name, const gunichar2 *dest_name,
		   gboolean fail_if_exists)
{
	TRACE;
	char *srcname = get_fullname (name);
	char *dstname = get_fullname (dest_name);
	int sfd, dfd, res;
	char buf[1024];

	if (!strcmp (srcname, dstname)) {
		g_free (srcname);
		g_free (dstname);
		SetLastError (ERROR_SHARING_VIOLATION);
		return FALSE;
	}

	sfd = pss_io_open (srcname, PSS_FREAD, 0);

	if (sfd < 0) {
		g_free (srcname);
		g_free (dstname);
		set_error_from_vita_errno (sfd);
		return FALSE;
	}

	dfd = pss_io_open (dstname, PSS_FWRITE|PSS_FCREAT|PSS_TRUNC, PSS_STM_RWU);
	if (dfd < 0) {
		g_free (srcname);
		g_free (dstname);
		pss_io_close (sfd);
		set_error_from_vita_errno (dfd);
		return FALSE;
	}

	g_free (srcname);
	g_free (dstname);

	while (TRUE) {
		res = pss_io_read (sfd, buf, 1024);
		// FIXME: Error vs. EOF detection ?
		if (res <= 0)
			break;
		pss_io_write (dfd, buf, res);
	}

	pss_io_close (sfd);
	pss_io_close (dfd);

	return TRUE;
}

gboolean DeleteFile(const gunichar2 *name)
{
	TRACE;
	char *fullname;
	int res;

	fullname = get_fullname (name);

	res = pss_io_remove (fullname);
	g_free (fullname);
	/* A missing file is not an error for DeleteFile () */
	if (res < 0 && res != PSS_ERROR_ERRNO_ENOENT) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	return TRUE;
}

guint32 GetFileAttributes (const gunichar2 *name)
{
	TRACE;
	char *fullname;
	PssIoStat stat;
	int res;

	fullname = get_fullname (name);

	res = pss_io_getstat (fullname, &stat);
	if (res < 0) {
		set_error_from_vita_errno (res);
		return INVALID_FILE_ATTRIBUTES;
	}

	g_free (fullname);

	if (stat.st_mode & PSS_STM_FREG)
		return FILE_ATTRIBUTE_NORMAL;
	else if (stat.st_mode & PSS_STM_FDIR)
		return FILE_ATTRIBUTE_DIRECTORY;
	else
		g_assert_not_reached ();
	return 0;
}

gboolean GetFileAttributesEx (const gunichar2 *name, WapiGetFileExInfoLevels level, gpointer info)
{
	TRACE;
	char *fullname;
	PssIoStat stat;
	int res;
	WIN32_FILE_ATTRIBUTE_DATA *data = info;
	uint64_t t;
	
	fullname = get_fullname (name);

	res = pss_io_getstat (fullname, &stat);
	if (res < 0) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	g_free (fullname);

	memset (data, 0, sizeof (*data));

	if (stat.st_mode & PSS_STM_FREG)
		data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	else if (stat.st_mode & PSS_STM_FDIR)
		data->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	data->nFileSizeLow = stat.st_size;

	pss_get_win32_filetime (&stat.st_ctime, &t);
	*(guint64*)&(data->ftCreationTime) = t;
	pss_get_win32_filetime (&stat.st_atime, &t);
	*(guint64*)&(data->ftLastAccessTime) = t;
	pss_get_win32_filetime (&stat.st_mtime, &t);
	*(guint64*)&(data->ftLastWriteTime) = t;

	return TRUE;
}

gboolean SetFileAttributes (const gunichar2 *name, guint32 attrs)
{
	TRACE;
	if (attrs & 0x80000000){
		/* mono extension, 'set executable bit' */
		return TRUE;
	} else {
		SetLastError (ERROR_NOT_SUPPORTED);
		return FALSE;
	}
}

WapiFileType GetFileType(gpointer handle)
{
	TRACE;
	unsigned int iH = (unsigned int)handle;
	if((iH == (unsigned int)STDIN_HANDLE)
	 ||(iH == (unsigned int)STDOUT_HANDLE)
	 ||(iH == (unsigned int)STDERR_HANDLE))
	{
		return FILE_TYPE_CHAR;
	}
	return FILE_TYPE_DISK;
}

gpointer _wapi_handle_new_fd (WapiHandleType type, int fd,
							  gpointer handle_specific);

gpointer _wapi_stdhandle_create (int fd, const gchar *name)
{
	struct _WapiHandle_file file_handle = {0};
	gpointer handle;
	
	file_handle.fileaccess = (fd == 0) ? GENERIC_READ : GENERIC_WRITE;

	file_handle.filename = g_strdup(name);
	/* some default security attributes might be needed */
	file_handle.security_attributes=0;

	/* Apparently input handles can't be written to.  (I don't
	 * know if output or error handles can't be read from.)
	 */
	if (fd == 0) {
		file_handle.fileaccess &= ~GENERIC_WRITE;
	}
	
	file_handle.sharemode=0;
	file_handle.attrs=0;

	handle = _wapi_handle_new_fd (WAPI_HANDLE_CONSOLE, fd, &file_handle);
	if (handle == _WAPI_HANDLE_INVALID) {
		SetLastError (ERROR_GEN_FAILURE);
		return(INVALID_HANDLE_VALUE);
	}
	
	return(handle);
}

gboolean _wapi_lookup_handle (gpointer handle, WapiHandleType type,
							  gpointer *handle_specific);
void _wapi_handle_ref (gpointer handle);

gpointer GetStdHandle(WapiStdHandle stdhandle)
{
	TRACE;
	struct _WapiHandle_file *file_handle;
	const gchar *name;
	int fd, ok;
	gpointer handle;

	// This is thread-safe
	files [STDOUT_HANDLE].file = stdout;
	files [STDIN_HANDLE].file = stdin;
	files [STDERR_HANDLE].file = stderr;

	files [STDOUT_HANDLE].dir_fd = -1;
	files [STDIN_HANDLE].dir_fd = -1;
	files [STDERR_HANDLE].dir_fd = -1;

	switch(stdhandle) {
	case STD_OUTPUT_HANDLE :
		fd = STDOUT_HANDLE;
		name = "<stdout>";
		break;
	case STD_INPUT_HANDLE :
		fd = STDIN_HANDLE;
		name = "<stdin>";
		break;
	case STD_ERROR_HANDLE :
		fd = STDERR_HANDLE;
		name = "<stderr>";
		break;
	default:
		SetLastError (ERROR_INVALID_PARAMETER);
		return(INVALID_HANDLE_VALUE);
	}

	handle = GINT_TO_POINTER (fd);

	// FIXME: locking

	ok = _wapi_lookup_handle (handle, WAPI_HANDLE_CONSOLE,
				  (gpointer *)&file_handle);
	if (ok == FALSE) {
		/* Need to create this console handle */
		handle = _wapi_stdhandle_create (fd, name);
		
		if (handle == INVALID_HANDLE_VALUE) {
			SetLastError (ERROR_NO_MORE_FILES);
			return INVALID_HANDLE_VALUE;
		}
	} else {
		/* Add a reference to this handle */
		_wapi_handle_ref (handle);
	}

	return handle;
}

gpointer CreateFile(const gunichar2 *name, guint32 fileaccess,
		    guint32 sharemode, WapiSecurityAttributes *security,
		    guint32 createmode, guint32 attrs,
		    gpointer template G_GNUC_UNUSED)
{
	TRACE;
	FILE *f;
	char *fullname;
	const char *access;
	PssIoStat stat;
	int fd, res;
	struct _WapiHandle_file file_handle = {0};
	gpointer handle;
	int dir_fd = -1;
	CryptoContext *context = NULL;

	fullname = get_fullname (name);

	if (get_crypto_policy != NULL) {
		int policy;

		get_crypto_policy (fullname, &policy);

		if (policy == WAPI_CRYPTO_POLICY_EDATA) {
			context = (CryptoContext *) g_malloc (sizeof (CryptoContext));
		}
	}	
	// FIXME: attributes

	if (fileaccess == GENERIC_READ && createmode == OPEN_EXISTING) {
		access = "r";
	} else if (fileaccess == GENERIC_WRITE && createmode == CREATE_ALWAYS) {
		access = "w";
	} else if (fileaccess == (GENERIC_WRITE|GENERIC_READ) && createmode == CREATE_ALWAYS) {
		access = "w+";
	} else if (fileaccess == (GENERIC_READ|GENERIC_WRITE) && createmode == OPEN_EXISTING) {
		access = "r+";
	} else if (fileaccess == (GENERIC_WRITE) && createmode == OPEN_EXISTING) {
		access = "r+";
	} else if (fileaccess == (GENERIC_READ|GENERIC_WRITE) && createmode == CREATE_NEW) {
		access = "w";
	} else if (fileaccess == (GENERIC_WRITE) && createmode == CREATE_NEW) {
		access = "w";
	} else if (fileaccess == (GENERIC_WRITE) && createmode == OPEN_ALWAYS) {
		access = "r+";
	} else if (fileaccess == (GENERIC_READ|GENERIC_WRITE) && createmode == OPEN_ALWAYS) {
		access = "r+";
	} else if (fileaccess == (GENERIC_READ) && createmode == OPEN_ALWAYS) {
		access = "r+";
	} else if (fileaccess == (GENERIC_READ|GENERIC_WRITE) && createmode == OPEN_ALWAYS) {
		access = "r+";
	} else if (fileaccess == (GENERIC_READ|GENERIC_WRITE) && createmode == TRUNCATE_EXISTING) {
		access = "r+";
	} else if (fileaccess == (GENERIC_WRITE) && createmode == TRUNCATE_EXISTING) {
		access = "w+";
	} else {
		printf ("%s %x %d %d %d\n", fullname, fileaccess, sharemode, createmode, attrs);
		g_assert_not_reached ();
		return _WAPI_HANDLE_INVALID;
	}

	switch (createmode) {
	case CREATE_NEW:
		// FIXME: This is racy
		if (context != NULL) {
			SetLastError (ERROR_NOT_SUPPORTED);
			return _WAPI_HANDLE_INVALID;
		}
		res = pss_io_getstat (fullname, &stat);
		if (res == 0) {
			SetLastError (ERROR_FILE_EXISTS);
			return _WAPI_HANDLE_INVALID;
		}
		f = fopen (fullname, access);
		break;
	case OPEN_ALWAYS:
		/* Open if exists, create otherwise */
		res = pss_io_getstat (fullname, &stat);
		if (res == 0) {
			SetLastError (ERROR_ALREADY_EXISTS);
			if (context != NULL) {
				pss_crypto_open ((PssCryptoContext *) context, fullname);
			} else {
				f = fopen (fullname, access);
			}
		} else {
			if (context != NULL) {
				SetLastError (ERROR_NOT_SUPPORTED);
				return _WAPI_HANDLE_INVALID;
			}
			f = fopen (fullname, "w");
		}
		break;
	case TRUNCATE_EXISTING:
		/* Truncate if exists, fail otherwise */
		if (context != NULL) {
			SetLastError (ERROR_NOT_SUPPORTED);
			return _WAPI_HANDLE_INVALID;
		}
		res = pss_io_getstat (fullname, &stat);
		if (res) {
			SetLastError (ERROR_FILE_NOT_FOUND);
			return _WAPI_HANDLE_INVALID;
		} else {
			f = fopen (fullname, access);
		}
		break;
	default:
		{
			if (context != NULL) {
				pss_crypto_open ((PssCryptoContext *) context, fullname);
			} else {
				f = fopen (fullname, access);
			}
			break;
		}
	}

	if (!f && !context) {
		/* Maybe its a directory, MonoIO.SetFileTime () requires us to support this */
		dir_fd = pss_io_dopen (fullname);
		if (dir_fd < 0) {
			g_free (fullname);
			// FIXME: error code
			return INVALID_HANDLE_VALUE;
		}
	}

	file_handle.filename = mono_unicode_to_external (name);
	file_handle.fileaccess=fileaccess;
	file_handle.sharemode=sharemode;
	file_handle.attrs=attrs;

	fd = (int)create_handle ();
	g_assert (fd != -1);
	if (dir_fd != -1) {
		files [fd].file = (FILE*)-1;
		files [fd].dir_fd = dir_fd;
	} else {
		files [fd].file = f;
		files [fd].dir_fd = -1;
	}
	files [fd].name = g_strdup (fullname);
	files [fd].attrs = attrs;
	if (context != NULL) {
		files [fd].context = context;
	}

	g_free (fullname);

	handle = _wapi_handle_new_fd (WAPI_HANDLE_FILE, fd, &file_handle);

	return handle;
}

gboolean ReadFile(gpointer handle, gpointer buffer, guint32 numbytes,
		  guint32 *bytesread, WapiOverlapped *overlapped)
{
	TRACE;
	VitaFile *vf = get_handle_data (handle);
	int res;

	if (vf->context != NULL) {
		res = pss_crypto_fread ((PssCryptoContext *) vf->context, buffer, numbytes);
	} else {
		res = fread (buffer, 1, numbytes, vf->file);
	}
	*bytesread = res;

	return TRUE;
}

guint32 GetFileSize(gpointer handle, guint32 *highsize)
{
	TRACE;
	VitaFile *vf = get_handle_data (handle);
	long res, cur;

	*highsize = 0;

	if (vf->context != NULL) {
		res = vf->context->size;
	} else {
		cur = ftell (vf->file);
		fseek (vf->file, 0, SEEK_END);
		res = ftell (vf->file);
		fseek (vf->file, cur, SEEK_SET);
	}

	return res;
}

gboolean SetFileTime(gpointer handle, const WapiFileTime *create_time,
		     const WapiFileTime *last_access,
		     const WapiFileTime *last_write)
{
	TRACE;
	char *name = get_name_for_handle (handle);
	PssIoStat stat;
	int res, flags;

	res = pss_io_getstat (name, &stat);
	if (res) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	flags = 0;
	if (create_time) {
		flags |= PSS_CST_CT;
		pss_set_win32_filetime (&stat.st_ctime, *(guint64*)create_time);
	}
	if (last_access) {
		flags |= PSS_CST_AT;
		pss_set_win32_filetime (&stat.st_atime, *(guint64*)last_access);
	}
	if (last_write) {
		flags |= PSS_CST_MT;
		pss_set_win32_filetime (&stat.st_mtime, *(guint64*)last_write);
	}

	// FIXME: pss_io_chstat () sometimes returns ENACCESS
	res = pss_io_chstat (name, &stat, flags);
	if (res) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	return TRUE;
}

gboolean CreatePipe (gpointer *readpipe, gpointer *writepipe,
		     WapiSecurityAttributes *security G_GNUC_UNUSED, guint32 size)
{
	TRACE;
	SetLastError (ERROR_NOT_SUPPORTED);
	return FALSE;
}

gboolean FlushFileBuffers(gpointer handle)
{
	TRACE;
	FILE *f = get_file_for_handle (handle);

	fflush(f);
	return TRUE;
}

guint32 GetTempPath (guint32 len, gunichar2 *buf)
{
	TRACE;
	g_assert (len >= 2);
	buf [0] = '/';
	buf [1] = 0;

	return 2;
}

gboolean LockFile (gpointer handle, guint32 offset_low,
		  guint32 offset_high, guint32 length_low,
		  guint32 length_high)
{
	SetLastError (ERROR_NOT_SUPPORTED);
	return FALSE;
}

gboolean UnlockFile (gpointer handle, guint32 offset_low,
			    guint32 offset_high, guint32 length_low,
			    guint32 length_high)
{
	SetLastError (ERROR_NOT_SUPPORTED);
	return FALSE;
}

gboolean WriteFile(gpointer handle, gconstpointer buffer,
			  guint32 numbytes, guint32 *byteswritten,
			  WapiOverlapped *overlapped)
{
	TRACE;
	FILE *f = get_file_for_handle (handle);
	int res;

	if (numbytes == 0) {
		if(byteswritten != NULL)
			*byteswritten = numbytes;
		return TRUE;
	}

	res = fwrite (buffer, 1, numbytes, f);

	if (res != numbytes)
		return FALSE;
	if (byteswritten != NULL)
		*byteswritten = numbytes;

	if ((int)handle == STDOUT_HANDLE || (int)handle == STDERR_HANDLE)
		/* These seem to be buffered on the VITA */
		fflush (f);

	return TRUE;
}

guint32 SetFilePointer(gpointer handle, gint32 movedistance,
			      gint32 *highmovedistance, WapiSeekMethod method)
{
	TRACE;
	FILE *f;
	int whence;
	long res;

	if (!check_handle (handle))
		return INVALID_SET_FILE_POINTER;
	f = get_file_for_handle (handle);
	if (highmovedistance)
		g_assert (*highmovedistance == 0);

	switch(method) {
	case FILE_BEGIN:
		whence=SEEK_SET;
		break;
	case FILE_CURRENT:
		whence=SEEK_CUR;
		break;
	case FILE_END:
		whence=SEEK_END;
		break;
	default:
		g_assert_not_reached ();
	}

	res = fseek (f, movedistance, whence);
	if (res)
		return INVALID_SET_FILE_POINTER;
	else
		return ftell (f);
}

gboolean SetEndOfFile(gpointer handle)
{
	TRACE;
	char *name = get_name_for_handle (handle);
	FILE *f = get_file_for_handle (handle);
	PssIoStat stat;
	int res;
	gint64 pos;

	pos = ftell (f);

	stat.st_size = pos;
	res = pss_io_chstat (name, &stat, PSS_CST_SIZE);
	if (res) {
		set_error_from_vita_errno (res);
		return FALSE;
	}

	return TRUE;
}

static void
file_close (gpointer handle, gpointer data)
{
	TRACE;
	VitaFile *vf = get_handle_data (handle);
	int attrs = get_attrs_for_handle (handle);
	char *name = get_name_for_handle (handle);

	if (vf->dir_fd != -1)
		pss_io_dclose (vf->dir_fd);
	else if (vf->context != NULL) {
		pss_crypto_close (vf->context);
		g_free (vf->context);
		vf->context = NULL;
	} else
		fclose (vf->file);

	if (attrs & FILE_FLAG_DELETE_ON_CLOSE)
		pss_io_remove (name);

	delete_handle (handle);
}

struct _WapiHandleOps _wapi_file_ops = {
	file_close,     /* close */
	NULL,			/* signal */
	NULL,			/* own */
	NULL,			/* is_owned */
	NULL,			/* special_wait */
	NULL			/* prewait */
};

struct _WapiHandleOps _wapi_console_ops = {
	NULL, // console_close,		/* close */
	NULL,			/* signal */
	NULL,			/* own */
	NULL,			/* is_owned */
	NULL,			/* special_wait */
	NULL			/* prewait */
};
