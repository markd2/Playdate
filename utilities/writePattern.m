#import <Foundation/Foundation.h>

// clang -g -Wall -framework Foundation -fobjc-arc -o writePattern writePattern.m

int main() {
    // Basket weave
    unsigned char buffer[] = {
        // pattern
        0b11111000,
        0b01110100,
        0b00100010,
        0b01000111,
        0b10001111,
        0b00010111,
        0b00100010,
        0b01110001,

        // mask
        0b00000111,
        0b10001011,
        0b11011101,
        0b10111000,
        0b01110000,
        0b11101000,
        0b11011101,
        0b10001110,
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
