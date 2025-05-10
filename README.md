linux course, Амир Биктагиров, tg @papaya991

Версия ядра - 6.14.6
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.14.6.tar.xz

busybox - 1.37.0 c выключенным tc
wget https://busybox.net/downloads/busybox-1.37.0.tar.bz2

структупа проекта:
 /boot     config System.map vmlinuz
 /root
    /bin    установленный busybox
    /dev
    /lib
        /modules установленные модули
    /proc
    /root
    /sys
    /tmp
    init

Запуск с помощью скрипта start.sh, запускать из корня проекта
Я на виртуалке, так что для себя какие то флаги убрал, чтобы запустить с ними выполните VIRTUALIZATION_FLAGS=true ./start.sh


