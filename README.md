## Linux Course

Автор: Амир Биктагиров  
Telegram: @papaya991

### Версии

- Версия ядра: 6.14.6  
  wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.14.6.tar.xz

- BusyBox: 1.37.0 (с выключенным tc)  
  wget https://busybox.net/downloads/busybox-1.37.0.tar.bz2

### Структура проекта
```bash
/boot
    config
    System.map
    vmlinuz
/root
    /bin    # установленный busybox
    /dev
    /lib
        /modules    # установленные модули
    /proc
    /root
    /sys
    /tmp
    init
```
### Запуск
Примените патч в директории содержащей linux-6.14.6

Для запуска проекта используйте скрипт start.sh. Cкрипт ждет что в той же директории есть папка boot с vmlinuz (собирать ядро рекомендуется с моим конфигом из корня проекта), а также папка root (такая же как в репозитории + bin с busybox, dev, lib с установленными модулями, proc, sys, tmp)

Пример запуска:
```bash
./start.sh
```
Для запуска с флагами виртуализации выполните:
```bash
VIRTUALIZATION_FLAGS=true ./start.sh
```
