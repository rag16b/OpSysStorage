#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

MODULE_LICENSE("GPL");

// System Call STUB
long (*STUB_stop_elevator)(void) = NULL;
EXPORT_SYMBOL(STUB_stop_elevator);

// System Call Wrapper
SYSCALL_DEFINE0(stop_elevator) {
    printk(KERN_NOTICE "Inside SYSCALL_DEFINE0 block. %s\n", __FUNCTION__);
    if (STUB_stop_elevator != NULL)
        return STUB_stop_elevator();
    else
        return -ENOSYS;
}
