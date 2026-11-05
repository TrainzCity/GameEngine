# 1. Определение целевой платформы
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    PLATFORM := $(shell uname -s 2>/dev/null || echo Unknown)
endif

# Базовые переменные сборки
CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
TARGET_NAME = demo

# Настройка расширения и команд под платформу
ifeq ($(PLATFORM),Windows)
    TARGET = $(TARGET_NAME).exe
    RM = del /Q /F 2>nul || rm -f
else
    TARGET = $(TARGET_NAME)
    RM = rm -f
endif

# 2. Каскадный поиск флагов SDL2 и SDL2_image
# Проверяем наличие pkg-config и пакетов
HAS_PKG_CONFIG := $(shell pkg-config --exists sdl2 SDL2_image 2>/dev/null && echo yes)
# Проверяем наличие sdl2-config (стандарт для Debian/Ubuntu/Fedora)
HAS_SDL2_CONFIG := $(shell which sdl2-config 2>/dev/null)

ifeq ($(HAS_PKG_CONFIG),yes)
    # Вариант А: pkg-config нашел оба пакета
    SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image)
    SDL_LIBS   := $(shell pkg-config --libs sdl2 SDL2_image)
else ifneq ($(HAS_SDL2_CONFIG),)
    # Вариант Б: fallback на sdl2-config + SDL2_image
    SDL_CFLAGS := $(shell sdl2-config --cflags)
    SDL_LIBS   := $(shell sdl2-config --libs) -lSDL2_image
else
    # Вариант В: жесткий fallback по путям ОС, если утилит конфигурации нет в PATH
    ifeq ($(PLATFORM),Darwin)
        # macOS (Homebrew пути для Apple Silicon / Intel)
        SDL_CFLAGS := -I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2 -D_THREAD_SAFE
        SDL_LIBS   := -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lSDL2_image
    else ifeq ($(PLATFORM),Windows)
        # Windows / MinGW
        SDL_CFLAGS := -Dmain=SDL_main
        SDL_LIBS   := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
    else
        # Универсальный Linux / BSD (Debian, Ubuntu, Alpine, RHEL)
        SDL_CFLAGS := -I/usr/include/SDL2 -I/usr/local/include/SDL2 -D_REENTRANT
        SDL_LIBS   := -lSDL2 -lSDL2_image -lm
    endif
endif

CFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS)

# Исходники и генерация списков .o / .d
SRCS = main.c test_graphic.c backend.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all run clean

all: $(TARGET)

# Линковка
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDLIBS) -o $@

# Компиляция .c в .o с сохранением дерева зависимостей
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# Кроссплатформенный запуск
run: $(TARGET)
ifeq ($(PLATFORM),Windows)
	$(TARGET)
else
	./$(TARGET)
endif

# Очистка
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)
