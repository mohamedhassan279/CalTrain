#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "caltrain.h"



void
station_init(struct station *station)
{
    if(pthread_mutex_init(&(station->mutex), NULL) != 0) {
        perror("error in pthread_mutex_init()");
        exit(1);
    }
    if (pthread_cond_init(&(station->train_loading), NULL) != 0) {
        perror("error in pthread_cond_init()");
        exit(1);
    }
    if (pthread_cond_init(&(station->train_ready_to_go), NULL) != 0) {
        perror("error in pthread_cond_init()");
        exit(1);
    }
    station->waiting_passengers = 0;
    station->available_seats = 0;
    station->enough_seats = 0;
}

void
station_load_train(struct station *station, int count)
{
    station->available_seats = count;

    pthread_mutex_lock(&(station->mutex));
    station->enough_seats = count;

    /*
     *  the train the station promptly leaves
     *  if no passengers are waiting at the station or it has no available free seats.
     */
    if (station->waiting_passengers != 0 && station->available_seats != 0) {
        // notify all waiting passengers that the train is in station
        pthread_cond_broadcast(&(station->train_loading));
        // wait until the train is full or there are no waiting passengers
        pthread_cond_wait(&(station->train_ready_to_go), &(station->mutex));
    }
    pthread_mutex_unlock(&(station->mutex));
}

void
station_wait_for_train(struct station *station)
{
    pthread_mutex_lock(&(station->mutex));

    station->waiting_passengers++;
    do {
        // wait until a train arrives at the station
        pthread_cond_wait(&(station->train_loading), &(station->mutex));
        station->enough_seats--;
    }
    while (station->enough_seats < 0); // wait for another train as there are no enough seats
    pthread_mutex_unlock(&(station->mutex));
}

void
station_on_board(struct station *station)
{
    pthread_mutex_lock(&(station->mutex));
    if(station->waiting_passengers != 0 && station->available_seats != 0){
        station->waiting_passengers--;
        station->available_seats--;
    }
    pthread_mutex_unlock(&(station->mutex));

    /*
     * notify the train to leave when no passengers are waiting or
     * there are no enough seats
     */
    if(station->waiting_passengers == 0 || station->available_seats == 0){
        pthread_cond_signal(&(station->train_ready_to_go));
    }
}
