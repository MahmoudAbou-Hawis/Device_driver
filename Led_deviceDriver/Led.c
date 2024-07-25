#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define MAX_ARRAY_SIZE 15

dev_t device_number;
struct cdev dev;
struct class *myClass;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mahmoud Abou-Hawis");
MODULE_DESCRIPTION("A hello world Pseudo device driver");

static int GPIOS[MAX_ARRAY_SIZE];
static int array_param_size;
module_param_array(GPIOS, int, &array_param_size, 0644);
MODULE_PARM_DESC(GPIOS, "Array of GPIOs");
MODULE_PARM_DESC(array_param_size, "Size of the GPIO array");



static ssize_t led_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos) {
    char kernel_buffer[3];
    struct inode *inode = file->f_inode;
    dev_t dev = inode->i_rdev;
    unsigned int minor_num = MINOR(dev);
    int len = min(count, sizeof(kernel_buffer) - 1);

    if (copy_from_user(kernel_buffer, buffer, len))
        return -EFAULT;

    kernel_buffer[len] = '\0';
    
    printk("%d %d %c\n",minor_num , GPIOS[minor_num],kernel_buffer[0]);
    switch(kernel_buffer[0])
    {
        case '1':
            gpio_set_value(GPIOS[minor_num],1);
            break;
        case '0':
            gpio_set_value(GPIOS[minor_num],0);
            break;
        default:
            printk(KERN_WARNING "Invalid input\n");
            break;
    }

    return len;
}

static struct file_operations operations = {
    .owner = THIS_MODULE,
    .write = led_write
};

static int __init Leds_init(void) {
    int retval, i;

    retval = alloc_chrdev_region(&device_number, 0, array_param_size, "led");
    if (retval < 0) {
        printk(KERN_WARNING "led: could not register device number\n");
        return retval;
    }
    printk(KERN_INFO "led: Major = %d Minor = %d\n", MAJOR(device_number), MINOR(device_number));

    cdev_init(&dev, &operations);
    retval = cdev_add(&dev, device_number, array_param_size);
    if (retval < 0) {
        unregister_chrdev_region(device_number, array_param_size);
        printk(KERN_WARNING "led: failed to add the driver char\n");
        return retval;
    }

    myClass = class_create("LED");
    if (IS_ERR(myClass)) {
        cdev_del(&dev);
        unregister_chrdev_region(device_number, array_param_size);
        return PTR_ERR(myClass);
    }

    for (i = 0; i < array_param_size; i++) {
        if (IS_ERR(device_create(myClass, NULL, MKDEV(MAJOR(device_number), MINOR(device_number) + i), NULL, "LED%d", i))) {
            while (i-- > 0) {
                device_destroy(myClass, MKDEV(MAJOR(device_number), MINOR(device_number) + i));
            }
            class_destroy(myClass);
            cdev_del(&dev);
            unregister_chrdev_region(device_number, array_param_size);
            return -1;
        }
    }
    for (i = 0; i < array_param_size; i++) {
        printk("GPIO: %d \n" , GPIOS[i]);
        retval = gpio_request(GPIOS[i], "gpio");
        printk("GPIO: %d \n" , GPIOS[i]);

        if (retval) {
            printk(KERN_ERR "Failed to request GPIO %d\n", GPIOS[i]);
            while (i-- > 0) {
                gpio_free(GPIOS[i] );
            }
            return retval;
        }
        else
        {
            retval = gpio_direction_output(GPIOS[i], 0);
            if (retval) {
            printk(KERN_ERR "Failed to set direction for GPIO %d\n", GPIOS[i]);
            while (i-- > 0) {
                gpio_free(GPIOS[i]);
            }
            return retval;
        }
        }
    }
    printk(KERN_INFO "led: Module loaded\n");
    return 0;
}

static void __exit Leds_exit(void) {
    int i;

    for (i = 0; i < array_param_size; i++) 
    {
        gpio_set_value(GPIOS[i] - '0',0);
        gpio_free(GPIOS[i] - '0');
    }

    for (i = 0; i < array_param_size; i++) {
        device_destroy(myClass, MKDEV(MAJOR(device_number), MINOR(device_number) + i));
    }

    class_unregister(myClass);
    class_destroy(myClass);
    cdev_del(&dev);
    unregister_chrdev_region(device_number, array_param_size);

    printk(KERN_INFO "led: Module unloaded\n");
}

module_init(Leds_init);
module_exit(Leds_exit);
