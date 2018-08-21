
NAME = ft_traceroute

CC = gcc

SRCDIR = src

CFLAGS  = -Wall -Werror -Wextra -g

INCL = -I incl

LIBFT_INCL = -I./libft/incl

LIBFT = ./libft/libftprintf.a

MAIN = main.c

SRC = traceroute.c

SRCFILES = $(addprefix $(SRCDIR)/, $(SRC))

RM = rm -fr

$(NAME):
		$(MAKE) -C ./libft/ re
		$(CC) $(CFLAGS) $(INCL) $(MAIN) $(SRCFILES) $(LIBFT) -o $(NAME)

clean:
		$(RM) $(OBJDIR)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)
