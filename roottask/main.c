#include <autoconf.h>
#include <utils/util.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <sel4/sel4.h>
#include <sel4runtime.h>
#include <sel4platsupport/bootinfo.h>
#include <sel4platsupport/platsupport.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <allocman/bootstrap.h>
#include <allocman/vka.h>

#include <sel4utils/vspace.h>

// Globals
seL4_BootInfo *binfo;
simple_t simple;
vka_t vka;
vspace_t vspace;
allocman_t *allocman;

// Static buffers

/* dimensions of virtual memory for the allocator to use */
#define ALLOCATOR_VIRTUAL_POOL_SIZE ((1 << seL4_PageBits) * 100)

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 20)

static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* static memory for virtual memory bootstrapping */
static sel4utils_alloc_data_t vspace_alloc_data;

int main(void)
{
    binfo = platsupport_get_bootinfo();
    int error;

    printf("Hello from main.c!\n");

    // debug_print_bootinfo(bootinfo);

#ifdef CONFIG_DEBUG_BUILD
    seL4_DebugNameThread(seL4_CapInitThreadTCB, "sel4-roottask-test");
#endif

    /* initialise libsel4simple, which abstracts away which kernel version
     * we are running on */
    simple_default_init_bootinfo(&simple, binfo);

    /* create an allocator */
    allocman = bootstrap_use_current_simple(&simple, ALLOCATOR_STATIC_POOL_SIZE, allocator_mem_pool);
    if (allocman == NULL) {
        ZF_LOGF("Failed to create allocman");
    }

    /* create a vka (interface for interacting with the underlying allocator) */
    allocman_make_vka(&vka, allocman);

    /* create a vspace (virtual memory management interface). Bootinfo is
       required so that it will be masked out as already mapped in */
    error = sel4utils_bootstrap_vspace_with_bootinfo_leaky(&vspace,
                                                           &vspace_alloc_data, simple_get_pd(&simple),
                                                           &vka, binfo);
    if (error) {
        ZF_LOGF("Failed to bootstrap vspace");
    }

    /* fill the allocator with virtual memory */
    void *allocator_vpool_vaddr;
    reservation_t virtual_reservation = vspace_reserve_range(&vspace,
                                               ALLOCATOR_VIRTUAL_POOL_SIZE, seL4_AllRights, 1, &allocator_vpool_vaddr);
    if (virtual_reservation.res == 0) {
        ZF_LOGF("Failed to provide virtual memory for allocator");
    }

    bootstrap_configure_virtual_pool(allocman, allocator_vpool_vaddr,
                                     ALLOCATOR_VIRTUAL_POOL_SIZE, simple_get_pd(&simple));

    // sel4platsupport_new_io_ops
    platsupport_serial_setup_simple(&vspace, &simple, &vka);

    printf("Serial device is now set up.\n");
    printf("OK byeee!\n");
    // 'Quit' by suspending the root thread
    seL4_TCB_Suspend(seL4_CapInitThreadTCB);
}