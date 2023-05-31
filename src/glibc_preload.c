#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>

#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#if defined(__amd64__)
	#define LD "ld-linux-x86-64.so.2"
#elif defined(__aarch64__)
	#define LD "ld-linux-aarch64.so.1"
#elif defined(__arm__)
	#define LD "ld-linux.so.3"
#elif defined(__s390x__)
	#define LD "ld64.so.1"
#elif defined(__powerpc64__)
	#define LD "ld64.so.2"
#elif defined(__hppa__) || defined(__mips__) || defined(__s390__) || defined(__powerpc__)
	#define LD "ld.so.1"
#elif defined(__i386__) || defined(__sparc__) || defined(__alpha__)
	#define LD "ld-linux.so.2"
#else
	#error "Unsupported system architecture"
#endif

static const char PROGRAM_INTERPRETER[] = STRINGIFY(GLIBC_INSTALL_PREFIX) "/lib/" LD;

static const char* const LIBRARY_PATH[] = {
	STRINGIFY(GLIBC_INSTALL_PREFIX) "/lib64",
	STRINGIFY(GLIBC_INSTALL_PREFIX) "/lib",
	"/usr/local/lib64",
	"/usr/local/lib",
	"/lib64",
	"/lib",
	"/usr/lib64",
	"/usr/lib"
};

int execve(const char* pathname, char* const argv[], char* const envp[]) {
	
	int (*__execve__)(const char* pathname, char* const argv[], char* const envp[]) = dlsym(RTLD_NEXT, "execve");
	
	if (__execve__ == NULL) {
		return -1;
	}
	
	int fd = open(pathname, O_RDONLY);
	
	if (fd == -1) {
		return -1;
	}
	
	char buffer[4] = {'\0'};
	const ssize_t size = read(fd, buffer, sizeof(buffer));
	
	if (size == -1) {
		const int cerrno = errno;
		close(fd);
		
		errno = cerrno;
		return -1;
	}
	
	close(fd);
	
	const char elf_magic_numbers[] = {0x7f, 0x45, 0x4c, 0x46};
	
	if (memcmp(buffer, elf_magic_numbers, sizeof(elf_magic_numbers)) != 0) {
		return (*__execve__)(pathname, argv, envp);
	}
	
	size_t argc = 0;
	
	while (1) {
		const char* const item = argv[argc];
		
		if (item == NULL) {
			break;
		}
		
		argc++;
	}
	
	const char* const npathname = PROGRAM_INTERPRETER;
	
	size_t buffs = 0;
	
	for (size_t index = 0; index < sizeof(LIBRARY_PATH) / sizeof(*LIBRARY_PATH); index++) {
		const char* const item = LIBRARY_PATH[index];
		
		buffs++;
		buffs += strlen(item);
	}
	
	char library_path[buffs + 1];
	library_path[0] = '\0';
	
	for (size_t index = 0; index < sizeof(LIBRARY_PATH) / sizeof(*LIBRARY_PATH); index++) {
		const char* const item = LIBRARY_PATH[index];
		
		strcat(library_path, item);
		
		if ((index + 1) < (sizeof(LIBRARY_PATH) / sizeof(*LIBRARY_PATH))) {
			strcat(library_path, ":");
		}
	}
	
	char* nargv[argc + 4];
	
	nargv[0] = (char*) npathname;
	nargv[1] = "--library-path";
	nargv[2] = library_path;
	nargv[3] = pathname;
	
	memcpy(&nargv[4], &argv[1], argc * sizeof(*argv));
	
	const int code = (*__execve__)(npathname, nargv, envp);
	return code;
	
}
