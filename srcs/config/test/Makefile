PARSE_SRCS :=	../config.cpp	\
				../config_parser.cpp	\
				../location_config.cpp	\
				../parser_utils.cpp		\
				../server_config.cpp	\
				../../exception/webserv_exception.cpp	\
				store.cpp 

SELECT_SRCS :=	../config.cpp	\
				../config_parser.cpp	\
				../location_config.cpp	\
				../parser_utils.cpp		\
				../server_config.cpp	\
				select_server_config_test.cpp 

UPDATE_SRCS :=	../config.cpp	\
				../config_parser.cpp	\
				../location_config.cpp	\
				../parser_utils.cpp		\
				../server_config.cpp	\
				update_uri_test.cpp 

parse: $(PARSE_SRCS)
	c++ -I../../exception -g -o parse $^

select: $(SELECT_SRCS)
	c++ -o select $^

update: $(UPDATE_SRCS)
	c++ -o update $^
