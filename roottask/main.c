#include <autoconf.h>
#include <utils/util.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <sel4/sel4.h>
#include <sel4runtime.h>
#include <sel4platsupport/bootinfo.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <allocman/bootstrap.h>
#include <allocman/vka.h>


// Globals
seL4_BootInfo *info;
simple_t simple;
vka_t vka;
allocman_t *allocman;

int main(void)
{
    setbuf(stdout, NULL);
    printf("Hello!\n");
    int error;
    info = platsupport_get_bootinfo();
    debug_print_bootinfo(info);
#ifdef CONFIG_DEBUG_BUILD
    seL4_DebugNameThread(seL4_CapInitThreadTCB, "sel4-roottask-test");
#endif

    /* initialise libsel4simple, which abstracts away which kernel version
     * we are running on */
    simple_default_init_bootinfo(&simple, info);

    printf("Bye!\n");
    // 'Quit' by suspending the root thread
    seL4_TCB_Suspend(seL4_CapInitThreadTCB);
}