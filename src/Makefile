# Compiler & flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(LIBFT_DIR)

# Directories
LIBFT_DIR = libft
PARSER_DIR = parser
EXECUTER_DIR = executor
SIGNALS_DIR = signals
UTILS_DIR = utils

# libft files
LIBFT = $(LIBFT_DIR)/libft.a

# src files
SRC_FILES = $(PARSER_DIR)/lexer.c \
	$(PARSER_DIR)/parser.c	\
	$(PARSER_DIR)/expansion.c \
	$(EXECUTER_DIR)/exec.c \
	$(EXECUTER_DIR)/builtins.c \
	$(EXECUTER_DIR)/redirections.c \
	$(EXECUTER_DIR)/pipes.c	\
	$(SIGNALS_DIR)/signal_handler.c \
	$(UTILS_DIR)/env.c \
	$(UTILS_DIR)/free.c

# Object files
OBJ_FILES = $(SRC_FILES:.c=.o)

# Output files
NAME = minishell

# Default rule
all: $(LIBFT) $(NAME)

# Call libft's Makefile
$(LIBFT):
	@make -C $(LIBFT_DIR) --silent
	@echo "$(LIBFT) compiled! ✔️"

# Compile object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

# clean & remake

clean:
	@rm -f $(OBJ_FILES)
	@make -C $(LIBFT_DIR) clean --silent
	@echo "cleaned! 🧹"

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean --silent
	@echo "fully cleaned! 🗑️"

re: fclean all

.PHONY: all clean fclean re
