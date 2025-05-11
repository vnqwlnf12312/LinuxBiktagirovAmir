#pragma once

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sprintf.h>
#include <linux/string.h>
#include <linux/interrupt.h>

irqreturn_t handler(int irq, void* dev);