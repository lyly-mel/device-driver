/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Lylia Melahi
* Student ID:: 923677843
* GitHub-Name:: lyly-mel
* Project:: Assignment 6 - Device Driver
*
* File:: simpleDriver.c
*
* Description:: this file is a simple encryption device 
* driver. it encrypt or decrypt the user input. Then read 
* back the result to the user interface.
**************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>

#include <linux/sched.h>
#define MY_MAJOR 415
#define MY_MINOR 0
#define DEVICE_NAME "simpleDriver"

int major, minor;
char *kernel_buffer;

struct cdev my_cdev;
int actual_rx_size = 0;

MODULE_AUTHOR("Melahi Lylia");
MODULE_DESCRIPTION("A simple encryption driver");
MODULE_LICENSE("GPL");

#define ENCRYPT_MODE 0 //if mode set to 0: encrypt the message
#define DECRYPT_MODE 1 //if mode set to 1: decrypt the message

#define IOCTL_SET_KEY 4 //command 4 to set the key to the user key
#define IOCTL_SET_MOD 3 //command 3 to set the mode to the user mode
#define DEFAULT_KEY 0 //0 is the default key value if no key is provided

struct encypt_data {
    int key;
    int mode;
    char *message;
} encypt_data;

//function to encrypte message by shifting it according to the value of key
void encrypt_message(char *message, int key) {
    int i = 0;
    char ch = '\0';
    //itterat through the message char by char
    while((ch = message[i])!= '\0'){
        if (ch >= 'A' && ch <= 'Z') { //encrypt uppercase letters
            ch = (ch + key - 'A') % 26 + 'A';
        } else if (ch >= 'a' && ch <= 'z') { //encrypt lower case letters
            ch = (ch + key - 'a') % 26 + 'a';
        }
        message[i] = ch;
        i++;
    }
}

//function to decrypte message by shifting it back according to the value of key
void decrypt_message(char *message, int key) {  
    int i = 0;
    char ch = '\0';
    //itterat through the message char by char
    while((ch = message[i])!= '\0'){
        if (ch >= 'A' && ch <= 'Z') {
            ch = (ch - key - 'A' + 26) % 26 + 'A';//decrypt uppercase letters
        } else if (ch >= 'a' && ch <= 'z') {
            ch = (ch - key - 'a' + 26) % 26 + 'a';//decrypt lowercase letters
        }
        message[i] = ch;
        i++;
    }
}
//reads the user's message, and write it to the message varibale
//returns how many bytes were passed in
static ssize_t myWrite(struct file *fs, const char __user *buf, size_t hsize, loff_t *off) {
    struct encypt_data *data;
    data = (struct encypt_data *) fs->private_data;

    if (data->message != NULL) {
        vfree(data->message); // Free previously allocated memory
        data->message = NULL;
    }

    data->message = vmalloc(hsize + 1); // Allocate memory for message buffer
    if (data->message == NULL){
        printk(KERN_ERR "Can not vmalloc.\n");// Memory allocation failed
        return -1; 
    }

    //copy from the user's buffer to the message variabel
    if (copy_from_user(data->message, buf, hsize) > 0) {
        vfree(data->message);
        // Error copying data from user space
        printk(KERN_ERR "ERROR copying user message.\n");
        return -1;
    }

    data->message[hsize] = '\0'; // Null-terminate the string

    if (data->mode == ENCRYPT_MODE) { //encrypt the message 
        encrypt_message(data->message, data->key);
    } else if (data->mode == DECRYPT_MODE) { //decrypt the message
        decrypt_message(data->message, data->key);
    } else{
        printk(KERN_ERR "ERROR SPECIFYING THE MODE. \n");
    }
    //return the bytes copyed from the user 
    return hsize;
}

static ssize_t myRead(struct file *fs, char __user *buf, size_t hsize, loff_t *off) {
    struct encypt_data *data;
    data = (struct encypt_data *) fs->private_data;
    size_t bytes_to_read = 0;

    if (data->message == NULL){
        printk(KERN_INFO "No message to read.\n");
        return 0; // No message to read
    }
    // Determine the number of bytes to copy
    bytes_to_read = strlen(data->message);

    //if the size of the message is bigger than user's buffer, 
    //the size to read is the size of the user's buffer
    if(bytes_to_read > hsize){
        bytes_to_read = hsize;
    }

    // Copy data from kernel space to user space
    if (copy_to_user(buf, data->message, bytes_to_read)) {
        printk(KERN_ERR "ERROR READING THE RESULT. \n");
        return -1; // Error copying data to user space
    }

    printk(KERN_INFO "THE RESULT IS: %s\n", data->message);
    printk(KERN_INFO "We read : %lu \n", bytes_to_read);

    //return the bytes read
    return bytes_to_read;

}

static int myOpen(struct inode *inode, struct file *fs) {
    struct encypt_data *data;
    data = vmalloc(sizeof(struct encypt_data));

    if (data == 0) {
        printk(KERN_ERR "Can not vmalloc. File not opened.\n");
        return -1;
    }
    //initialize the encrypt_data structure
    data->key = DEFAULT_KEY; //set the key to the default value
    data->mode = ENCRYPT_MODE;//the default value for mode is encryption
    data->message = NULL;

    fs->private_data = data;

    return 0;
}

static int myClose(struct inode *inode, struct file *fs) {
    struct encypt_data *data;
    data = (struct encypt_data*) fs->private_data;
    if(data->message != NULL){
        vfree(data->message);
    }
    vfree(data);

    printk(KERN_INFO "closed\n");

    return 0;
}

static long myIoCtl(struct file *fs, unsigned int command, unsigned long info) {
    struct encypt_data *data;
    data = (struct encypt_data*) fs->private_data;

    switch (command) {
        case IOCTL_SET_KEY: //the command is to set the key
            printk(KERN_INFO "set key.\n");
            if(copy_from_user(&(data->key), (int *)info, sizeof(int)) > 0){
                printk(KERN_ERR "ERROR GETTING THE KEY\n");
                printk(KERN_INFO "USE DEFAULT KEY VALUE 0\n");
            }
            break;
        case IOCTL_SET_MOD: //the command is to set the mode 
            printk(KERN_INFO "set mode.\n");
            if(copy_from_user(&(data->mode), (int *)info, sizeof(int)) > 0){
                printk(KERN_ERR "ERROR GETTING THE MODE\n");
                printk(KERN_INFO "USE DEFAULT mode VALUE: ENCRYPTED_MODE\n");
            }
            break;
        default:
            printk(KERN_ERR "failed in myioctl.\n");
            return -1;
    }
    printk(KERN_INFO "succeed in myioctl.\n");
    return 0;
}

// another data structure
struct file_operations fops = {
        .open = myOpen,
        .release = myClose,
        .write = myWrite,
        .read = myRead,
        .unlocked_ioctl = myIoCtl,
        .owner = THIS_MODULE,

};

// creates a device node in .dev, returns error if not made
int init_module(void) {
    int result, registers;
    dev_t devno;

    devno = MKDEV(MY_MAJOR, MY_MINOR);

    registers = register_chrdev_region(devno, 1, DEVICE_NAME);
    printk(KERN_INFO "Register chardev succeeded 1: %d\n", registers);
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, devno, 1);
    printk(KERN_INFO "Dev Add chardev suceeded 2: %d\n", result);
    printk(KERN_INFO "Welcome - Simple Driver is loaded.\n");
    // result = register_chrdev(13, "harry", &fops);
    if (result < 0) {
        printk(KERN_ERR "Register chardev failed: %d\n", result);
    }

    return result;
}

// unregistering and removing device from kernel
void cleanup_module(void) {
    dev_t devno;
    devno = MKDEV(MY_MAJOR, MY_MINOR);
    unregister_chrdev_region(devno, 1);
    cdev_del(&my_cdev);

    printk(KERN_INFO "Goodbye from Simple Driver!\n");
}