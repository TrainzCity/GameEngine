# 1. Определение целевой платформы
ifeq ($(OS),Windows_NT)
    PLATFORM   := Windows
    EXE_EXT    := .exe
    RUN_PREFIX :=
    RM         := del /Q /F 2>nul || rm -f
else
    PLATFORM   := $(shell uname -s 2>/dev/null || echo Unknown)
    EXE_EXT    :=
    RUN_PREFIX := ./
    RM         := rm -f
endif

# Базовые переменные сборки
CC ?= gcc
BASE_CFLAGS ?= -Wall -Wextra -O2
TARGET_NAME = demo
TARGET = $(TARGET_NAME)$(EXE_EXT)

# 2. Каскадный поиск флагов SDL2 и SDL2_image
HAS_PKG_CONFIG := $(shell pkg-config --exists sdl2 SDL2_image 2>/dev/null && echo yes)
HAS_SDL2_CONFIG := $(shell which sdl2-config 2>/dev/null)

ifeq ($(HAS_PKG_CONFIG),yes)
    SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image)
    SDL_LIBS   := $(shell pkg-config --libs sdl2 SDL2_image)
else ifneq ($(HAS_SDL2_CONFIG),)
    SDL_CFLAGS := $(shell sdl2-config --cflags)
    SDL_LIBS   := $(shell sdl2-config --libs) -lSDL2_image
else
    ifeq ($(PLATFORM),Darwin)
        SDL_CFLAGS := -I/opt/homebrew/include/SDL2 -I/usr/local/include/SDL2 -D_THREAD_SAFE
        SDL_LIBS   := -L/opt/homebrew/lib -L/usr/local/lib -lSDL2 -lSDL2_image
    else ifeq ($(PLATFORM),Windows)
        SDL_CFLAGS := -Dmain=SDL_main
        SDL_LIBS   := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
    else
        SDL_CFLAGS := -I/usr/include/SDL2 -I/usr/local/include/SDL2 -D_REENTRANT
        SDL_LIBS   := -lSDL2 -lSDL2_image -lm
    endif
endif

CFLAGS = $(BASE_CFLAGS) $(SDL_CFLAGS)
LDLIBS += $(SDL_LIBS)

# Исходники основного проекта
SRCS = main.c backend.c graphic.c vec2.c automata.c animation.c object.c reaction.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

# 3. Конфигурация модульных тестов
TEST_MODULES = rect nav_graph

TEST_SRCS_rect      = rect.c rect_unit_test.c
TEST_SRCS_nav_graph = nav_graph.c nav_graph_test.c

TEST_BINS = $(addprefix test_,$(addsuffix $(EXE_EXT),$(TEST_MODULES)))

# Обработка аргумента в команде "make test <module>"
ifeq (test,$(firstword $(MAKECMDGOALS)))
    TEST_ARG := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
    ifneq ($(TEST_ARG),)
        INVALID_MODS := $(filter-out $(TEST_MODULES),$(TEST_ARG))
        ifneq ($(INVALID_MODS),)
            $(error Неизвестный модуль теста: $(INVALID_MODS). Доступны: $(TEST_MODULES))
        endif
        $(eval .PHONY: $(TEST_ARG))
        $(eval $(TEST_ARG): ;)
    endif
endif

SELECTED_TESTS := $(if $(TEST_ARG),$(TEST_ARG),$(TEST_MODULES))

.PHONY: all run test clean

all: $(TARGET)

# Линковка основного приложения
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDLIBS) -o $@

# Компиляция .c в .o с сохранением дерева зависимостей
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

# Генерация правил сборки и запуска для каждого тестового модуля
define MAKE_TEST_RULE
test_$(1)$(EXE_EXT): $$(TEST_SRCS_$(1))
	$$(CC) $$(BASE_CFLAGS) $$^ -o $$@

.PHONY: run-test-$(1)
run-test-$(1): test_$(1)$(EXE_EXT)
	@echo === Testing $(1) ===
	$$(RUN_PREFIX)test_$(1)$(EXE_EXT)
endef

$(foreach mod,$(TEST_MODULES),$(eval $(call MAKE_TEST_RULE,$(mod))))

test: $(addprefix run-test-,$(SELECTED_TESTS))

# Кроссплатформенный запуск
run: $(TARGET)
	$(RUN_PREFIX)$(TARGET)

# Очистка
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET) $(TEST_BINS)
