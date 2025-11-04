# Compiler & flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(LIBFT_DIR)
LDFLAGS = -lreadline -lncurses
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
$(PARSER_DIR)/lexer_token_utils.c \
$(PARSER_DIR)/parse.c \
$(PARSER_DIR)/parse_utils.c \
$(PARSER_DIR)/expand.c \
$(PARSER_DIR)/expand_arg.c \
$(PARSER_DIR)/expand_basics.c \
$(PARSER_DIR)/expand_split_replace.c \
$(PARSER_DIR)/expand_split_whitespace.c \
$(PARSER_DIR)/expand_utils.c \
$(PARSER_DIR)/syntax.c \
$(PARSER_DIR)/quote_state.c \
$(EXECUTER_DIR)/exec.c \
$(EXECUTER_DIR)/heredoc.c \
$(EXECUTER_DIR)/heredoc_utils.c \
$(EXECUTER_DIR)/redir_heredoc1.c \
$(EXECUTER_DIR)/redir_heredoc2.c \
$(EXECUTER_DIR)/redir_utils.c \
$(EXECUTER_DIR)/redir_utils2.c \
$(EXECUTER_DIR)/redir.c \
$(EXECUTER_DIR)/pipes.c \
$(EXECUTER_DIR)/exec_utils.c \
$(EXECUTER_DIR)/path.c \
$(EXECUTER_DIR)/path_utils.c \
$(SIGNALS_DIR)/signal_handler.c \
$(SIGNALS_DIR)/parent_child_setup.c \
$(UTILS_DIR)/utils.c \
$(UTILS_DIR)/trim_quotes.c \
$(UTILS_DIR)/free.c \
$(UTILS_DIR)/error.c \
$(UTILS_DIR)/struct_to_ptr.c \
$(UTILS_DIR)/ptr_to_struct.c \
$(UTILS_DIR)/get_next_line.c \
$(BUILTIN_DIR)/cd_cmd.c \
$(BUILTIN_DIR)/cd_cmd_utils.c \
$(BUILTIN_DIR)/echo_cmd.c \
$(BUILTIN_DIR)/pwd_cmd.c \
$(BUILTIN_DIR)/exit_cmd.c \
$(BUILTIN_DIR)/unset_cmd.c \
$(BUILTIN_DIR)/unset_cmd_utils.c \
$(BUILTIN_DIR)/env_cmd.c \
$(BUILTIN_DIR)/export_cmd.c \
$(BUILTIN_DIR)/export_cmd_bubble.c \
$(BUILTIN_DIR)/export_cmd_keys.c \
$(BUILTIN_DIR)/export_cmd_utils.c


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
