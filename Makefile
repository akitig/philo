# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akunimot <akitig24@gmail.com>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/09 14:59:44 by akunimot          #+#    #+#              #
#    Updated: 2025/03/09 16:01:48 by akunimot         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME         = philo
CC           = gcc
CFLAGS       = -Wall -Wextra -Werror -g -fsanitize=address

# ---------- srcs/objs ----------
SRC_DIR      = ./srcs
OBJ_DIR      = ./objs

SRCS         = $(SRC_DIR)/main.c \
				$(SRC_DIR)/ft_atoi.c \
				$(SRC_DIR)/ft_is.c \

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