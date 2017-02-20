program_NAME := myprogram
program_C_SRCS := $(wildcard *.c)
program_C_OBJS := ${program_C_SRCS:.c=.o}

all: $(program_NAME)

$(program_NAME): $(program_C_OBJS)
	$(LINK.cc) $(program_C_OBJS) -o $(program_NAME)