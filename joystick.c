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
#define ABUT_GPIO 26
#define BBUT_GPIO 27

#define DEBOUNCE_MS 20

struct joy_gpio {
  struct input_dev *input;
  struct button_gpio *up;
  struct button_gpio *down;
  struct button_gpio *left;
  struct button_gpio *right;
  struct button_gpio *abut;
  struct button_gpio *bbut;
};

struct button_gpio {
  struct gpio_desc *desc;
  int irq;
  unsigned int keycode;
  struct delayed_work dwork;
};

static struct joy_gpio *joy;

static irqreturn_t joy_irq_handler(int irq, void *data)
{
  struct button_gpio *button = data;
  int gpio = gpiod_get_value(button->desc);
  
  //printk(KERN_ALERT "%s: Interrupt handled\n", DRV_NAME);

  input_report_key(joy->input, button->keycode, gpio);

  input_sync(joy->input);

  return IRQ_HANDLED;
}

static void joy_work_func(struct work_struct *work)
{

}

static int setup_irq_line(struct button_gpio *button, int GPIO, char *name, unsigned int keycode)
{
  int ret;

  button->keycode = keycode;

  button->desc = gpio_to_desc(GPIO);
  button->irq = gpiod_to_irq(button->desc);

  ret = request_irq(button->irq, joy_irq_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, name, button);

  return ret;
}

static int __init joy_init(void)
{
  int ret;

  printk(KERN_ALERT "%s: Hello world\n", DRV_NAME);

  joy = kzalloc(sizeof(*joy), GFP_KERNEL);
  joy->up = kzalloc(sizeof(*joy->up), GFP_KERNEL);
  joy->down = kzalloc(sizeof(*joy->down), GFP_KERNEL);
  joy->left = kzalloc(sizeof(*joy->left), GFP_KERNEL);
  joy->right = kzalloc(sizeof(*joy->right), GFP_KERNEL);
  joy->abut = kzalloc(sizeof(*joy->abut), GFP_KERNEL);
  joy->bbut = kzalloc(sizeof(*joy->bbut), GFP_KERNEL);

  joy->input = input_allocate_device();
  if (!joy->input) {
    printk(KERN_ALERT "%s: Failed to allocate device\n", DRV_NAME);
  }

  joy->input->name = "Joystick/Buttons";
  set_bit(EV_KEY, joy->input->evbit);
  set_bit(KEY_UP, joy->input->keybit);
  set_bit(KEY_DOWN, joy->input->keybit);
  set_bit(KEY_LEFT, joy->input->keybit);
  set_bit(KEY_RIGHT, joy->input->keybit);
  set_bit(KEY_A, joy->input->keybit);
  set_bit(KEY_B, joy->input->keybit);

  if (input_register_device(joy->input)) printk(KERN_ALERT "%s: Failed to register input device %s\n", DRV_NAME, joy->input->name);
  /* TODO: set input capabilities (input_set_capability) */

  // IRQ for UP Direction
  ret = setup_irq_line(joy->up, UP_GPIO, "joystick_up", KEY_UP);

  // IRQ for DOWN Direction
  ret = setup_irq_line(joy->down, DOWN_GPIO, "joystick_down", KEY_DOWN);

  // IRQ for LEFT Direction
  ret = setup_irq_line(joy->left, LEFT_GPIO, "joystick_left", KEY_LEFT);

  // IRQ for RIGHT Direction
  ret = setup_irq_line(joy->right, RIGHT_GPIO, "joystick_right", KEY_RIGHT);

  // IRQ for A Button
  ret = setup_irq_line(joy->abut, ABUT_GPIO, "button_a", KEY_A);

  // IRQ for B Button
  ret = setup_irq_line(joy->bbut, BBUT_GPIO, "button_b", KEY_B);

  return 0;
}

static void __exit joy_exit(void)
{
  printk(KERN_ALERT "%s: Goodbye world\n", DRV_NAME);

  free_irq(joy->up->irq, joy);
  free_irq(joy->down->irq, joy);
  free_irq(joy->left->irq, joy);
  free_irq(joy->right->irq, joy);
  free_irq(joy->abut->irq, joy);
  free_irq(joy->bbut->irq, joy);

  input_unregister_device(joy->input);

  kfree(joy->up);
  kfree(joy->down);
  kfree(joy->left);
  kfree(joy->right);
  kfree(joy->abut);
  kfree(joy->bbut);
  kfree(joy);
}

module_init(joy_init);
module_exit(joy_exit);

MODULE_AUTHOR("Aidan Funk");
MODULE_DESCRIPTION("GPIO Joystick Driver");
MODULE_LICENSE("GPL");

