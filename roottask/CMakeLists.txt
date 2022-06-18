project(roottask-test C)

# warn about everything
# add_compile_options(-Wall -W -Wextra)
# enable stack unwind tables -- this allows backtrace() to work. (this is an AOS thing)
# add_compile_options(-funwind-tables)

find_package(seL4 REQUIRED)
find_package(elfloader-tool REQUIRED)
find_package(musllibc REQUIRED)
find_package(util_libs REQUIRED)
find_package(seL4_libs REQUIRED)

# sel4_import_kernel()
elfloader_import_project()

# This sets up environment build flags and imports musllibc and runtime libraries.
# Required if you want any std C libraries!
musllibc_setup_build_environment_with_sel4runtime()
sel4_import_libsel4()
util_libs_import_libraries()
sel4_libs_import_libraries()

# TODO: make CPIO of userspace apps
# list of apps to include in the cpio archive
# get_property(apps GLOBAL PROPERTY apps_property)
# MakeCPIO(archive.o "${apps}")

add_executable(
    roottask-test
    EXCLUDE_FROM_ALL
    main.c
)
# target_include_directories(roottask-test PRIVATE "include")
target_link_libraries(
    roottask-test
    sel4
    muslc
    sel4muslcsys
    sel4utils
    sel4allocman
    elf
    cpio
    utils
    # serial
    # clock
    # sel4cspace
)

# Set this program as the rootserver (initial task/thread) for the kernel
DeclareRootserver(roottask-test)