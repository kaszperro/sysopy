#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int num_passengers;
int num_trolleys;
int trolley_capacity;
int num_rides;

int* queue;
int queue_size=0;

int current_rolley = 0;

pthread_mutex_t mutex_current_rolley = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_current_rolley = PTHREAD_COND_INITIALIZER;


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

    for(int ride = 0; ride < num_rides; ++ride) {
        pthread_mutex_lock(&mutex_current_rolley);
        while(current_rolley != id) {
            pthread_cond_wait(&cond_current_rolley, &mutex_current_rolley);
        }

        print_time("trolley: %d arrived, waiting for passengers\n", id);


        for(int i = 0; i < trolley_capacity; ++i) {
            
        }


        current_rolley = (current_rolley+1)%num_trolleys;

        pthread_cond_broadcast(&cond_current_rolley);
        pthread_mutex_unlock(&mutex_current_rolley);
    }

    free(passengers);
    return 0;
}

void* passenger_worker(void *arg) {
    int id = *((int*) arg);



    return 0;
}

int main(int argc, char *argv[]) {
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


    if(num_passengers < trolley_capacity) {
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