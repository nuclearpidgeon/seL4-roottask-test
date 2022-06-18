# Adapted from UNSW AOS project and sel4test's settings.cmake files
# https://github.com/SEL4PROJ/AOS/

cmake_minimum_required(VERSION 3.7.2)

# build all libs as static
# not sure if this is needed...
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}/../../")
file(GLOB project_modules ${project_dir}/projects/*)
list(
    APPEND
        CMAKE_MODULE_PATH
        ${project_dir}/kernel
        ${project_dir}/tools/seL4/cmake-tool/helpers/
        ${project_dir}/tools/seL4/elfloader-tool/
        ${project_modules}
)

set(SEL4_CONFIG_DEFAULT_ADVANCED ON)

include(application_settings)

correct_platform_strings()

find_package(seL4 REQUIRED)
sel4_configure_platform_settings()

set(valid_platforms "ia32" "qemu-arm-virt" "odroidc2" "rpi4")
set_property(CACHE PLATFORM PROPERTY STRINGS ${valid_platforms})
if(NOT "${PLATFORM}" IN_LIST valid_platforms)
message(FATAL_ERROR "Invalid PLATFORM selected: \"${PLATFORM}\"
Valid platforms are: \"${valid_platforms}\"")
endif()

include(${project_dir}/kernel/configs/seL4Config.cmake)

# Pull in required CMake modules from seL4tools/cmake-tool
include(rootserver)
include(simulation)

sel4_import_kernel()

# Elfloader settings that correspond to how Data61 sets its boards up.
ApplyData61ElfLoaderSettings(${KernelPlatform} ${KernelSel4Arch})

# turn on all the nice features for debugging
# TODO for benchmarking, these should be turned off
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
set(KernelVerificationBuild OFF CACHE BOOL "" FORCE)
set(KernelIRQReporting ON CACHE BOOL "" FORCE)
set(KernelPrinting ON CACHE BOOL "" FORCE)
set(KernelDebugBuild ON CACHE BOOL "" FORCE)