all:
	gcc -pthread -lpthread philosopher.c -o philo
debug:
	gcc -pthread -lpthread -g -fsanitize=address philosopher.c -o philo
clean:
	rm -f philo
