HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = Sampler.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

######
# IMPORTANT: You must add your source folders to VPATH for make to find them
# ex: VPATH += src1:src2
######

VPATH += src:../common-source

# List C source files here

LIB_SRC = ../common-source/buttonpumper.c ../common-source/memory.c ../common-source/globals.c ../common-source/patterns.c ../common-source/geometry.c ../common-source/timer.c

SRC = src/main.c $(LIB_SRC) src/demoSample.c src/drawingDemo.c src/bitmapDemo.c src/fontDemo.c src/tableDemo.c src/synthDemo.c src/moreBitmapDemo.c

# List all user directories here
UINCDIR = ../common-source

# List user asm files
UASRC = 

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

