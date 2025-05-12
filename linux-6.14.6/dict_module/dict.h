#pragma once

#include <linux/hashtable.h>
#include <linux/jhash.h>

#define MAX_USERS_LOG 10 // max users is 2**MAX_USERS_LOG
#define COMMAND_BUF_SIZE 10
#define NAME_BUF_SIZE 30
#define SURNAME_BUF_SIZE 40
#define PHONE_BUF_SIZE 20
#define MAIL_BUF_SIZE 40
#define INPUT_BUF_SIZE (COMMAND_BUF_SIZE + NAME_BUF_SIZE + SURNAME_BUF_SIZE + PHONE_BUF_SIZE + MAIL_BUF_SIZE + 10)
#define ANSWER_BUF_SIZE (NAME_BUF_SIZE+SURNAME_BUF_SIZE+10+PHONE_BUF_SIZE+MAIL_BUF_SIZE+8)

extern DECLARE_HASHTABLE(users_map, MAX_USERS_LOG);

struct user_info {
    char* name;
    char* surname;
    int age;
    char* phone;
    char* mail;
};

struct user_node {
    struct user_info user;
    struct hlist_node node;
};

u32 calc_hash(const char* key);

void free_user_info(struct user_info* user);

int free_user_info_if_bad_field(struct user_info* user);

struct user_info* copy_user_info(struct user_info* dst, const struct user_info* src);

int add_user_impl(const struct user_info* user);

int del_user_impl(const char* surname);

int get_user_impl(const char* surname, struct user_info* user);