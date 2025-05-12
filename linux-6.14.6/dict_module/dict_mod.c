#include "dict_mod.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sprintf.h>
#include <linux/string.h>

#include "dict.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIR");
MODULE_DESCRIPTION("dictionary containing user data");
MODULE_VERSION("0.01");

static int major;
static struct class* dict_class = NULL;
static struct cdev dict_data;
static struct device* dict_device = NULL;
static char answer[ANSWER_BUF_SIZE] = {'\0'};

static ssize_t dict_read(struct file* f, char __user* u, size_t l, loff_t* o) {
    size_t answer_len = strlen(answer);
    if (*o > answer_len) {
        return 0;
    }
    size_t read_len = min(l, answer_len - *o);
    if (copy_to_user(u, answer + *o, read_len)) {
        pr_err("failed to answer\n");
        return -EFAULT;
    }
    *o += read_len;
    return read_len;
}

int do_add(const char* args) {
    pr_info("doing add\n");

    char name[NAME_BUF_SIZE];
    char surname[SURNAME_BUF_SIZE];
    int age;
    char phone[PHONE_BUF_SIZE];
    char mail[MAIL_BUF_SIZE];

    int res = sscanf(args, "%s %s %d %s %s", name, surname, &age, phone, mail);
    if (res < 0) {
        pr_err("failed to read input\n");
        return res;
    }
    if (res != 5) {
        pr_err("failed to match input, expected 5 arguments: name, surname, age, mail, phone. got %d \n", res);
        return -1;
    }

    struct user_node* found;
    u32 key = calc_hash(surname);
    hash_for_each_possible(users_map, found, node, key) {
        if (strcmp(found->user.surname, surname) == 0) {
            pr_err("user already exists\n");
            return -1;
        }
    }

    struct user_info user;
    user.name = name;
    user.surname = surname;
    user.age = age;
    user.phone = phone;
    user.mail = mail;

    res = add_user_impl(&user);
    if (res < 0) {
        pr_err("failed to insert user\n");
        return -1;
    }

    pr_info("added successfully\n");

    return 0;
}

int do_del(const char* args) {
    pr_info("doing del\n");

    char surname[SURNAME_BUF_SIZE];
    int res = sscanf(args, "%s", surname);
    if (res < 0) {
        pr_err("failed to read input\n");
        return res;
    }
    if (res != 1) {
        pr_err("failed to match input, expected surname\n");
        return -1;
    }

    res = del_user_impl(surname);
    if (res < 0) {
        pr_info("failed to delete user");
        return res;
    }

    pr_info("successfully deleted\n");
    return 0;
}

int do_get(const char* args) {
    pr_info("doing get\n");

    char surname[SURNAME_BUF_SIZE];
    int res = sscanf(args, "%s", surname);
    if (res < 0) {
        pr_err("failed to read input\n");
        return res;
    }
    if (res != 1) {
        pr_err("failed to match input, expected surname\n");
        return -1;
    }

    struct user_info user;
    user.name = kmalloc(NAME_BUF_SIZE, GFP_KERNEL);
    user.surname = kmalloc(SURNAME_BUF_SIZE, GFP_KERNEL);
    user.phone = kmalloc(PHONE_BUF_SIZE, GFP_KERNEL);
    user.mail = kmalloc(MAIL_BUF_SIZE, GFP_KERNEL);
    res = free_user_info_if_bad_field(&user);
    if (res == 1) {
        return -ENOMEM;
    } 

    res = get_user_impl(surname, &user);
    if (res < 0) {
        free_user_info(&user);
        pr_err("failed to get user\n");
        return res;
    }

    sprintf(answer, "%s %s %d %s %s", user.name, user.surname, user.age, user.phone, user.mail);
    free_user_info(&user);

    pr_info("successfully got user\n");
    return 0;
}

static ssize_t dict_write(struct file* f, const char __user* u, size_t count, loff_t* offset) {
    pr_info("starting to write\n");

    char buf [INPUT_BUF_SIZE];
    int res = copy_from_user(buf, u, count);
    if (res) {
        pr_err("failed to read input\n");
        return -EFAULT;
    }

    char command[COMMAND_BUF_SIZE];
    res = sscanf(buf, "%s", command);

    if (res < 0) {
        pr_err("failed to read input\n");
        return res;
    }
    if (res != 1) {
        pr_err("failed to match input, expected command\n");
        return -1;
    }

    const char* args = u + strlen(command) + 1;

    if (strcmp(command, "add") == 0) {
        res = do_add(args);
        if (res < 0) {
            return res;
        }
    } else if (strcmp(command, "del") == 0) {
        res = do_del(args);
        if (res < 0) {
            return res;
        }
    } else if (strcmp(command, "get") == 0) {
        res = do_get(args);
        if (res < 0) {
            return res;
        }
    } else {
        pr_err("unkown command \n");
        return -1;
    }

    *offset += count;
    return count;
}

static int dict_open(struct inode* i, struct file* f) {
    return 0;
}

static int dict_release(struct inode* i, struct file* f) {
    return 0;
}

static long dict_ioctl(struct file* f, unsigned int cmd, unsigned long arg) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dict_read,
    .write = dict_write,
    .open = dict_open,
    .release = dict_release,
    .unlocked_ioctl = dict_ioctl
};

static int __init dict_init(void) {
    pr_info("starting dict module\n");

    int err;
    dev_t dev;
    err = alloc_chrdev_region(&dev, 0, 1, "dict");
    if (err < 0) {
        pr_err("dict failed to allocate device\n");
        return err;
    }
    major = MAJOR(dev);

    dict_class = class_create("dict");
    if (IS_ERR(dict_class)) {
        unregister_chrdev_region(dev, 1);
        pr_err("dict failed to create class\n");
        return PTR_ERR(dict_class);
    }

    cdev_init(&dict_data, &fops);
    dict_data.owner = THIS_MODULE;

    err = cdev_add(&dict_data, dev, 1);
    if (err < 0) {
        unregister_chrdev_region(dev, 1);
        class_destroy(dict_class);
        pr_err("dict failed to add device\n");
        return err;
    }

    dict_device = device_create(dict_class, NULL, dev, NULL, "dict");
    if (IS_ERR(dict_device)) {
        unregister_chrdev_region(dev, 1);
        class_destroy(dict_class);
        pr_err("dict failed to create device\n");
        return PTR_ERR(dict_class);
    }

    hash_init(users_map);

    pr_info("successfully started dict module, registered device %d\n", major);
    return 0;
}

static void __exit dict_exit(void) {
    pr_info("exiting dict module\n");

    int dev = MKDEV(major, 0);
    device_destroy(dict_class, dev);
    class_destroy(dict_class);
    unregister_chrdev_region(dev, 1);

    pr_info("successfully exited dict module\n");
}

module_init(dict_init);
module_exit(dict_exit);