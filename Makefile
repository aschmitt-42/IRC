NAME		= IRC
	
CC			= c++
FLAGS		= -Wall -Wextra -Werror -std=c++98
RM			= rm -rf

OBJDIR		= .objFiles
SRCDIR		= srcs
INCDIR		= inc

FILES		= main server server_aux
INCFILE		= irc

SRC			= $(addprefix $(SRCDIR)/, $(FILES:=.cpp))
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER		= $(addprefix $(INCDIR)/, $(INCFILE:=.hpp))

all: $(NAME)

$(NAME): $(OBJ) $(HEADER) 
	$(CC) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADER) Makefile
	mkdir -p $(OBJDIR)
	$(CC) $(FLAGS) -I $(INCDIR) -c $< -o $@

clean:
	$(RM) $(OBJDIR) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re