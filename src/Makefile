# Compiler & flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(LIBFT_DIR) -I/opt/homebrew/opt/readline/include 
LDFLAGS = -L/opt/homebrew/opt/readline/lib -lreadline -lhistory

# Directories
LIBFT_DIR = libft
PARSER_DIR = parser
EXECUTER_DIR = executor
SIGNALS_DIR = signals
UTILS_DIR = utils

# libft files
LIBFT = $(LIBFT_DIR)/libft.a

# src files 
# $(PARSER_DIR)/lexer.c
# $(PARSER_DIR)/archived_split.c
# $(PARSER_DIR)/expansion.c
# $(PARSER_DIR)/parse.c
# $(PARSER_DIR)/parse_utils.c
SRC_FILES = main.c \
$(EXECUTER_DIR)/exec.c \
$(EXECUTER_DIR)/builtins.c \
$(EXECUTER_DIR)/redirections.c \
$(EXECUTER_DIR)/pipes.c \
$(SIGNALS_DIR)/signal_handler.c \
$(SIGNALS_DIR)/parent_child_setup.c \
$(UTILS_DIR)/env.c \
$(UTILS_DIR)/parser_stub_comptest.c \
# $(UTILS_DIR)/free.c

# Object files
OBJ_FILES = $(SRC_FILES:.c=.o)

# Output files
NAME = minishell

# Default rule
all: $(LIBFT) $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBFT) $(LDFLAGS) -o $(NAME)

# Call libft's Makefile
$(LIBFT):
	@make -C $(LIBFT_DIR) --silent
	@echo "$(LIBFT) compiled! ✔️"

# Compile object files
%.o: %.c
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
