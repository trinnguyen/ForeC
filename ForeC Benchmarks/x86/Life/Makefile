ifeq ($(shell uname), Linux)
	CC			= clang
	CFLAGS		= -pthread -g
else ifeq ($(shell uname), Darwin)
	CC			= clang
	CFLAGS		= -pthread -g
	LDFLAGS		= -I/opt/X11/include -L/opt/X11/lib
else
	CC			= gcc
endif

forec:
	forecc Life.forec Life.1.foreh

c:
	$(CC) $(CFLAGS) -Wall Life.c -o Life

dot:
	dot -T pdf Life.dot -o Life.pdf

visual.forec:
	forecc Life.visual.forec Life.visual.foreh

visual.c:
	$(CC) $(CFLAGS) -Wall Life.visual.c -o Life.visual $(LDFLAGS) -lX11

visual.dot:
	dot -T pdf Life.visual.dot -o Life.visual.pdf

clean:
	rm -f Life Life.visual *.exe *.c *.dot *.txt *.xml *.pdf

