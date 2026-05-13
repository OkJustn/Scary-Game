# ─────────────────────────────────────────────────────────────────────────────
#  Makefile – Raylib Maze Portal
#  Folder layout:
#    main.c          – entry point
#    src/            – module sources  (map, player, monster, renderer, hud)
#    include/        – headers
#    lib/            – raylib static/import libraries
#    imgs/           – textures (runtime, not compiled)
#    output/         – compiled binary
# ─────────────────────────────────────────────────────────────────────────────

# ── Toolchain ─────────────────────────────────────────────────────────────────
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -Llib
LIBS    = -lraylib -lopengl32 -lgdi32 -lwinmm

# ── Paths ─────────────────────────────────────────────────────────────────────
SRC_DIR = src
OUT_DIR = output
TARGET  = $(OUT_DIR)/main.exe

# ── Sources & Objects ─────────────────────────────────────────────────────────
# Gather every .c under src/ plus the root main.c
SRC_FILES = main.c \
            $(SRC_DIR)/map.c      \
            $(SRC_DIR)/player.c   \
            $(SRC_DIR)/monster.c  \
            $(SRC_DIR)/renderer.c \
            $(SRC_DIR)/hud.c

OBJ_FILES = $(patsubst %.c, $(OUT_DIR)/%.o, $(notdir $(SRC_FILES)))

# ── Default Target ────────────────────────────────────────────────────────────
all: $(OUT_DIR) $(TARGET)

# ── Link ──────────────────────────────────────────────────────────────────────
$(TARGET): $(OBJ_FILES)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@
	@echo "Build complete: $@"

# ── Compile: root main.c ──────────────────────────────────────────────────────
$(OUT_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Compile: src/*.c ─────────────────────────────────────────────────────────
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

# ── Create output directory if missing ───────────────────────────────────────
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# ── Clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -f $(OUT_DIR)/*.o $(TARGET)

.PHONY: all clean
