#include "klava.h"

#include <asm/io.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIR");
MODULE_DESCRIPTION("keyboard taps counter");
MODULE_VERSION("0.01");

#define IRQ_LINE 1
#define PS2_KEYBOARD_DATA_PORT 0x60
#define TIMER_COOLDOWN secs_to_jiffies(60)
static atomic_t taps_amount = ATOMIC_INIT(0);
static struct timer_list info_timer;

irqreturn_t handler(int irq, void* dev) {
  unsigned char scan_code = inb(PS2_KEYBOARD_DATA_PORT);

  if (!(scan_code & 0x80)) {
    atomic_inc(&taps_amount);
  }

  return IRQ_HANDLED;
}

static void info_callback(struct timer_list* obj) {
  int amount = atomic_xchg(&taps_amount, 0);
  mod_timer(&info_timer, jiffies + TIMER_COOLDOWN);
  pr_info("YOU PRESSED %d buttons last minute\n", amount);
}

static int __init klava_init(void) {
  pr_info("starting klava counter\n");

  int res = request_irq(IRQ_LINE, handler, IRQF_SHARED, "taps counter", handler);
  if (res < 0) {
    pr_err("failed to request irq\n");
    return res;
  }

  timer_setup(&info_timer, info_callback, 0);

  mod_timer(&info_timer, jiffies + TIMER_COOLDOWN);

  pr_info("started successfully\n");

  return 0;
}

static void __exit klava_exit(void) {
  free_irq(IRQ_LINE, handler);
  del_timer_sync(&info_timer);
}

module_init(klava_init);
module_exit(klava_exit);