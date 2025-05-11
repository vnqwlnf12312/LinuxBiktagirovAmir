#include "dict.h"

#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(add_user, const struct user_info __user*, user) {
    return add_user_impl(user);
}

SYSCALL_DEFINE1(del_user, const char __user*, surname) {
    return del_user_impl(surname);
}

SYSCALL_DEFINE2(get_user, const char __user*, surname, struct user_info __user*, user) {
    return get_user_impl(surname, user);
}