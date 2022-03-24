#include "philo.h"

int ft_strlen(char *s)
{
  	int	i;

	i = 0;
	while (s[i])
		i++;
    return (i);
}

int	ft_error_message(char *msg)
{
    int l;

    l = ft_strlen(msg);
	write(2,"Error ", 6);
	write(2, msg, l);
	write(2, "\n", 1);
	return (1);
}

void    ft_display_message(t_rules *rules, int id, char *msg)
{
    pthread_mutex_lock(&(rules->message));
    if (!(rules->died))
    {
        printf("%lld ",ft_current_time() - rules->time_start);
        printf("%d ", id + 1);
        printf("%s\n",msg);
    }
    pthread_mutex_unlock(&(rules->message));
    return ;
}
int	ft_isnum(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	ft_myatoi(char *str)
{
	int	i;
	int	n;

	i = 0;
	n = 0;
	if (str[i] == '+')
		i++;
	if (ft_isnum(&str[i]))
	{
		while (str[i])
		{
			n = n * 10 + (str[i] - 48);
			i++;
		}
		if (n >= 0 && n <= 2147483647)
			return (n);
		else
			return (ft_error_message("Out of range."));
	}
	else
		return (ft_error_message("Invalid argument."));
}

long long    ft_current_time()
{
    struct timeval tv;
    long long    ct;

    gettimeofday(&tv, NULL);
    ct = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return (ct);
}

void	ft_sleep(t_rules *rules, long long rest)
{
	long long ct;

	ct = ft_current_time();
    while (!(rules->died))
    {
        if ((ft_current_time() - ct) >= rest)
            break ;
        usleep(50);
    }
}

int	ft_rules_init(t_rules *rules, char **argv)
{
	rules->num_philo = ft_myatoi(argv[1]);
    rules->time_die = ft_myatoi(argv[2]);
    rules->time_eat = ft_myatoi(argv[3]);
    rules->time_sleep = ft_myatoi(argv[4]);
    rules->meal_count = 0;
    rules->died = 0;

    if (rules->num_philo < 2 || rules->time_die < 0
        || rules->time_eat < 0 || rules->time_sleep < 0
        || rules->num_philo > 250)
        return (1);
  
    if (argv[5])
    {
        rules->num_eat = ft_myatoi(argv[5]);
        if (rules->num_eat <= 0)
            return (1);
    }
    else
        rules->num_eat = -1;
	return (0);
}

int ft_mutex_init(t_rules *rules)
{
    int i;

    i = rules->num_philo;
    while (--i >= 0)
    {
        if (pthread_mutex_init(&(rules->fork[i]), NULL))
            return (1);
    }
    if (pthread_mutex_init(&(rules->message), NULL))
        return (1);
    if (pthread_mutex_init(&(rules->meal), NULL))
        return (1);
    return (0);
}

int ft_philo_init(t_rules *rules)
{
    int i;

    i = rules->num_philo;
    while (--i >= 0)
    {
        rules->philosopher[i].id = i;
        rules->philosopher[i].x_ate = 0;
        rules->philosopher[i].left_fork = i;
        rules->philosopher[i].right_fork = (i + 1) % rules->num_philo;
        rules->philosopher[i].time_end = 0;
        rules->philosopher[i].rules = rules;
    }
    return (0);
}

int ft_thread_create(t_rules *rules)
{
    t_philo *phi;
    int i;

    i = 0;
    phi = rules->philosopher;
    rules->time_start = ft_current_time();
    while (i < rules->num_philo)
    {
        if (pthread_create(&(phi[i].thr), NULL, mythread, &(phi[i])))
            return (1);
        usleep(10000);
        phi[i].time_end = ft_current_time();
        i++;
    }
    ft_check_death(rules, rules->philosopher);
    ft_thread_exit(rules, phi);
    return (0);
}

void    *mythread(void *philo)
{
    int i;
    t_rules *rules;
    t_philo *phi;

    i = 0;
    phi = (t_philo*)philo;
    rules = phi->rules;
    if (phi->id % 2)
        usleep(15000);
    while (!(rules->died))
    {
        ft_philo_eat(phi);
        if (rules->meal_count)
            break;
        ft_display_message(rules, phi->id, "is sleeping.");
        ft_sleep(rules, rules->time_sleep);
        ft_display_message(rules, phi->id, "is thinking.");
        i++;
    }
    return (NULL);
}

void    ft_philo_eat(t_philo *phi)
{
    t_rules *rules;

    rules = phi->rules;
    pthread_mutex_lock(&(rules->fork[phi->left_fork]));
    ft_display_message(rules, phi->id, "has taken a fork.");
    pthread_mutex_lock(&(rules->fork[phi->right_fork]));
    ft_display_message(rules, phi->id, "has taken a fork.");
    pthread_mutex_lock(&(rules->meal));
    ft_display_message(rules, phi->id, "is eating.");
    phi->time_end = ft_current_time();
    pthread_mutex_unlock(&(rules->meal));
    ft_sleep(rules, rules->time_eat);
    (phi->x_ate)++;
    pthread_mutex_unlock(&(rules->fork[phi->left_fork]));
    pthread_mutex_unlock(&(rules->fork[phi->right_fork]));
}

void    ft_check_death(t_rules *rules, t_philo *phi)
{
    int i;
    
    while (!(rules->meal_count))
    {
        i = -1;
        while (++i < rules->num_philo && !(rules->died))
        {
            pthread_mutex_lock(&(rules->meal));
            if (ft_current_time() - phi[i].time_end)
            {
                ft_display_message(rules, i, "died");
                rules->died = 1;
            }
            pthread_mutex_unlock(&(rules->meal));
            usleep(100);
        }
        if (rules->died)
            break ;
        i = 0;
        while (rules->num_eat != -1 && i < rules->num_philo
            && phi[i].x_ate >= rules->num_eat)
            i++;
        if (i == rules->num_philo)
            rules->meal_count = 1;
    }
}

void    ft_thread_exit(t_rules *rules, t_philo *philo)
{
    int i;

    i = -1;
    while (++i < rules->num_philo)
        pthread_join(philo[i].thr, NULL);
    i = -1;
    while (++i < rules->num_philo)
        pthread_mutex_destroy(&(rules->fork[i]));
    pthread_mutex_destroy(&(rules->message));
}

int main(int argc, char **argv)
{
    t_rules rules;
    t_philo phi;

    if (argc != 5 && argc != 6)
        return (ft_error_message("Invalid number of argument."));
    if (ft_rules_init(&rules, argv))
        return (ft_error_message("Invalid argument."));
    if (ft_philo_init(&rules))
        return (ft_error_message("philosopher initializing."));
    if (ft_mutex_init(&rules))
        return(ft_error_message("Mutex initializing."));       
    if (ft_thread_create(&rules))
        return (ft_error_message("creating thread."));
 
    return (0);
}