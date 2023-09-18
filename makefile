# ----------------------------
# Makefile Options
# ----------------------------

NAME = CE2048
ICON = icon.png
DESCRIPTION = "The game 2048"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
