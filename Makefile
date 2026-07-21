NAME	= codexion

CC		= cc

CFLAGS	= -Wall -Wextra -Werror -pthread

SRCDIR	= coders

SRCS	= $(SRCDIR)/main.c \
			$(SRCDIR)/cycle.c \
			$(SRCDIR)/scheduler.c \
			$(SRCDIR)/monitor.c \
			$(SRCDIR)/helpers.c \
			$(SRCDIR)/parsing.c \
			$(SRCDIR)/setup.c \
			$(SRCDIR)/clean_safe.c \
			$(SRCDIR)/single_coder.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
