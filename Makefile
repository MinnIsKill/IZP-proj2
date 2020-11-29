  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    		adds debugging information to the executable file
  #  -std=c99	lets the compiler know that we're using the standard version of C (C99)
  #  -Wall 		turns on most, but not all, compiler warnings
  #  -Wextra	turns on the rest of compiler warnings
  #  -Werror	treats warnings as errors
  CFLAGS  = -std=c99 -Wall -Wextra -Werror

  # the build target executable:
  TARGET = sps

  all: $(TARGET)

  $(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

  clean:
	$(RM)	$(TARGET)