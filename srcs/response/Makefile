NAME		:= a.out

CXX			:= c++

CXXFLAGS	:= -Wall -Wextra -Werror -MMD -MP -std=c++98 -pedantic

SRCSDIR		:= ./

OBJSDIR		:= objs/

SRCS		:= main.cpp \
					http_response.cpp \
					server_config.cpp

OBJS		:= $(addprefix $(OBJSDIR), $(SRCS:.cpp=.o))

DEPS		:= $(OBJS:.o=.d)

SRCS		:= $(addprefix $(SRCSDIR), $(SRCS))

VPATH		:= $(SRCSDIR)

INCLUDE		:= -I./ -I./tmp/

RM			:= rm -rf

DEBUG		:= -g

LEAKS   := -D LEAKS=1

all:		$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(OBJSDIR)%.o: %.cpp
	@if [ ! -d $(OBJSDIR) ]; then mkdir $(OBJSDIR); fi
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(OBJSDIR)
	$(RM) *.dSYM
	$(RM) *.txt

fclean:		clean
	$(RM) $(NAME)

re:			fclean all

debug:		CXXFLAGS += $(DEBUG)
debug:		re

leaks:		CXXFLAGS += $(LEAKS)
leaks:		test

-include $(DEPS)

.PHONY:		all clean fclean re debug leaks
