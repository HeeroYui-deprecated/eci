/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int ZeroValue = 0;

void UnistdAccess(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = access(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void UnistdAlarm(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = alarm(_param[0]->Val->Integer);
}

void UnistdChdir(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = chdir(_param[0]->Val->Pointer);
}

void UnistdChroot(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = chroot(_param[0]->Val->Pointer);
}

void UnistdChown(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = chown(_param[0]->Val->Pointer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void UnistdClose(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = close(_param[0]->Val->Integer);
}

void UnistdConfstr(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = confstr(_param[0]->Val->Integer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void UnistdCtermid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = ctermid(_param[0]->Val->Pointer);
}

void UnistdDup(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = dup(_param[0]->Val->Integer);
}

void UnistdDup2(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = dup2(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void Unistd_Exit(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_exit(_param[0]->Val->Integer);
}

void UnistdFchown(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fchown(_param[0]->Val->Integer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void UnistdFchdir(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fchdir(_param[0]->Val->Integer);
}

void UnistdFdatasync(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fdatasync(_param[0]->Val->Integer);
}

void UnistdFork(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fork();
}

void UnistdFpathconf(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fpathconf(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdFsync(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = fsync(_param[0]->Val->Integer);
}

void UnistdFtruncate(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = ftruncate(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdGetcwd(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = getcwd(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void UnistdGetdtablesize(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getdtablesize();
}

void UnistdGetegid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getegid();
}

void UnistdGeteuid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = geteuid();
}

void UnistdGetgid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getgid();
}

void UnistdGethostid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = gethostid();
}

void UnistdGetlogin(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = getlogin();
}

void UnistdGetlogin_r(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getlogin_r(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void UnistdGetpagesize(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getpagesize();
}

void UnistdGetpass(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = getpass(_param[0]->Val->Pointer);
}

void UnistdGetpgrp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getpgrp();
}

void UnistdGetpid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getpid();
}

void UnistdGetppid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getppid();
}

void UnistdGetuid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = getuid();
}

void UnistdGetwd(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = getcwd(_param[0]->Val->Pointer, PATH_MAX);
}

void UnistdIsatty(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isatty(_param[0]->Val->Integer);
}

void UnistdLchown(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = lchown(_param[0]->Val->Pointer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void UnistdLink(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = link(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void UnistdLockf(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = lockf(_param[0]->Val->Integer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void UnistdLseek(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = lseek(_param[0]->Val->Integer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void UnistdNice(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = nice(_param[0]->Val->Integer);
}

void UnistdPathconf(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = pathconf(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void UnistdPause(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = pause();
}

void UnistdRead(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = read(_param[0]->Val->Integer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void UnistdReadlink(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = readlink(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void UnistdRmdir(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = rmdir(_param[0]->Val->Pointer);
}

void UnistdSbrk(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = sbrk(_param[0]->Val->Integer);
}

void UnistdSetgid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setgid(_param[0]->Val->Integer);
}

void UnistdSetpgid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setpgid(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdSetpgrp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setpgrp();
}

void UnistdSetregid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setregid(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdSetreuid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setreuid(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdSetsid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setsid();
}

void UnistdSetuid(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = setuid(_param[0]->Val->Integer);
}

void UnistdSleep(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = sleep(_param[0]->Val->Integer);
}

void UnistdSymlink(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = symlink(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void UnistdSync(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	sync();
}

void UnistdSysconf(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = sysconf(_param[0]->Val->Integer);
}

void UnistdTcgetpgrp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = tcgetpgrp(_param[0]->Val->Integer);
}

void UnistdTcsetpgrp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = tcsetpgrp(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdTruncate(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = truncate(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void UnistdTtyname(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = ttyname(_param[0]->Val->Integer);
}

void UnistdTtyname_r(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = ttyname_r(_param[0]->Val->Integer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void UnistdUalarm(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = ualarm(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void UnistdUnlink(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = unlink(_param[0]->Val->Pointer);
}

void UnistdUsleep(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = usleep(_param[0]->Val->Integer);
}

void UnistdVfork(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = vfork();
}

void UnistdWrite(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = write(_param[0]->Val->Integer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}


/* handy structure definitions */
const char UnistdDefs[] = "\
typedef int uid_t; \
typedef int gid_t; \
typedef int pid_t; \
typedef int off_t; \
typedef int size_t; \
typedef int ssize_t; \
typedef int useconds_t;\
typedef int intptr_t;\
";

/* all unistd.h functions */
struct LibraryFunction UnistdFunctions[] = {
	{ UnistdAccess,        "int access(char *, int);" },
	{ UnistdAlarm,         "unsigned int alarm(unsigned int);" },
	{ UnistdChdir,         "int chdir(char *);" },
	{ UnistdChroot,        "int chroot(char *);" },
	{ UnistdChown,         "int chown(char *, uid_t, gid_t);" },
	{ UnistdClose,         "int close(int);" },
	{ UnistdConfstr,       "size_t confstr(int, char *, size_t);" },
	{ UnistdCtermid,       "char *ctermid(char *);" },
	{ UnistdDup,           "int dup(int);" },
	{ UnistdDup2,          "int dup2(int, int);" },
	{ Unistd_Exit,         "void _exit(int);" },
	{ UnistdFchown,        "int fchown(int, uid_t, gid_t);" },
	{ UnistdFchdir,        "int fchdir(int);" },
	{ UnistdFdatasync,     "int fdatasync(int);" },
	{ UnistdFork,          "pid_t fork(void);" },
	{ UnistdFpathconf,     "long fpathconf(int, int);" },
	{ UnistdFsync,         "int fsync(int);" },
	{ UnistdFtruncate,     "int ftruncate(int, off_t);" },
	{ UnistdGetcwd,        "char *getcwd(char *, size_t);" },
	{ UnistdGetdtablesize, "int getdtablesize(void);" },
	{ UnistdGetegid,       "gid_t getegid(void);" },
	{ UnistdGeteuid,       "uid_t geteuid(void);" },
	{ UnistdGetgid,        "gid_t getgid(void);" },
	{ UnistdGethostid,     "long gethostid(void);" },
	{ UnistdGetlogin,      "char *getlogin(void);" },
	{ UnistdGetlogin_r,    "int getlogin_r(char *, size_t);" },
	{ UnistdGetpagesize,   "int getpagesize(void);" },
	{ UnistdGetpass,       "char *getpass(char *);" },
	{ UnistdGetpgrp,       "pid_t getpgrp(void);" },
	{ UnistdGetpid,        "pid_t getpid(void);" },
	{ UnistdGetppid,       "pid_t getppid(void);" },
	{ UnistdGetuid,        "uid_t getuid(void);" },
	{ UnistdGetwd,         "char *getwd(char *);" },
	{ UnistdIsatty,        "int isatty(int);" },
	{ UnistdLchown,        "int lchown(char *, uid_t, gid_t);" },
	{ UnistdLink,          "int link(char *, char *);" },
	{ UnistdLockf,         "int lockf(int, int, off_t);" },
	{ UnistdLseek,         "off_t lseek(int, off_t, int);" },
	{ UnistdNice,          "int nice(int);" },
	{ UnistdPathconf,      "long pathconf(char *, int);" },
	{ UnistdPause,         "int pause(void);" },
	{ UnistdRead,          "ssize_t read(int, void *, size_t);" },
	{ UnistdReadlink,      "int readlink(char *, char *, size_t);" },
	{ UnistdRmdir,         "int rmdir(char *);" },
	{ UnistdSbrk,          "void *sbrk(intptr_t);" },
	{ UnistdSetgid,        "int setgid(gid_t);" },
	{ UnistdSetpgid,       "int setpgid(pid_t, pid_t);" },
	{ UnistdSetpgrp,       "pid_t setpgrp(void);" },
	{ UnistdSetregid,      "int setregid(gid_t, gid_t);" },
	{ UnistdSetreuid,      "int setreuid(uid_t, uid_t);" },
	{ UnistdSetsid,        "pid_t setsid(void);" },
	{ UnistdSetuid,        "int setuid(uid_t);" },
	{ UnistdSleep,         "unsigned int sleep(unsigned int);" },
	{ UnistdSymlink,       "int symlink(char *, char *);" },
	{ UnistdSync,          "void sync(void);" },
	{ UnistdSysconf,       "long sysconf(int);" },
	{ UnistdTcgetpgrp,     "pid_t tcgetpgrp(int);" },
	{ UnistdTcsetpgrp,     "int tcsetpgrp(int, pid_t);" },
	{ UnistdTruncate,      "int truncate(char *, off_t);" },
	{ UnistdTtyname,       "char *ttyname(int);" },
	{ UnistdTtyname_r,     "int ttyname_r(int, char *, size_t);" },
	{ UnistdUalarm,        "useconds_t ualarm(useconds_t, useconds_t);" },
	{ UnistdUnlink,        "int unlink(char *);" },
	{ UnistdUsleep,        "int usleep(useconds_t);" },
	{ UnistdVfork,         "pid_t vfork(void);" },
	{ UnistdWrite,         "ssize_t write(int, void *, size_t);" },
	{ NULL, NULL }
};

/* creates various system-dependent definitions */
void UnistdSetupFunc() {
	/* define NULL */
	if (!VariableDefined(TableStrRegister("NULL"))) {
		VariableDefinePlatformVar(NULL, "NULL", &IntType, (union AnyValue *)&ZeroValue, FALSE);
	}
	/* define optarg and friends */
	VariableDefinePlatformVar(NULL, "optarg", CharPtrType, (union AnyValue *)&optarg, TRUE);
	VariableDefinePlatformVar(NULL, "optind", &IntType, (union AnyValue *)&optind, TRUE);
	VariableDefinePlatformVar(NULL, "opterr", &IntType, (union AnyValue *)&opterr, TRUE);
	VariableDefinePlatformVar(NULL, "optopt", &IntType, (union AnyValue *)&optopt, TRUE);
}

#endif /* !BUILTIN_MINI_STDLIB */

