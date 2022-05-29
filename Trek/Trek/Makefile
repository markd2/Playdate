HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = Trek.pdx

# Locate the SDK
SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)

######
# IMPORTANT: You must add your source folders to VPATH for make to find them
# ex: VPATH += src1:src2
######

VPATH += src:../../common-source

# List C source files here

LIB_SRC = ../../common-source/buttonpumper.c ../../common-source/memory.c ../../common-source/globals.c ../../common-source/patterns.c ../../common-source/geometry.c ../../common-source/timer.c ../../common-source/drawhelpers.c

SRC = src/main.c src/trek.c src/panel.c src/galaxyOverviewPanel.c src/menuPanel.c $(LIB_SRC)

# List all user directories here
UINCDIR = ../../common-source

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

# Alas, the common.mk clobbers this. Bug filed.
# If the "unrecognized file types" error annoys you, then
# edit common.mk in the SDK.
PDCFLAGS = --skip-unknown

include $(SDK)/C_API/buildsupport/common.mk

