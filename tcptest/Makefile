all:recv send


recv:tcprecv.c
	gcc $< -o $@
send:tcpsend.c
	gcc $< -o $@

.PHONY:
clean:
	rm recv send
