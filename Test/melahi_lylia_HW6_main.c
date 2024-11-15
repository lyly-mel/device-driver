/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Lylia Melahi
* Student ID:: 923677843
* GitHub-Name:: lyly-mel
* Project:: Assignment 6 - Device Driver
*
* File:: melahi_lylia_HW6_main.c
*
* Description:: a user in terface for the device drive 
* encryption. It promps the user for a text input, a key and 
* encryption or decryption mode.Then it will print the result
**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEVICE_FILE "/dev/simpleDriver"
#define IOCTL_SET_KEY 4
#define IOCTL_SET_MOD 3
#define MAX_TEXT_SIZE 512 //max size for the user text

void get_user_key(int *key){
    printf("Enter an integer key.\n");
    int scan_key = scanf("%d", key);
    //keep looking while the input is not valid
    while (scan_key != 1) {
        // Input was not a valid integer
        printf("Invalid input. Please enter an integer.\n");
        while (getchar() != '\n'); // Clear the input buffer
        scan_key = scanf("%d", key); // prompt the user to enter the key again
    } 
}

void get_user_mode(int *mode){
    printf("Enter the mode (0 for encrypt, 1 for decrypt).\n");
    int scan_mode = scanf("%d", mode);
    //keep looking while the input is not valid
    while (*mode != 0 && *mode != 1) {
        // Input was not a valid integer
        printf("Invalid input. Please enter 0 or 1.\n");
        while (getchar() != '\n'); // Clear the input buffer
        scan_mode = scanf("%d", mode); // prompt the user to enter the key again
    } 
}

void set_key(int fd, int key) {
    //call ioctl in the device driver to set the key 
    ioctl(fd, IOCTL_SET_KEY, &key);
}

void set_mode(int fd, int mode) {
    //call ioctl in the device driver to set the mode
    ioctl(fd, IOCTL_SET_MOD, &mode);
}

int main (int argc, char * argv[])
{
    int fd, key, mode;
    char message[MAX_TEXT_SIZE];

    //open the file and return its file descriptor
    fd = open(DEVICE_FILE, O_RDWR);
    printf("Returned from open file, %d\n", fd);
    if(fd < 0) {
        printf("device open error\n");
        perror("device file open error\n");
        return -1;
    }
    else {
        printf("device open success.\n");
    }

    // get message from the user
    printf("Enter the message to be encrypted/decrypted.\n");
    fgets(message, sizeof(message), stdin);

    // get key from the user   
    get_user_key(&key);
    //set the key in the device driver using ioctl
    set_key(fd, key); 
    printf("the key entered %d\n", key);

    // get mode from the user
    get_user_mode(&mode);
    //set the mode in the device driver using ioctl
    set_mode(fd, mode);
    printf("the mode entered %d\n", mode);

    // Write message to the device file
    write(fd, message, strlen(message));

    // Read the result from the device file
    char result[MAX_TEXT_SIZE];
    read(fd, result, sizeof(result));
    printf("The result: %s\n", result);

    // Close the device file
    close(fd);

    return 0;
}

