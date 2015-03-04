CC = g++
CFLAGS = -Wall

TARGET = proj1_server
TARGET2 = proj1_client
TARGET3 = proj1_server4
TARGET4 = proj1_client4

 
all: $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET)

$(TARGET2): $(TARGET2).cpp
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp

$(TARGET3): $(TARGET3).cpp
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET3).cpp -std=c++11

$(TARGET4): $(TARGET4).cpp
	$(CC) $(CFLAGS) -o $(TARGET4) $(TARGET4).cpp


clean:
	$(RM) $(TARGET) $(TARGET2) $(TARGET3) $(TARGET4)
