#include "dict.h"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sprintf.h>
#include <linux/string.h>

DEFINE_HASHTABLE(users_map, MAX_USERS_LOG);

void free_user_info(struct user_info* user) {
    kfree(user->name);
    kfree(user->surname);
    kfree(user->phone);
    kfree(user->mail);
}

int free_user_info_if_bad_field(struct user_info* user) {
    if (!user->name || !user->surname || !user->phone || !user->mail) {
        free_user_info(user);
        return 1;
    }

    return 0;
}

u32 calc_hash(const char* key) {
    return jhash(key, strlen(key), 0);
}

struct user_info* copy_user_info(struct user_info* dst, const struct user_info* src) {
    strcpy(dst->name, src->name);
    strcpy(dst->surname, src->surname);
    dst->age = src->age;
    strcpy(dst->phone, src->phone);
    strcpy(dst->mail, src->mail);

    return dst;
}

int add_user_impl(const struct user_info* user) {
    struct user_node* node = kmalloc(sizeof(struct user_node), GFP_KERNEL);
    if (!node) {
        pr_err("failed to allocate memory for user_info\n");
        return -ENOMEM;
    }

    struct user_info* node_user = &node->user;
    node_user->name = kmalloc(strlen(user->name), GFP_KERNEL);
    node_user->surname = kmalloc(strlen(user->surname), GFP_KERNEL);
    node_user->phone = kmalloc(strlen(user->phone), GFP_KERNEL);
    node_user->mail = kmalloc(strlen(user->mail), GFP_KERNEL);
    
    int res = free_user_info_if_bad_field(node_user);
    if (res == 1) {
        return -ENOMEM;
    }

    copy_user_info(node_user, user);

    hash_add(users_map, &node->node, calc_hash(user->surname));

    return 0;
}

int del_user_impl(const char* surname) {
    int deleted_count = 0;
    struct user_node* found;
    u32 key = calc_hash(surname);
    hash_for_each_possible(users_map, found, node, key) {
        if (strcmp(found->user.surname, surname) == 0) {
            hash_del(&found->node);
            free_user_info(&found->user);
            kfree(found);
            pr_info("deleted user\n");
            ++deleted_count;
        }
    }

    if (deleted_count == 0) {
        pr_err("no such user\n");
        return -1;
    }

    return 0;
}

int get_user_impl(const char* surname, struct user_info* user) {
    int found_count = 0;
    struct user_node* found;
    u32 key = calc_hash(surname);
    hash_for_each_possible(users_map, found, node, key) {
        if (strcmp(found->user.surname, surname) == 0) {
            ++found_count;
            copy_user_info(user, &found->user);
        }
    }

    if (found_count == 0) {
        pr_err("no such user\n");
        return -1;
    }

    return 0;
}