NAME := webserv

CC := c++
RM := rm -f
CFLAGS := -Wall -Wextra -Werror -std=c++98

# proyect directories
INCLUDE_DIR := src/include
OBJ_DIR := obj

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -fsanitize=leak
endif

# Source files without SRC_DIR
SRC := src/class/Client.cpp\
	src/class/HttpRequest.cpp\
	src/class/HttpResponse.cpp\
	src/class/Listener.cpp\
	src/class/Location.cpp\
	src/class/LocationContainer.cpp\
	src/class/Server.cpp\
	src/class/Webserv.cpp\
	src/main.cpp\
	src/methods/DELETE.cpp\
	src/methods/GET.cpp\
	src/methods/POST.cpp\
	src/readConfig/readConfig.cpp\
	src/utils.cpp

# Setup obj and src files and obj folders
OBJ := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
OBJ_FOLDER := $(sort $(dir $(OBJ)))

# Add -l, -L and -I flags
INCLUDE_PATH_FLAG := -I$(INCLUDE_DIR) $(addprefix -I,$(sort $(dir $(SRC))))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE_PATH_FLAG) $(LIB_PATH_FLAG) $(OBJ) $(LIB_FLAG) $(FRAMEWORK) -o $@

%.a:
	@make -C $(dir $@)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_FOLDER)
	$(CC) $(CFLAGS) $(INCLUDE_PATH_FLAG) -c -o $@ $<

clean:
	@if [ -d $(OBJ_DIR) ]; then \
		echo "Deleted $(OBJ_DIR) folder"; \
		$(RM) -rf $(OBJ_DIR);\
	fi

fclean: clean
	@if [ -f $(NAME) ]; then \
		echo "Deleted $(NAME) executable";\
		$(RM) $(NAME); \
	fi

re: fclean all

.PHONY: clean fclean all re $(LIB_PATH) commit
