#import <Foundation/Foundation.h>

// clang -g -Wall -framework Foundation -fobjc-arc -o writePattern writePattern.m

int main() {
    // Basket weave
    unsigned char buffer[] = {
        0b11111000,
        0b01110100,
        0b00100010,
        0b01000111,
        0b10001111,
        0b00010111,
        0b00100010,
        0b01110001,

        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
    };

    int fd = open("basket-weave.pattern", O_CREAT | O_WRONLY, 0664);
    if (fd == -1) {
        printf("oops open %d %s\n", fd, strerror(errno));
    }

    int success = write(fd, buffer, sizeof(buffer));

    if (success == -1) {
        printf("oops write %d %s\n", success, strerror(errno));
    }

    printf("size? %lu\n", sizeof(buffer));
    close(fd);

    printf("wrote to basket-weave.pattern\n");

    return 0;
} // main
