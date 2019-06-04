#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

int num_passengers;
int num_trolleys;
int trolley_capacity;
int num_rides;
int trolleys_stoped=0;

int* queue;
int queue_size=0;

int current_trolley = 0;
int current_passenger = -1;
int passengers_inside = 0;
int current_leaver=-1;
int button_pressed =0;
int button_passenger=-1;

pthread_mutex_t mutex_current_trolley = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_passenger_entered = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_button_passenger = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_passenger_left = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_current_trolley = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_entered = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_button_passenger = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_left = PTHREAD_COND_INITIALIZER;

void push_queue(int passenger) {
    queue[queue_size++] = passenger;
}

int pop_queue() {
    if(queue_size == 0) return -1;
    int to_ret = queue[0];
    for(int i = 0; i < queue_size-1; ++i) {
        queue[i] = queue[i+1];
    }
    queue_size -= 1;
    return to_ret;
}


void print_time(const char* format, ...) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    int h = tv.tv_sec / 3600;
    int m = (tv.tv_sec - h * 3600) / 60;
    int s = tv.tv_sec - h * 3600 - m * 60;
    int ms = tv.tv_usec / 1000;

    printf("%02d:%02d:%02d:%03d: ", h % 24, m, s, ms);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}



void* trolley_worker(void *arg) {
    int id = *((int*)arg);

    int* passengers = malloc(trolley_capacity * sizeof(int));
    for(int i = 0; i < trolley_capacity; ++i) {
        passengers[i] = -1;
    }
    int trolley_full = 0;

    for(int ride = 0; ride <= num_rides; ++ride) {
        pthread_mutex_lock(&mutex_current_trolley);
        while(current_trolley != id) {
            pthread_cond_wait(&cond_current_trolley, &mutex_current_trolley);
        }

        current_passenger = -1;
        current_leaver = -1;
        button_pressed = 0;
        button_passenger = -1;
        passengers_inside=0;

        print_time("trolley: %d arrived\n", id);
        
        print_time("trolley: %d opened door\n", id);
        
        if(trolley_full == 1) {
            pthread_mutex_lock(&mutex_passenger_left);
            passengers_inside = trolley_capacity;
            for(int i = 0; i < trolley_capacity; ++i) {
                current_leaver = passengers[i];
                pthread_cond_broadcast(&cond_passenger_left);
                while(current_leaver != -1) {
                    pthread_cond_wait(&cond_passenger_left, &mutex_passenger_left);
                }
                passengers_inside--;
            }
            pthread_mutex_unlock(&mutex_passenger_left);
        }

        if(ride == num_rides) {
            print_time("trolley: %d stops\n", id);
            current_trolley = (current_trolley+1)%num_trolleys;
            trolleys_stoped++;
            pthread_cond_broadcast(&cond_passenger_entered);
            pthread_cond_broadcast(&cond_current_trolley);
            pthread_mutex_unlock(&mutex_current_trolley);
            break;
        }


        pthread_mutex_lock(&mutex_passenger_entered);
        for(int i = 0; i < trolley_capacity; ++i) {
            
            current_passenger = pop_queue();
            passengers[i] = current_passenger;
            passengers_inside ++;
            pthread_cond_broadcast(&cond_passenger_entered);
            while(current_passenger != -1) {
                pthread_cond_wait(&cond_passenger_entered, &mutex_passenger_entered);
            }
            
        }
        trolley_full=1;
        pthread_mutex_unlock(&mutex_passenger_entered);


        pthread_mutex_lock(&mutex_button_passenger);
        button_passenger = passengers[rand() % trolley_capacity];
        pthread_cond_broadcast(&cond_button_passenger);
        while (button_pressed != 1) {
            pthread_cond_wait(&cond_button_passenger, &mutex_button_passenger);
        }
        pthread_mutex_unlock(&mutex_button_passenger);

        print_time("trolley: %d closed doors\n", id);
        
        print_time("trolley: %d is leaving the platform\n", id);

        current_trolley = (current_trolley+1)%num_trolleys;

        pthread_cond_broadcast(&cond_current_trolley);
        pthread_mutex_unlock(&mutex_current_trolley);
        
        usleep((rand() % 10) * 1000);
        //sleep(5);
    }
    free(passengers);

    
    return 0;
}

void* passenger_worker(void *arg) {
    int id = *((int*) arg);


    while(num_trolleys > 0) {
        pthread_mutex_lock(&mutex_passenger_entered);
        while (current_passenger != id && trolleys_stoped < num_trolleys) {
            pthread_cond_wait(&cond_passenger_entered, &mutex_passenger_entered);
        }
        if(num_trolleys == trolleys_stoped) {
            print_time("passenger: %d walks away, no more trolleys\n", id);

            pthread_cond_broadcast(&cond_passenger_entered);
            pthread_mutex_unlock(&mutex_passenger_entered);
            break;
        }
        print_time("passenger: %d entered trolley: %d, places left: %d\n", id, current_trolley, trolley_capacity - passengers_inside);
        current_passenger = -1;
        pthread_cond_broadcast(&cond_passenger_entered);
        pthread_mutex_unlock(&mutex_passenger_entered);


        pthread_mutex_lock(&mutex_button_passenger);
        while (button_passenger == -1) {
            pthread_cond_wait(&cond_button_passenger, &mutex_button_passenger);
        }

        if(button_passenger == id) {
            print_time("passenger: %d pressed the button\n", id);
            button_pressed = 1;
            pthread_cond_broadcast(&cond_button_passenger);
        }

        pthread_mutex_unlock(&mutex_button_passenger);


        pthread_mutex_lock(&mutex_passenger_left);
        while (current_leaver != id ) {
            pthread_cond_wait(&cond_passenger_left, &mutex_passenger_left);
        }
       
        print_time("passenger: %d left trolley: %d, passengers inside: %d\n", id, current_trolley, passengers_inside-1);
        current_leaver = -1;
        pthread_cond_broadcast(&cond_passenger_left);
        push_queue(id);
        pthread_mutex_unlock(&mutex_passenger_left);

    }
 


    return 0;
}

int main(int argc, char *argv[]) {
    srand(time(0));
    if(argc != 5) {
        fprintf(stderr, "wrong arguments, usage: [num_passengers, num_trolleys, trolley_capacity, num_rides]\n");
        exit(1);
    }


    if( sscanf(argv[1], "%d", &num_passengers) == 0 ||
        sscanf(argv[2], "%d", &num_trolleys) == 0 ||
        sscanf(argv[3], "%d", &trolley_capacity) == 0 ||
        sscanf(argv[4], "%d", &num_rides) == 0) {

        fprintf(stderr, "cant parse arguments\n");
        exit(1); 
    }


    if(num_passengers < trolley_capacity*num_trolleys) {
        fprintf(stderr, "not enought people\n");
        exit(1); 
    }

    pthread_t* passengers_threads = malloc(num_passengers * sizeof(pthread_t));
    pthread_t* trolleys_threads = malloc(num_trolleys * sizeof(pthread_t));

    int *passengers = malloc(num_passengers * sizeof(int));
    int *trolleys = malloc(num_trolleys * sizeof(int));

    queue = malloc(num_passengers * sizeof(int));

    for(int i = 0; i < num_passengers; ++i) {
        passengers[i] = i;
        push_queue(i);
        if(pthread_create(&passengers_threads[i], NULL, passenger_worker, &passengers[i]) != 0) {
            fprintf(stderr, "cant create passengers threads\n");
            exit(1); 
        }
    }

    for(int i = 0; i < num_trolleys; ++i) {
        trolleys[i] = i;
        if(pthread_create(&trolleys_threads[i], NULL, trolley_worker, &trolleys[i]) != 0) {
            fprintf(stderr, "cant create trolleys threads\n");
            exit(1); 
        }
    }

    for(int i = 0; i < num_passengers; ++i) {
        if(pthread_join(passengers_threads[i], NULL) != 0 ) {
            fprintf(stderr, "cant join passengers threads\n");
            exit(1); 
        }
        print_time("passenger thread: %d stopped working\n", i);
    }

    for(int i = 0; i < num_trolleys; ++i) {
        if(pthread_join(trolleys_threads[i], NULL) != 0 ) {
            fprintf(stderr, "cant join trolleys threads\n");
            exit(1); 
        }
        print_time("trolley thread: %d stopped working\n", i);
    }


    free(passengers);
    free(trolleys);
    free(passengers_threads);
    free(trolleys_threads);
    free(queue);

    return 0;
}