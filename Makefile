
NAME = traceroute

CC = gcc

SRCDIR = src

CFLAGS  = -Wall -Werror -Wextra

INCL = -I incl

LIBFT = ./libft/libftprintf.a

SRC = main.c traceroute.c send_recv_echo.c check_packet.c handle_response.c
SRC += set_port_ttl_probe.c set_addr_iface.c useage.c

SRCFILES = $(addprefix $(SRCDIR)/, $(SRC))

RM = rm -fr

$(NAME):
		$(MAKE) -C ./libft/ re
		$(CC) $(CFLAGS) $(INCL) $(SRCFILES) $(LIBFT) -o $(NAME)

clean:
		$(RM) $(OBJDIR)

fclean: clean
		$(RM) $(NAME)

re: fclean $(NAME)
