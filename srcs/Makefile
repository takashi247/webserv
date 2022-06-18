SRCS	= main.cpp \
		server/server.cpp \
		server/socket.cpp \
		server/http_request_parser.cpp \
		response/http_response.cpp \
		config/config.cpp \
		config/location_config.cpp \
		config/parser_utils.cpp \
		config/server_config.cpp

CXX		= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
NAME	= webserv

OBJS	= $(SRCS:.cpp=.o)
DEPS	= $(SRCS:.cpp=.d)
INCS	= -Iconfig/ -Iresponse/ -Iserver/
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
