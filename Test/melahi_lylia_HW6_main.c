/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Lylia Melahi
* Student ID:: 923677843
* GitHub-Name:: lyly-mel
* Project:: Assignment 6 - Device Driver
*
* File:: melahi_lylia_HW6_main.c
*
* Description:: 
**************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main (int argc, char * argv[])
{
    int fd, info;
    long n1, n2, n3;
    char Message[] = {"My message That Send"};
    char read_buf[512];

    fd = open("/dev/simpleDriver", O_WRONLY);
    printf("Returned from open file, %d\n", fd);
    if(fd < 0)
    {
        printf("device open error\n");
        perror("device file open error\n");
        return -1;
    }
    else
    {
        printf("device open success.\n");
    }

    for (long i=1; i<30; i++)
    {
        n1 = write(fd, "testing", i*2);
        n2 = ioctl(fd, 3, &info);
        printf("wrote to %d, that returned %ld, count %d - Ret Val From ioctl:%ld \n",fd, n1,info, n2);
    }
    close(fd);

    return 0;
}