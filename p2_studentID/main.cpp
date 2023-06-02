#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timeval t_global_start;
Restroom restroom;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		// Print error message and usage example
		printf("[ERROR] Expected 1 argument, but got %d\n", argc - 1);
		printf("[USAGE] %s <number>\n", argv[0]);
		return 1; // Exit the program with an error status
	}

	// This is to set the global start time
	gettimeofday(&t_global_start, NULL);
	srand(time(NULL));

	int *num_persons_ptr = static_cast<int *>(malloc(sizeof(int)));
	*num_persons_ptr = atoi(argv[1]);

	pthread_t tid_input, tid_queue;

	if (pthread_create(&tid_input, NULL, input_thread, num_persons_ptr) != 0)
	{
		fprintf(stderr, "Error creating input thread\n");
		return 1;
	}
	if (pthread_create(&tid_queue, NULL, queue_thread, NULL))
	{
		fprintf(stderr, "Error creating queue thread\n");
	}

	if (pthread_join(tid_input, NULL) != 0)
	{
		fprintf(stderr, "Error joining input thread\n");
		return 1;
	}
	if (pthread_join(tid_queue, NULL))
	{
		fprintf(stderr, "Error joining queue thread\n");
	}

	free(num_persons_ptr);

	return 0;

	/* 	if (pthread_create(&tid_queue, NULL, queue_thread, NULL))
		{
			fprintf(stderr, "Error creating queue thread\n");
		}

		if (pthread_create(&tid_restroom, NULL, restroom_thread, NULL))
		{
			fprintf(stderr, "Error creating restroom thread\n");
		}

		if (pthread_join(tid_queue, NULL))
		{
			fprintf(stderr, "Error joining queue thread\n");
		}

		if (pthread_join(tid_restroom, NULL))
		{
			fprintf(stderr, "Error joining restroom thread\n");
		}
		return 0; */

	/* 	int num_prople = atoi(argv[1]);

		// Example code for sleep and class usage
		Person p1;
		p1.set_order(1);

		usleep(MSEC(200));
		p1.start();


		usleep(MSEC(150));
		p1.complete();
		///////////////////////////////////////////


		if(pthread_create(&tid, NULL, threadfunc, NULL)) {
			fprintf(stderr, "Error creating thread\n");
		}
		usleep(MSEC(10));

		for (int i=0; i<5; i++) {
			printf("Wake up thread after (%d) seconds\n", (5-i));
			usleep(MSEC(1000));
		}

		printf("Wake up thread\n");
		status = pthread_cond_signal(&cond);
		 */

	/* wait for the second thread to finish */
	/* 	if(pthread_join(tid, NULL)) {
			fprintf(stderr, "Error joining thread\n");
		}

		return 0;
	 */
}
