# seL4-roottask-test

Testing ground for developing a root task on the seL4 microkernel from scratch, with deployment targets of QEMU, and ideally also the Odroid-C2 and Raspberrry  Pi 3 or 4.

Based off the build system structures in:
* the [UNSW Advanced Operating Systems course](https://www.cse.unsw.edu.au/~cs9242/current/)'s starter project
    * code and build system https://github.com/SEL4PROJ/AOS
    * repo manifest https://github.com/SEL4PROJ/AOS-manifest/
* sel4bench
    * code and build system https://github.com/seL4/sel4bench
    * repo manifest https://github.com/seL4/sel4bench-manifest/
* sel4test
    * code and build system https://github.com/seL4/sel4test/
    * repo manifest https://github.com/seL4/sel4test-manifest/
* sel4-tutorials
    * tutorial code and build system generator https://github.com/seL4/sel4-tutorials/
    * repo manifest https://github.com/seL4/sel4-tutorials-manifest/

## Getting Started

This project, like all the others mentioned above, is structured using the [Google 'repo' tool](https://gerrit.googlesource.com/git-repo/) for wrangling many git repos together at once. See https://docs.sel4.systems/projects/buildsystem/repo-cheatsheet.html for some notes from the seL4 people on using repo.

Normally this requires a 'manifest' repository that houses a repo XML file to describe all the repos to pull down, but to keep things simpler for now, the manifest file has just been put here below:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!--
    Based off sel4test manifest
-->
<manifest>
  <remote name="seL4" fetch="https://github.com/seL4"/>
  <remote name="nuclearpidgeon" fetch="https://github.com/nuclearpidgeon"/>
  <default remote="seL4" revision="master"/>

  <project name="musllibc.git" path="projects/musllibc" revision="3d6b939e8f05cb1d2a1a8c8166609bf2e652e975" upstream="sel4" dest-branch="sel4"/>
  <project name="seL4.git" path="kernel" revision="756a37b7d3c07de927fad58a25f907190c2b9232" upstream="master" dest-branch="master"/>
  <project name="seL4_libs.git" path="projects/seL4_libs" revision="d9e53b132c32c974909078c6eb7f6d19c316e389" upstream="master" dest-branch="master"/>
  <project name="seL4_tools.git" path="tools/seL4" revision="3234b1fb94dea6525e3ba3df205a453baf63c1ae" upstream="master" dest-branch="master">
    <linkfile src="cmake-tool/init-build.sh" dest="init-build.sh"/>
    <linkfile src="cmake-tool/griddle" dest="griddle"/>
  </project>
  <project name="sel4runtime.git" path="projects/sel4runtime" revision="d935dd05da0cf959e9fd0140af913dc6fdaa0221" upstream="master" dest-branch="master"/>
  <project name="util_libs.git" path="projects/util_libs" revision="f5838745c011af49ae5c3241ce560207e6dedcec" upstream="master" dest-branch="master"/>

  <project name="sel4-roottask-test" remote="nuclearpidgeon" path="projects/roottask-test" revision="main"/>
</manifest>
```

1. Put this code in a `default.xml` file in a `seL4-roottask-test-manifest` folder
1. `git init`, `git add -A`, and `git commit` to make the folder a proper repo. (the repo tool seems to require operating from a git repo annoyingly)
1. Move to a new `seL4-roottask-test` folder and run `repo init -u /path/to/seL4-roottask-test-manifest`
1. Then run `repo sync` to pull down all of the repositories into the folder structure required for the project

The project can then be built with CMake and ninja. You can set up all the build dependencies on your host system yourself if you want, or just use the [Dockerfiles provided/maintained by the seL4 foundation](https://github.com/seL4/seL4-CAmkES-L4v-dockerfiles) to get a Linux container environment that has it all ready to go.

### x86 QEMU build example

1. Make a new directory inside `seL4-roottask-test` for housing a build, such as `build_qemu_x86`
1. Run CMake as follows: `cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE="../kernel/gcc.cmake" -DSEL4_CACHE_DIR="../.sel4_cache" -DPLATFORM="ia32" -C "../projects/roottask-test/settings.cmake" ../projects/roottask-test`. If everything has lined up properly, this should generate a `build.ninja` file with everything required to compile both the kernel and the root task into a machine image.
1. Run `ninja` to do all the actual compilation. The output will be two image files put into the `images` directory.
1. Run `./simulate` to run QEMU with the compiled images
