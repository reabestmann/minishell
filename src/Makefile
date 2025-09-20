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
BUILTIN_DIR = builtins

# libft files
LIBFT = $(LIBFT_DIR)/libft.a

# src files 
SRC_FILES = main.c \
$(PARSER_DIR)/lexer.c \
$(PARSER_DIR)/parse.c \
$(PARSER_DIR)/expand.c \
$(PARSER_DIR)/parse_utils.c \
$(EXECUTER_DIR)/exec.c \
$(EXECUTER_DIR)/redir.c \
$(EXECUTER_DIR)/pipes.c \
$(SIGNALS_DIR)/signal_handler.c \
$(SIGNALS_DIR)/parent_child_setup.c \
$(UTILS_DIR)/utils.c \
$(UTILS_DIR)/trim_quotes.c \
$(UTILS_DIR)/free.c \
$(UTILS_DIR)/struct_to_ptr.c \
$(UTILS_DIR)/ptr_to_struct.c \
$(BUILTIN_DIR)/cd_cmd.c \
$(BUILTIN_DIR)/echo_cmd.c \
$(BUILTIN_DIR)/pwd_cmd.c \
$(BUILTIN_DIR)/exit_cmd.c \
$(BUILTIN_DIR)/unset_cmd.c \
$(BUILTIN_DIR)/env_cmd.c
# $(BUILTIN_DIR)/export_cmd.c

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
