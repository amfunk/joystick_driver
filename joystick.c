#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/input.h>

#define DRV_NAME "joystick-gpio"

#define UP_GPIO 15
#define DOWN_GPIO 15
#define LEFT_GPIO 15
#define RIGHT_GPIO 15

/* Step 1: Define your button/joystick structure */
struct joy_gpio {
    struct input_dev *input;
    struct gpio_desc *desc;
    int irq;

    /* TODO: add fields for GPIO descriptors, IRQs, state, etc. */
};

struct joy_gpio *joy;

/* Step 2: IRQ handler for button presses */
static irqreturn_t joy_irq_handler(int irq, void *data)
{
  /* TODO: read GPIO value
   * TODO: map it to an input event (BTN_DPAD_UP, etc.)
   * TODO: call input_report_key() and input_sync()
   */
  return IRQ_HANDLED;
}

/* Step 3: Probe function — runs when the device is bound */
static int __init joy_init(void)
{

  printk(KERN_ALERT "Hello world: %s\n", DRV_NAME);

  /* TODO: allocate memory (devm_kzalloc) */
  joy = kzalloc(sizeof(*joy), GFP_KERNEL);

  /* TODO: request input device (devm_input_allocate_device) */
  /* TODO: set device name/id */
  /* TODO: set input capabilities (input_set_capability) */
  /* TODO: request GPIOs from DT (devm_gpiod_get) */
  joy->desc = gpio_to_desc(UP_GPIO);
  /* TODO: map GPIOs to IRQs (gpiod_to_irq) */
  joy->irq = gpiod_to_irq(joy->desc);
  /* TODO: request IRQs (devm_request_threaded_irq) */
  int ret = request_irq(joy->irq, joy_irq_handler, 0, "joy", joy);

  /* TODO: register the input device (input_register_device) */

  return 0;
}

static void __exit joy_exit(void)
{
  printk(KERN_ALERT "Goodbye world\n");
  kfree(joy);
}

module_init(joy_init);
module_exit(joy_exit);

MODULE_AUTHOR("Aidan Funk");
MODULE_DESCRIPTION("GPIO Joystick Driver");
MODULE_LICENSE("GPL");

