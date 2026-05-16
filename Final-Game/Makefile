# ─────────────────────────────────────────────────────────────────────────────
#  Makefile – Tenacity
#  Layout:
#    main.c          – entry point
#    src/            – map, player, monster, renderer, hud, audio
#    include/        – types, map, player, monster, renderer, hud, audio headers
#    lib/            – raylib static/import libraries
#    imgs/           – texture assets   (runtime)
#    audio/          – sound/music assets (runtime)
#    output/         – compiled binary + object files
# ─────────────────────────────────────────────────────────────────────────────

# Toolchain 
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -Llib
LIBS    = -lraylib -lopengl32 -lgdi32 -lwinmm

# Paths 
SRC_DIR = src
OUT_DIR = output
TARGET  = $(OUT_DIR)/main.exe

# Sources
SRC_FILES = main.c \
            $(SRC_DIR)/map.c      \
            $(SRC_DIR)/player.c   \
            $(SRC_DIR)/monster.c  \
            $(SRC_DIR)/renderer.c \
            $(SRC_DIR)/hud.c      \
            $(SRC_DIR)/audio.c

OBJ_FILES = $(patsubst %.c, $(OUT_DIR)/%.o, $(notdir $(SRC_FILES)))

# Default Target
all: $(OUT_DIR) $(TARGET)

# Link 
$(TARGET): $(OBJ_FILES)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@
	@echo "Build complete: $@"

# Compile: root main.c 
$(OUT_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile: src/*.c 
$(OUT_DIR)/map.o: $(SRC_DIR)/map.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/player.o: $(SRC_DIR)/player.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/monster.o: $(SRC_DIR)/monster.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/renderer.o: $(SRC_DIR)/renderer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/hud.o: $(SRC_DIR)/hud.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/audio.o: $(SRC_DIR)/audio.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create output directory if missing
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Clean
clean:
	rm -f $(OUT_DIR)/*.o $(TARGET)

.PHONY: all clean
