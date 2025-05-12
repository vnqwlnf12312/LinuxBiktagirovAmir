#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sprintf.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIR");
MODULE_DESCRIPTION("provides vma info");
MODULE_VERSION("0.01");

#define VMA_NAME "mmaneg"
#define COMMAND_BUFFER_SIZE 30
#define ARGS_BUFFER_SIZE 100
#define BUFFER_SIZE (COMMAND_BUFFER_SIZE + ARGS_BUFFER_SIZE + 1)

static ssize_t vma_read(struct file* f, char __user* u, size_t l, loff_t* o) {
    return 0;
}

static int vma_open(struct inode* i, struct file* f) {
    return 0;
}

static int vma_release(struct inode* i, struct file* f) {
    return 0;
}

int do_list_vma(char* args) {
    struct mm_struct* mm = current->mm;

    down_read(&mm->mmap_lock);
    VMA_ITERATOR(it, mm, 0);
    struct vm_area_struct* vma;

    for_each_vma(it, vma) {
        pr_info("vma: 0x%lx - 0x%lx, size: %lu KB, flags: %lx\n",
               vma->vm_start, vma->vm_end,
               (vma->vm_end - vma->vm_start) >> 10,
               vma->vm_flags);
    }

    up_read(&mm->mmap_lock);

    return 0;
}

int do_findpage(char* args) {
    unsigned long addr;
    int res = sscanf(args, "%lx", &addr);
    if (res < 0) {
        pr_err("failed to read argument\n");
        return res;
    }
    if (res != 1) {
        pr_err("failed to match arguments, expected addr\n");
        return -1;
    }

    struct mm_struct* mm = current->mm;

    down_read(&mm->mmap_lock);

    struct vm_area_struct* vma = find_vma(mm, addr);
    if (!vma || (vma->vm_start > addr)) {
        pr_err("address not found\n");
        up_read(&mm->mmap_lock);
        return -1;
    }
    struct page* page = virt_to_page(addr);
    if (!page) {
        pr_err("invalid page\n");
        up_read(&mm->mmap_lock);
        return -1;
    }
    phys_addr_t phys_addr = page_to_phys(page);

    pr_info("virtual address: 0x%lx, Physical address: %pa\n", addr, &phys_addr);
    up_read(&mm->mmap_lock);

    return 0;
}

int do_writeval(char* args) {
    unsigned long addr;
    unsigned long val;
    int res = sscanf(args, "%lx %lu", &addr, &val);
    if (res < 0) {
        pr_err("failed to read argument\n");
        return res;
    }
    if (res != 2) {
        pr_err("failed to match arguments, expected addr adn value\n");
        return -1;
    }

    struct mm_struct* mm = current->mm;

    down_read(&mm->mmap_lock);

    struct vm_area_struct* vma = find_vma(mm, addr);
    if (!vma || (vma->vm_start > addr)) {
        pr_err("address not found\n");
        up_read(&mm->mmap_lock);
        return -1;
    }

    if (!(vma->vm_flags & VM_WRITE)) {
        pr_err("no write permissions at this address\n");
        up_read(&mm->mmap_lock);
        return -EPERM;
    }

    up_read(&mm->mmap_lock);

    if (access_ok((void*)addr, sizeof(unsigned long))) {
        res = put_user(val, (unsigned long*)addr);
        if (res < 0) {
            return res;
        }
    } else {
        down_write(&mm->mmap_lock);
        memcpy((void*)addr, &val, sizeof(unsigned long));
        up_write(&mm->mmap_lock);
    }

    return 0;
}

static ssize_t vma_write(struct file* f, const char __user* u, size_t count, loff_t* offset) {
    pr_info("starting to write\n");

    char buf [BUFFER_SIZE];
    int res = copy_from_user(buf, u, count);
    if (res) {
        pr_err("failed to read input\n");
        return -EFAULT;
    }

    char command [COMMAND_BUFFER_SIZE];
    res = sscanf(buf, "%s", command);
    if (res < 0) {
        pr_err("failed to read input\n");
        return res;
    }
    if (res != 1) {
        pr_err("failed to match input, expected command\n");
        return -1;
    }

    char* args = buf + strlen(command) + 1;

    if (strcmp(command, "listvma") == 0) {
        res = do_list_vma(args);
        if (res < 0) {
            return res;
        }
    } else if (strcmp(command, "findpage") == 0) {
        res = do_findpage(args);
        if (res < 0) {
            return res;
        }
    } else if (strcmp(command, "writeval") == 0) {
        res = do_writeval(args);
        if (res < 0) {
            return res;
        }
    } else {
        pr_err("unkown command\n");
        return -1;
    }

    pr_info("wrote successfully\n");
    *offset += count;
    return count;
}

static struct proc_ops pops = {
    .proc_read = vma_read,
    .proc_write = vma_write,
    .proc_open = vma_open,
    .proc_release = vma_release
};

static int __init vma_mod_init(void) {
    pr_info("starting vma module\n");

    struct proc_dir_entry* entry = proc_create(VMA_NAME, S_IRWXU, NULL, &pops);
    if (!entry) {
        return -ENOMEM;
    }

    pr_info("successfully started vma module\n");
    return 0;
}

static void __exit vma_mod_exit(void) {
    pr_info("exiting vma module\n");

    remove_proc_entry(VMA_NAME, NULL);

    pr_info("successfully exited vma module\n");
}

module_init(vma_mod_init);
module_exit(vma_mod_exit);