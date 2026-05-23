NAME        :=  RPN

SRC_DIR     :=  src
OBJ_DIR     :=  bin
INC_DIR     :=  include

FILES       :=  main.cpp \
				RPN.cpp

SRC         :=  $(addprefix $(SRC_DIR)/, $(FILES))
OBJ         :=  $(addprefix $(OBJ_DIR)/, $(FILES:%.cpp=%.o))
DEP         :=  $(OBJ:.o=.d)

CXX         :=  c++
WFLAGS      :=  -Wall -Wextra -Werror
STDFLAGS    :=  -std=c++98 -pedantic
DEPFLAGS    :=  -MMD -MP
INCLUDE     :=  -I$(INC_DIR)

CXXFLAGS    :=  $(WFLAGS) $(STDFLAGS) $(DEPFLAGS) $(INCLUDE)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP)

.PHONY: all clean fclean re
