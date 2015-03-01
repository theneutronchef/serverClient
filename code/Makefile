CC = g++
CFLAGS = -Wall

TARGET = proj1_server
TARGET2 = proj1_client

all: $(TARGET) $(TARGET2)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET)

$(TARGET2): $(TARGET2).cpp
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp

clean:
	$(RM) $(TARGET) $(TARGET2)
