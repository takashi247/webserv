SRCS	= main.cpp \
		server.cpp \
		socket.cpp \
		http_request_parser.cpp

CXX		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -D DUMMY_RESPONSE
NAME	= webserv

OBJS	= $(SRCS:.cpp=.o)
DEPS	= $(SRCS:.cpp=.d)
INCS	= -I../ -I./ -Istub/
RM		= rm -f

.cpp.o:
		$(CXX) $(CXXFLAGS) $(INCS) -c -MMD -MP -MF $(<:.cpp=.d) $< -o $(<:.cpp=.o)

$(NAME):	$(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^

-include $(DEPS)

all:	$(NAME)

ref:	$(NAME)

clean:
		$(RM) $(OBJS) $(DEPS) *.d *.o

fclean:	clean
		$(RM) $(NAME)

re:	fclean all

.PHONY:		all ref clean fclean re
