NAME := WebServ

CC := c++
RM := rm -f
CFLAGS := -Wall -Wextra -Werror -std=c++98

# proyect directories
SRC_DIR := src
INCLUDE_DIR := include
OBJ_DIR := obj

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -fsanitize=leak
endif

# Source files without SRC_DIR
SRC := set_config/set_config.cpp\
	main.cpp\
	class/HttpResponse.cpp\
	class/HttpRequest.cpp\
	utils.cpp

# Setup obj and src files and obj folders
SRC := $(addprefix $(SRC_DIR)/,$(SRC))
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ_FOLDER := $(sort $(dir $(OBJ)))

# Add -l, -L and -I flags
INCLUDE_PATH_FLAG := -I$(INCLUDE_DIR) $(addprefix -I,$(sort $(dir $(SRC))))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE_PATH_FLAG) $(LIB_PATH_FLAG) $(OBJ) $(LIB_FLAG) $(FRAMEWORK) -o $@

%.a:
	@make -C $(dir $@)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
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
