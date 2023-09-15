# ----------------------------
# Makefile Options
# ----------------------------

NAME = 2048CE
ICON = icon.png
DESCRIPTION = "The game 2048"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz -Wimplicit-function-declaration
CXXFLAGS = -Wall -Wextra -Oz -Wimplicit-function-declaration

# ----------------------------

include $(shell cedev-config --makefile)
