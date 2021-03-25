# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gjessica <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/12/08 23:16:50 by mondrew           #+#    #+#              #
#    Updated: 2021/03/25 22:10:16 by mondrew          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = a.out
SRCS = 	./main.cpp \
		ASocketOwner.cpp \
		Config.cpp \
		EventSelector.cpp \
		HTTPRequest.cpp \
		HTTPResponse.cpp \
		Location.cpp \
		Logger.cpp \
		Server.cpp \
		Session.cpp

INCLUDES = ./
FLAGS = -Wall -Wextra -Werror -std=c++98
OBJS = $(SRCS:.cpp=.o)

START_PRINT = \e[1;33m * webserv is starting\t\t\t\t\t\t\t\t$<\e[1;32m[ OK ] $<\e[0m
STOP_PRINT =  \e[1;33m * webserv is stopping\t\t\t\t\t\t\t\t$<\e[1;32m[ OK ] $<\e[0m
STATUS_RUN_PRINT = \e[0;32m *$<\e[1;33m webserv is running $<\e[0m
STATUS_NOT_RUN_PRINT = \e[0;31m *$<\e[1;33m webserv is not running $<\e[0m
ALREADY_LAUNCHED_PRINT = \e[1;32m *$<\e[1;33m webserv is already launched$<\e[0m
IS_NOT_LAUNCHED_PRINT = \e[0;31m *$<\e[1;33m webserv is not launched $<\e[0m

.PHONY: all clean fclean re

%.o: %.cpp
	@clang++ $(FLAGS) -I $(INCLUDES) -c $< -o $@
	@echo -n .
	@sleep 0.05
	@echo -n .
	@sleep 0.05
	@echo -n .

all: wait $(NAME)

$(NAME): $(OBJS)
	@clang++ $(OBJS) -o $(NAME)
	@/bin/rm -f $(OBJS)
	@echo "\nCompilation successfully done!"

clean:
	@/bin/rm -f $(OBJS)
	@echo "Object files has been removed!"

fclean: clean
	@/bin/rm -f $(NAME)
	@echo "Everything has been removed!"

wait:
	@echo "Compiling...Please wait..."

re: fclean all

run:
	@./$(NAME) configs/server.conf

start:
	@if pgrep -x $(NAME) > /dev/null; then\
		echo "$(ALREADY_LAUNCHED_PRINT)";\
	else\
		./$(NAME) configs/server.conf > /dev/null & echo "$(START_PRINT)";\
	fi

status:
	@if pgrep -x $(NAME) > /dev/null; then\
		echo "$(STATUS_RUN_PRINT)";\
	else\
		echo "$(STATUS_NOT_RUN_PRINT)";\
	fi

stop:
	@if pgrep -x $(NAME) > /dev/null; then\
		pkill $(NAME);\
	   	echo "$(STOP_PRINT)";\
	else\
		echo "$(IS_NOT_LAUNCHED_PRINT)";\
	fi

restart: re run
