## Linux Course

Автор: Амир Биктагиров  
Telegram: @papaya991

### Версии

- Версия ядра: 6.14.6  
  Скачать ядро

- BusyBox: 1.37.0 (с выключенным tc)  
  Скачать BusyBox

### Структура проекта
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
### Запуск

Для запуска проекта используйте скрипт start.sh. Запускать скрипт необходимо из корня проекта.

Пример запуска:
./start.sh
Для запуска с флагами виртуализации выполните:
VIRTUALIZATION_FLAGS=true ./start.sh