#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct user_info {
    char* name;
    char* surname;
    int age;
    char* phone;
    char* mail;
};

int main() {
    struct user_info user;
    user.name = "Amir";
    user.surname = "BIKTAGIROV";
    user.age = 21;
    user.phone = "88005553535";
    user.mail = "lol@mail.ru";

    int res = syscall(468, &user);
    printf("add res: %d\n", res);

    struct user_info got_user;
    got_user.name = malloc(100);
    got_user.surname = malloc(100);
    got_user.phone = malloc(100);
    got_user.mail = malloc(100);
    res = syscall(467, user.surname, &got_user);
    printf("get res: %d\n", res);

    printf("GOT %s %s %d %s %s\n", got_user.name, got_user.surname, got_user.age, got_user.phone, got_user.mail);

    res = syscall(469, user.surname);
    printf("del res: %d\n", res);

    free(got_user.name);
    free(got_user.surname);
    free(got_user.phone);
    free(got_user.mail);
}