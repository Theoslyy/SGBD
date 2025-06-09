CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic -Iheader -g
LDFLAGS := 
EXEC := executavel

# Diretórios
SRC_DIR := src
CLASSES_DIR := $(SRC_DIR)/classes
OBJ_DIR := obj

# Arquivos fonte
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
CLASSES_FILES := $(wildcard $(CLASSES_DIR)/*.cpp)
ALL_SRC_FILES := $(SRC_FILES) $(CLASSES_FILES)

# Arquivos objeto
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES)) \
             $(patsubst $(CLASSES_DIR)/%.cpp,$(OBJ_DIR)/classes/%.o,$(CLASSES_FILES))

all: $(EXEC)

$(EXEC): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) $^ -o $@

# Regra para objetos do diretório principal
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para objetos do diretório classes
$(OBJ_DIR)/classes/%.o: $(CLASSES_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean