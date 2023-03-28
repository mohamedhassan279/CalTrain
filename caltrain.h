#include <pthread.h>

struct station {
    pthread_mutex_t mutex;
    pthread_cond_t train_loading;
    pthread_cond_t train_ready_to_go;
    int waiting_passengers;
    int available_seats;
    int enough_seats;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);