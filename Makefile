NAME         = philo
CC           = cc
CFLAGS       = -Wall -Wextra -Werror -g

# ---------- srcs/objs ----------
SRC_DIR      = ./srcs
OBJ_DIR      = ./objs

SRCS         = $(SRC_DIR)/main.c

OBJS         = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ==============================================================================

all: $(LIBFT) $(NAME)

# ---------- philo ----------
$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(READLINE) -o $(NAME)
	@echo "philo compiled."

# ---------- make objs ----------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I./includes -c $< -o $@

# ---------- clean --------------
clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re