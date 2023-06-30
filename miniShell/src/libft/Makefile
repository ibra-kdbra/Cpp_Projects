SRC_LST_FCTS := ft_lstadd_back.c \
				ft_lstadd_front.c \
				ft_lstclear.c \
				ft_lstdelone.c \
				ft_lstiter.c \
				ft_lstlast.c \
				ft_lstmap.c \
				ft_lstnew.c \
				ft_lstsize.c
SRC_LST_FCTS := $(addprefix lst_fcts/, $(SRC_LST_FCTS))

SRC_MEMORY_FCTS := ft_bzero.c \
				   ft_calloc.c \
				   ft_memchr.c \
				   ft_memcmp.c \
				   ft_memcpy.c \
				   ft_memmove.c \
				   ft_memset.c \
				   ft_free_arr.c
SRC_MEMORY_FCTS := $(addprefix memory_fcts/, $(SRC_MEMORY_FCTS))

SRC_STRING_FCTS :=  ft_array_size.c \
					ft_atoi.c \
					ft_erratoi.c \
					ft_itoa.c \
					ft_split.c \
					ft_strchr.c \
					ft_strdup.c \
					ft_striteri.c \
					ft_strjoin.c \
					ft_strjoin_add_space.c \
					ft_strjoinf.c \
					ft_strlcat.c \
					ft_strlcpy.c \
					ft_strlen.c \
					ft_strmapi.c \
					ft_strncmp.c \
					ft_strcmp.c \
					ft_strnstr.c \
					ft_strrchr.c \
					ft_strtrim.c \
					ft_substr.c \
					ft_tolower.c \
					ft_toupper.c \
					ft_strtok.c \
					ft_atoll.c \
					ft_erratoll.c
SRC_STRING_FCTS:= $(addprefix string_fcts/, $(SRC_STRING_FCTS))

SRC_WRITE_FCTS := ft_dprintf.c \
				  ft_putchar_fd.c \
				  ft_putendl_fd.c \
				  ft_puthex_fd.c \
				  ft_putnbr_fd.c \
				  ft_putptr_fd.c \
				  ft_putstr_fd.c
SRC_WRITE_FCTS := $(addprefix write_fcts/, $(SRC_WRITE_FCTS))

SRC_CHECK_FCTS := ft_isext.c \
				  ft_isalnum.c \
				  ft_isalpha.c \
				  ft_isascii.c \
				  ft_isdigit.c \
				  ft_isprint.c \
				  ft_isblank.c \
				  ft_arealnum.c \
				  ft_arealpha.c \
				  ft_areascii.c \
				  ft_aredigit.c \
				  ft_areprint.c \
				  ft_areblank.c
SRC_CHECK_FCTS := $(addprefix check_fcts/, $(SRC_CHECK_FCTS))

SRC_INPUT_FCTS := get_next_line.c
SRC_INPUT_FCTS := $(addprefix input_fcts/, $(SRC_INPUT_FCTS))

SRC_A += $(SRC_LST_FCTS) 
SRC_A += $(SRC_MEMORY_FCTS) 
SRC_A += $(SRC_STRING_FCTS) 
SRC_A += $(SRC_WRITE_FCTS) 
SRC_A += $(SRC_CHECK_FCTS) 
SRC_A += $(SRC_INPUT_FCTS)

OBJ_A := $(SRC_A:.c=.o)

TARGET := libft.a
CC := clang
CFLAGS := -Wall -Werror -Wextra -g

# --- Printf Utils --- #
RPATH := `realpath --relative-to=${PWD} ${CURDIR}`/
RPATH := ${shell if [ "${RPATH}" != "./" ]; then echo ${RPATH}; fi}
NORMAL := ${shell tput sgr0}
RED := ${shell tput setaf 1}
GREEN := ${shell tput setaf 2}
BLUE := ${shell tput setaf 4}
green_plus := $(BLUE)[$(GREEN)+$(BLUE)]$(NORMAL)
red_minus := $(BLUE)[$(RED)-$(BLUE)]$(NORMAL)

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf '$(green_plus) $(BLUE)$(RPATH)$@$(NORMAL)\n'

$(TARGET): $(OBJ_A)
	@ar rcs $(TARGET) $(OBJ_A)
	@printf '$(green_plus) $(BLUE)$(RPATH)$(TARGET)$(NORMAL)\n'

all: $(TARGET)

clean:
	@rm -f $(OBJ_A)
	@${foreach OBJ, $(OBJ_A), \
		printf '$(red_minus) $(BLUE)$(RPATH)$(OBJ)$(NORMAL)\n';}

fclean: clean
	@rm -f $(TARGET)
	@printf '$(red_minus) $(BLUE)$(RPATH)$(TARGET)$(NORMAL)\n'

re: fclean
	@make -s all

.PHONY: all clean fclean re
