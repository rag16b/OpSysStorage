#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

MODULE_LICENSE("GPL");

// System Call STUB
long (*STUB_issue_request)(int, int, int) = NULL;
EXPORT_SYMBOL(STUB_issue_request);

// System Call Wrapper
SYSCALL_DEFINE3(issue_request, int, passenger_type, int, start_floor, int, destination_floor) {
    printk(KERN_NOTICE "Inside SYSCALL_DEFINE3 block. %s You have the following ints: %d, %d, %d\n",
	 __FUNCTION__, passenger_type, start_floor, destination_floor);
    if (STUB_issue_request != NULL)
        return STUB_issue_request(passenger_type, start_floor, destination_floor);
    else
        return -ENOSYS;
}
