#include "p2_threads.h"
#include "utils.h"
#include <random>
extern pthread_cond_t cond;
extern pthread_mutex_t mutex;
extern Restroom restroom;
extern struct timeval t_global_start;
void *threadfunc(void *parm)
{
	int status;

	Person *p = (Person *)parm;

	while ((restroom.get_status() == MENPRESENT && p->get_gender() == 0) || (restroom.get_status() == WOMENPRESENT && p->get_gender() == 1))
	{
		pthread_cond_wait(&cond, &mutex);
	}

	/*
		printf(" [Thread] Start\n");

		printf(" [Thread] Locks\n");

		printf(" [Thread] Blocked\n");
		status = pthread_cond_wait(&cond, &mutex);

		printf(" [Thread] Starts again.\n"); */

	restroom.enter_restroom(*p);
	for (int i = 0; i < 3; i++)
	{
		printf(" [Thread] Complete thread after (%d) seconds\n", (3 - i));
		usleep(MSEC(1000));
	}

	restroom.leave_restroom(*p);

	if ((p->get_gender() == 0 && restroom.get_menPresent() == 0) || (p->get_gender() == 1 && restroom.get_womenPresent() == 0))
	{
		restroom.set_status(EMPTY);
		
	}

	pthread_mutex_unlock(&mutex);
	return NULL;

	/*
		printf(" [Thread] Unlocks\n");
		status = pthread_mutex_unlock(&mutex);
		printf(" [Thread] Complete\n"); */ 
}

void woman_wants_to_enter()
{
	pthread_mutex_lock(&mutex);
	Person woman;
	woman.set_gender(1);
	woman.set_order(restroom.get_next_order());
	restroom.add_person(woman);
	pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter()
{
	pthread_mutex_lock(&mutex);
	Person man;
	man.set_gender(0);
	man.set_order(restroom.get_next_order());
	restroom.add_person(man);
	pthread_mutex_unlock(&mutex);
}

void woman_leaves()
{
	pthread_mutex_unlock(&mutex);
	restroom.remove_person(1);
	pthread_mutex_unlock(&mutex);
}

void man_leaves()
{
	pthread_mutex_unlock(&mutex);
	restroom.remove_person(0);
	pthread_mutex_unlock(&mutex);
}

void *queue_thread(void *parm)
{
	while (true) {
        pthread_mutex_lock(&mutex);
        if (!restroom.empty()) {
            Person p = restroom.pop();  // assume restroom.pop() retrieves and removes the next person in queue
            int gender = p.get_gender();
            int stay_time = p.get_stay_time();

            printf("[%02d ms][Queue] Send (%s) into the restroom (Stay %d ms), Status: Total: %d (Men: %d, Women: %d)\n", 
                   get_current_time(), 
                   gender == 0 ? "Man" : "Woman",
                   stay_time,
                   restroom.get_total(),  // assuming get_total(), get_men(), get_women() are restroom's methods
                   restroom.get_men(), 
                   restroom.get_women());

            pthread_mutex_unlock(&mutex);
            usleep(MSEC(stay_time));
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}



void *restroom_thread(void *arg) {
    while (true) {
        pthread_mutex_lock(&mutex);
        if (!restroom.empty()) {
            Person p = restroom.front();  // assume restroom.front() retrieves the next person in queue but does not remove
            int gender = p.get_gender();

            printf("[%02d ms][Restroom] (%s) goes into the restroom, State is (%s): Total: %d (Men: %d, Women: %d)\n", 
                   get_current_time(), 
                   gender == 0 ? "Man" : "Woman",
                   restroom.get_status() == EMPTY ? "empty" : (gender == 0 ? "MenPresent" : "WomenPresent"),
                   restroom.get_total(),
                   restroom.get_men(),
                   restroom.get_women());

            pthread_mutex_unlock(&mutex);
            usleep(MSEC(p.get_stay_time()));

            pthread_mutex_lock(&mutex);
            if (gender == 0) {
                man_leaves();
            } else {
                woman_leaves();
            }

            printf("[%02d ms][Restroom] (%s) left the restroom. Status is changed, Status is (%s): Total: %d (Men: %d, Women: %d)\n", 
                   get_current_time(), 
                   gender == 0 ? "Man" : "Woman",
                   restroom.get_status() == EMPTY ? "empty" : (restroom.get_status() == MENPRESENT ? "MenPresent" : "WomenPresent"),
                   restroom.get_total(),
                   restroom.get_men(),
                   restroom.get_women());

            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

void *input_thread(void *arg)
{
    int *num_persons_ptr = (int *)arg;
    int num_persons = *num_persons_ptr;

    for (int i = 0; i < num_persons * 2; i++)
    {
        // Generate a random gender (0 for men, 1 for women)
        int gender = i % 2;

        if (gender == 0)
        {
            man_wants_to_enter();
        }
        else
        {
            woman_wants_to_enter();
        }

        struct timeval t_current;
        gettimeofday(&t_current, NULL);

        // Display that the person goes into the queue
        printf("[%02ld ms][Input] A person (%s) goes into the queue\n",
               get_elasped_time(t_global_start, t_current),
               gender == 0 ? "Man" : "Woman");

        // Wait for a random time interval (1 ms - 5 ms)
        usleep(MSEC((rand() % 5) + 1));
    }

    return NULL;
}
