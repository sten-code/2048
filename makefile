# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz -Wimplicit-function-declaration
CXXFLAGS = -Wall -Wextra -Oz -Wimplicit-function-declaration

# ----------------------------

include $(shell cedev-config --makefile)
