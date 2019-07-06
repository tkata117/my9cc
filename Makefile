CFLAGS=-std=c11 -Wall -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

TEST_DIR=./test
TEST_SRCS=$(wildcard $(TEST_DIR)/*.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)


9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc $(TEST_OBJS)
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp* $(TEST_DIR)/*~

.PHONY: test clean
