CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -I. -Isrc/app/encryptDecrypt -Isrc/app/fileHandling -Isrc/app/processes

MAIN_TARGET = encrypt_decrypt
Enryption_TARGET = Enryption

MAIN_SRC = main.cpp \
           src/app/processes/ProcessManagement.cpp \
           src/app/fileHandling/IO.cpp \
           src/app/fileHandling/ReadEnv.cpp \
           src/app/encryptDecrypt/Enryption.cpp

Enryption_SRC = src/app/encryptDecrypt/EnryptionMain.cpp \
               src/app/encryptDecrypt/Enryption.cpp \
               src/app/fileHandling/IO.cpp \
               src/app/fileHandling/ReadEnv.cpp

MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
Enryption_OBJ = $(Enryption_SRC:.cpp=.o)

all: $(MAIN_TARGET) $(Enryption_TARGET)

$(MAIN_TARGET): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(Enryption_TARGET): $(Enryption_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MAIN_OBJ) $(Enryption_OBJ) $(MAIN_TARGET) $(Enryption_TARGET)

.PHONY: clean all