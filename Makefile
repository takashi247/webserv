NAME		:= webserv

CXX			:= c++

CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -MMD -MP -pedantic

SRCSDIR		:= srcs/

VPATH		:= $(SRCSDIR)

OBJSDIR		:= objs/

SRCS	:= main.cpp \
		server/server.cpp \
		server/client_socket.cpp \
		server/server_socket.cpp \
		server/http_request_parser.cpp \
		server/receive_body.cpp \
		response/http_response.cpp \
		utility/wrapper.cpp \
		config/config_parser.cpp \
		config/config.cpp \
		config/location_config.cpp \
		config/parser_utils.cpp \
		config/server_config.cpp \
		exception/webserv_exception.cpp

OBJS		:= $(addprefix $(OBJSDIR), $(SRCS:.cpp=.o))

DEPS		:= $(OBJS:.o=.d)

SRCS		:= $(addprefix $(SRCSDIR), $(SRCS))

INCLUDE		:= -I$(SRCSDIR)config/ -I$(SRCSDIR)response/ -I$(SRCSDIR)server/ -I$(SRCSDIR)exception/ -I$(SRCSDIR)utility/

RM			:= rm -rf

DEBUG		:= -g -fsanitize=address

LEAKS		:= -D LEAKS=1

LOG			:= -D LOG=1

all:		$(NAME)

$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@

$(OBJSDIR)%.o: %.cpp
	mkdir -p $(OBJSDIR)$(*D)
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
leaks:		re

log:			CXXFLAGS += $(LOG)
log:			re

-include $(DEPS)

.PHONY:		all clean fclean re debug leaks
