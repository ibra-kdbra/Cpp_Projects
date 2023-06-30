MAKEFLAGS += -s -j
.DEFAULT_GOAL = all

# --- Printf Utils --- #
RPATH := $(shell realpath --relative-to=${PWD} ${CURDIR})/
RPATH := ${shell if [ "${RPATH}" != "./" ]; then echo ${RPATH}; fi}
NORMAL := ${shell tput sgr0}
RED := ${shell tput setaf 1}
GREEN := ${shell tput setaf 2}
BLUE := ${shell tput setaf 4}
green_plus := $(BLUE)[$(GREEN)+$(BLUE)]$(NORMAL)
red_minus := $(BLUE)[$(RED)-$(BLUE)]$(NORMAL)

# --- Project sources --- #
SRC := main.c\
	   prompt.c\
	   signal_handlers.c\
	   utils/env_list_functions.c\
	   utils/env_list_functions_bis.c\
	   utils/is_utils.c\
	   utils/env_array_functions.c
SRC_BUILTINS := cd/cd.c\
				echo/echo.c\
				env/env.c\
				export/export.c\
				export/export_bis.c\
				export/export_utils.c\
				pwd/pwd.c\
				unset/unset.c\
				exit/exit.c
SRC_BUILTINS := $(addprefix builtins/, $(SRC_BUILTINS))
SRC_PARSING := parse_input.c\
			   parse_input_bis.c\
			   add_space_operators.c\
			   split_cmd.c\
			   tokenise_input.c\
			   expand_env_var.c\
			   expand_env_var_bis.c\
			   remove_quote.c\
			   parse_tokens.c\
			   parse_tokens_bis.c\
			   utils_parsing.c
SRC_PARSING := $(addprefix parsing/, $(SRC_PARSING))
SRC_EXEC := file_handling.c \
			heredoc_read.c \
			heredoc_use.c \
			exec_utils.c \
			init_cmd.c \
			main_exec.c \
			main_exec_bis.c \
			check_dir.c \
			builtins_hdl.c
SRC_EXEC := $(addprefix exec/, $(SRC_EXEC))

SRC_ALL := ${SRC} ${SRC_BUILTINS} ${SRC_PARSING} ${SRC_EXEC}

OBJ := $(addprefix obj/, $(SRC_ALL:.c=.o))

HDRS := minishell.h\
		def_struct.h \
		exec_defs.h
HDRS := $(addprefix inc/, $(HDRS))

NAME := minishell

CC := clang
CFLAGS := -Wall -Werror -Wextra -g
LIBS := -lreadline

obj/%.o: src/%.c $(HDRS)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf '$(green_plus) $(BLUE)$(RPATH)$@$(NORMAL)\n'

all:
	@clear -x
	@make -s ${NAME}

$(NAME): $(OBJ)
	@make -sC src/libft
	@$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $(NAME) -Lsrc/libft -lft
	@printf '$(green_plus) $(BLUE)$(RPATH)$(NAME)$(NORMAL)\n'

clean:
	@make -sC src/libft fclean
	@rm -rf ./obj/
	@printf '${red_minus} ${BLUE}${RPATH}obj/${NORMAL}\n'

fclean: clean
	@rm -f $(NAME)
	@printf '$(red_minus) $(BLUE)$(RPATH)$(NAME)$(NORMAL)\n'

re: fclean
	@make -s

run:
	@make -s
	@clear -x
	@valgrind -s --leak-check=full --show-leak-kinds=all --suppressions=vg_supp --track-fds=yes ./minishell

norm:
	@norminette | grep -v OK

.PHONY: all clean fclean re run
