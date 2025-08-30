#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/input.h>

#define DRV_NAME "joystick-gpio"

#define UP_GPIO 4
#define DOWN_GPIO 18
#define LEFT_GPIO 24
#define RIGHT_GPIO 17

struct joy_gpio {
    struct input_dev *input;
    struct gpio_desc *up_desc;
    struct gpio_desc *down_desc;
    struct gpio_desc *left_desc;
    struct gpio_desc *right_desc;
    int up_irq;
    int down_irq;
    int left_irq;
    int right_irq;

    /* TODO: add fields for GPIO descriptors, IRQs, state, etc. */
};

static struct joy_gpio *joy;

static irqreturn_t joy_irq_handler(int irq, void *data)
{
  int gpio;
  unsigned int keycode;

  if (irq == joy->up_irq) {
    gpio = gpio_get_value(UP_GPIO);
    keycode = KEY_UP;
  } else if (irq == joy->down_irq) {
    gpio = gpio_get_value(DOWN_GPIO);
    keycode = KEY_DOWN;
  } else if (irq == joy->left_irq) {
    gpio = gpio_get_value(LEFT_GPIO);
    keycode = KEY_LEFT;
  } else if (irq == joy->right_irq) {
    gpio = gpio_get_value(RIGHT_GPIO);
    keycode = KEY_RIGHT;
  } else {
    printk(KERN_ALERT "%s: Unknown IRQ line, exiting...\n", DRV_NAME);
    return IRQ_HANDLED; //TODO lookup irq error return values
  }

  input_report_key(joy->input, keycode, gpio);
  printk(KERN_ALERT "%s: Interrupt handled\n", DRV_NAME);
  printk(KERN_ALERT "%s: GPIO reads at (%d)\n", DRV_NAME, gpio);

  input_sync(joy->input);

  return IRQ_HANDLED;
}

static int __init joy_init(void)
{

  int ret;

  printk(KERN_ALERT "%s: Hello world\n", DRV_NAME);

  joy = kzalloc(sizeof(*joy), GFP_KERNEL);

  joy->input = input_allocate_device();
  if (!joy->input) printk(KERN_ALERT "%s: Failed to allocate device\n", DRV_NAME);
  joy->input->name = "Joystick/Button";
  set_bit(EV_KEY, joy->input->evbit);
  set_bit(KEY_UP, joy->input->keybit);
  set_bit(KEY_DOWN, joy->input->keybit);
  set_bit(KEY_LEFT, joy->input->keybit);
  set_bit(KEY_RIGHT, joy->input->keybit);
  if (input_register_device(joy->input)) printk(KERN_ALERT "%s: Failed to register input device %s\n", DRV_NAME, joy->input->name);
  /* TODO: set input capabilities (input_set_capability) */

  // IRQ for UP Direction
  joy->up_desc = gpio_to_desc(UP_GPIO);
  joy->up_irq = gpiod_to_irq(joy->up_desc);
  ret = request_irq(joy->up_irq, joy_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "joystick_up", joy); //TODO: I might wanna make this TRIGGER_HIGH or TRIGGER_RISING, see how it interfaces with retropi

  // IRQ for DOWN Direction
  joy->down_desc = gpio_to_desc(DOWN_GPIO);
  joy->down_irq = gpiod_to_irq(joy->down_desc);
  ret = request_irq(joy->down_irq, joy_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "joystick_down", joy); //TODO: I might wanna make this TRIGGER_HIGH or TRIGGER_RISING, see how it interfaces with retropi

  // IRQ for LEFT Direction
  joy->left_desc = gpio_to_desc(LEFT_GPIO);
  joy->left_irq = gpiod_to_irq(joy->left_desc);
  ret = request_irq(joy->left_irq, joy_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "joystick_left", joy); //TODO: I might wanna make this TRIGGER_HIGH or TRIGGER_RISING, see how it interfaces with retropi

  // IRQ for RIGHT Direction
  joy->right_desc = gpio_to_desc(RIGHT_GPIO);
  joy->right_irq = gpiod_to_irq(joy->right_desc);
  ret = request_irq(joy->right_irq, joy_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "joystick_right", joy); //TODO: I might wanna make this TRIGGER_HIGH or TRIGGER_RISING, see how it interfaces with retropi

  //if(gpiod_set_debounce(joy->up_desc, 1000000)) printk(KERN_ALERT "%s: Failed to set debounce\n", DRV_NAME);


  return 0;
}

static void __exit joy_exit(void)
{
  printk(KERN_ALERT "%s: Goodbye world\n", DRV_NAME);

  free_irq(joy->up_irq, joy);
  free_irq(joy->down_irq, joy);
  free_irq(joy->left_irq, joy);
  free_irq(joy->right_irq, joy);

  input_unregister_device(joy->input);

  kfree(joy);
}

module_init(joy_init);
module_exit(joy_exit);

MODULE_AUTHOR("Aidan Funk");
MODULE_DESCRIPTION("GPIO Joystick Driver");
MODULE_LICENSE("GPL");

