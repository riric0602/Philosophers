/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_life.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchbouki <rchbouki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/08 19:40:47 by rchbouki          #+#    #+#             */
/*   Updated: 2023/08/14 18:01:24 by rchbouki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	ft_second_fork(t_data *data, int id)
{
	if (id == 1)
		return (data->number - 1);
	else
		return (id - 2);
}

static void	philo_eating(t_philo *philo, t_data *data, int id, int second)
{
	ft_printf(data, "has taken a fork", id);
	ft_printf(data, "is eating", id);
	ft_usleep(data->t_eat);
	pthread_mutex_lock(&(philo->time));
	philo->time_after_food = get_time();
	philo->times_eaten++;
	pthread_mutex_unlock(&(philo->time));
	food_utils(data, philo);
	pthread_mutex_unlock(&(data->forks[id - 1]));
	pthread_mutex_unlock(&(data->forks[second]));
}

/* This function will lock the forks and unlock them after the philo eats */
static int	philo_food(t_philo *philo, t_data *data, int id)
{
	int	second_fork;

	second_fork = ft_second_fork(data, id);
	pthread_mutex_lock(&(data->forks[id - 1]));
	if (check_death(data))
	{
		pthread_mutex_unlock(&(data->forks[id - 1]));
		return (0);
	}
	ft_printf(data, "has taken a fork", id);
	if (data->number == 1)
	{
		pthread_mutex_unlock(&(data->forks[id - 1]));
		return (0);
	}
	pthread_mutex_lock(&(data->forks[second_fork]));
	if (check_death(data))
	{
		pthread_mutex_unlock(&(data->forks[id - 1]));
		pthread_mutex_unlock(&(data->forks[second_fork]));
		return (0);
	}
	philo_eating(philo, data, id, second_fork);
	return (1);
}

/* The thread function that is the real gestion of the philo */
static void	*thread_function(void *args)
{
	int			id;
	t_philo		*philo;
	t_data		*data;

	philo = (t_philo *)args;
	data = philo->data;
	id = philo->id;
	if (id % 2 != 0)
		ft_usleep(50);
	pthread_mutex_lock(&(philo->time));
	philo->times_eaten = 0;
	pthread_mutex_unlock(&(philo->time));
	while (1)
	{
		if (data->max_meals == 0 || philo_food(philo, data, id) == 0)
			break ;
		if (check_death(data))
			return (0);
		ft_printf(data, "is sleeping", id);
		ft_usleep(data->t_sleep);
		if (check_death(data))
			break ;
		ft_printf(data, "is thinking", id);
	}
	return (NULL);
}

/* This function will create each thread and wait for them in pthread_join */
void	philo_creation(t_data *data)
{
	int		i;
	t_philo	*philo;

	i = 0;
	philo = malloc(sizeof(t_philo) * data->number);
	if (!philo)
		return ;
	data->tid = malloc(sizeof(pthread_t) * data->number);
	if (!data->tid)
		return ;
	data->philos = philo;
	data->start = get_time();
	pthread_mutex_init(&(data->write), NULL);
	ft_init_philo(data, philo);
	while (i < data->number)
	{
		philo[i].time_after_food = get_time();
		pthread_create(&(data->tid[i]), NULL, &thread_function, &(philo[i]));
		i++;
	}
	main_death(data, philo);
	i = 0;
	while (i < data->number)
		pthread_join(data->tid[i++], NULL);
}
