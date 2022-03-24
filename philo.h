#ifndef PHILO_H
# define PHILO_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

typedef struct t_philo
{
    int id;
    int x_ate;
    int left_fork;
    int right_fork;
    long long   time_end;
    struct t_rules  *rules;
    pthread_t thr;
}   t_philo;

typedef struct t_rules
{
    t_philo philosopher[250];
	pthread_mutex_t	fork[250];
	pthread_mutex_t message;
	pthread_mutex_t	meal;
	int	num_philo;
	int	time_die;
	int	time_eat;
	int	time_sleep;
	int	num_eat;
	int	meal_count;
	int	died;
    long long	time_start;
}	t_rules;


int ft_strlen(char *s);
int	ft_error_message(char *msg);
int	ft_isnum(char *str);
int	ft_myatoi(char *str);
long long    ft_current_time(void);
int	ft_rules_init(t_rules *rules, char **argv);
int ft_mutex_init(t_rules *rules);
int ft_philo_init(t_rules *rules);
int ft_thread_create(t_rules *rules);
void    *mythread(void *philo);
int main(int argc, char **argv);
void    ft_display_message(t_rules *rules, int id, char *msg);
void	ft_sleep(t_rules *rules, long long rest);
void    ft_philo_eat(t_philo *phi);
void    ft_thread_exit(t_rules *rules, t_philo *philo);
void    ft_check_death(t_rules *rules, t_philo *phi);



#endif


