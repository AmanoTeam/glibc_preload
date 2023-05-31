cmake_minimum_required(VERSION 3.13)

project(
	glibc_preload
	VERSION 0.1
	DESCRIPTION "A execve() wrapper that allows executing binaries using a glibc installed on a non stardard location"
	HOMEPAGE_URL "https://github.com/AmanoTeam/glibc_preload"
	LANGUAGES C
)

add_library(
	glibcpreload
	SHARED
	src/glibc_preload.c
)

target_compile_definitions(
	glibcpreload
	PRIVATE
	GLIBC_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

install(
	TARGETS glibcpreload
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib NAMELINK_SKIP
)
