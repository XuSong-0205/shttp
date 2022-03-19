.PHONY : clean

CURR_DIR := $(shell pwd)
BIN_DIR := bin
SRC_DIR := src
BUILD_DIR := build


SRC := $(SRC_DIR)/ \
	$(SRC_DIR)/socket/ \
	$(SRC_DIR)/utils/ \
	$(SRC_DIR)/shttp/


VPATH := $(SRC)


SRC_CPP := $(foreach dir, $(SRC), $(wildcard $(dir)/*.cpp))
OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(SRC_CPP:.cpp=.o)))


INCLUDE := $(SRC_DIR)/json/
LINK_FLAGS := -lpthread


CXX := g++
CXXFLAGS := -Wall -g -std=c++14 -I $(INCLUDE)


SHTTP_NAME := shttp
SHTTP := $(BIN_DIR)/$(SHTTP_NAME)
START := start



$(SHTTP) : $(OBJS)
	$(CXX) $^ -o $@ $(LINK_FLAGS)
	@echo ---------over---------


$(BUILD_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(START) : 
	cd $(CURR_DIR)/$(BIN_DIR) && ./$(SHTTP_NAME)



clean : 
	rm -f $(BUILD_DIR)/*
	rm -f $(BIN_DIR)/*