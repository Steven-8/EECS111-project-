#include "p2_threads.h"
#include "utils.h"
#include <queue>
extern pthread_cond_t cond;
extern pthread_mutex_t mutex;
extern Restroom restroom;
extern struct timeval t_global_start;
extern std::queue<Person> man_queue;
extern std::queue<Person> woman_queue;
bool isAddingDone = false;
void *handle_restroom_entry(void *arg);
/* void *threadfunc(void *parm)
{
    int status;

    Person *p = (Person *)parm;

    while ((restroom.get_status() == MENPRESENT && p->get_gender() == 0) || (restroom.get_status() == WOMENPRESENT && p->get_gender() == 1))
    {
        pthread_cond_wait(&cond, &mutex);
    }

    printf(" [Thread] Start\n");

    printf(" [Thread] Locks\n");

    printf(" [Thread] Blocked\n");
    status = pthread_cond_wait(&cond, &mutex);

    printf(" [Thread] Starts again.\n");

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

    printf(" [Thread] Unlocks\n");
    status = pthread_mutex_unlock(&mutex);
    printf(" [Thread] Complete\n");
} */

void woman_enter_queue()
{
    Person woman;
    woman.set_gender(1);
    woman.set_order(restroom.get_next_order());
    woman.set_time((rand() % 8) + 3);
    woman_queue.push(woman);

    struct timeval t_current;
    gettimeofday(&t_current, NULL);
    printf("[%02ld ms][Input] A person (%s) goes into the queue\n",
           get_elasped_time(t_global_start, t_current),
           "Woman");
}

void man_enter_queue()
{
    Person man;
    man.set_gender(0);
    man.set_order(restroom.get_next_order());
    man.set_time((rand() % 8) + 3);
    man_queue.push(man);
    struct timeval t_current;
    gettimeofday(&t_current, NULL);
    printf("[%02ld ms][Input] A person (%s) goes into the queue\n",
           get_elasped_time(t_global_start, t_current),
           "man");
}

void woman_wants_to_enter()
{
    pthread_mutex_lock(&mutex);
    if (!woman_queue.empty())
    {
        Person p = woman_queue.front(); // Get the person at the front of the queue
        woman_queue.pop();              // Remove the person from the queue
        int stay_time = p.get_time();   // Get the stay time of the person
        struct timeval t_current;
        gettimeofday(&t_current, NULL);
        printf("[%02ld ms][Queue] Send (Woman) into the restroom (Stay %d ms), Status: Total: %ld (Men: %ld, Women: %ld)\n",
               get_elasped_time(t_global_start, t_current),
               stay_time,
               man_queue.size() + woman_queue.size(), // Total people in queue
               man_queue.size(),                      // Men in queue
               woman_queue.size());                   // Women in queue

        restroom.enter_restroom(p);
        printf("[%02ld ms][Restroom] (Woman) goes into the restroom, State is (MenPresent): Total: %d (Men: %d, Women: %d)\n",
               get_elasped_time(t_global_start, t_current),
               restroom.get_menPresent() + restroom.get_womenPresent(),
               restroom.get_menPresent(),
               restroom.get_womenPresent());

        Person *p_copy = new Person(p);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_restroom_entry, (void *)p_copy);
    }
    pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter()
{
    pthread_mutex_lock(&mutex);
    if (!man_queue.empty())
    {
        Person p = man_queue.front(); // Get the person at the front of the queue
        man_queue.pop();              // Remove the person from the queue
        int stay_time = p.get_time(); // Get the stay time of the person
        struct timeval t_current;
        gettimeofday(&t_current, NULL);
        printf("[%02ld ms][Queue] Send (Man) into the restroom (Stay %d ms), Status: Total: %ld (Men: %ld, Women: %ld)\n",
               get_elasped_time(t_global_start, t_current),
               stay_time,
               man_queue.size() + woman_queue.size(), // Total people in queue
               man_queue.size(),                      // Men in queue
               woman_queue.size());                   // Women in queue

        restroom.enter_restroom(p);
        printf("[%02ld ms][Restroom] (Man) goes into the restroom, State is (MenPresent): Total: %d (Men: %d, Women: %d)\n",
               get_elasped_time(t_global_start, t_current),
               restroom.get_menPresent() + restroom.get_womenPresent(),
               restroom.get_menPresent(),
               restroom.get_womenPresent());

        Person *p_copy = new Person(p);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_restroom_entry, (void *)p_copy);
    }
    pthread_mutex_unlock(&mutex);
}

void woman_leaves(Person p)
{
    // lock the mutex to ensure thread safety
    pthread_mutex_lock(&mutex);

    restroom.leave_restroom(p);

    struct timeval t_current;
    gettimeofday(&t_current, NULL);

    // print out the current state of the restroom
    printf("[%02ld ms][Restroom] (Woman) left the restroom. Status is changed, Status is (%s): Total: %d (Men: %d, Women: %d)\n",
           get_elasped_time(t_global_start, t_current),
           (restroom.get_status() == EMPTY) ? "empty" : "WomenPresent",
           restroom.get_menPresent() + restroom.get_womenPresent(), restroom.get_menPresent(), restroom.get_womenPresent());

    // unlock the mutex
    pthread_mutex_unlock(&mutex);
}

void man_leaves(Person p)
{
    // lock the mutex to ensure thread safety
    pthread_mutex_lock(&mutex);

    // decrease the count of men in the restroom
    restroom.leave_restroom(p);

    struct timeval t_current;
    gettimeofday(&t_current, NULL);

    // print out the current state of the restroom
    printf("[%02ld ms][Restroom] (Man) left the restroom. Status is changed, Status is (%s): Total: %d (Men: %d, Women: %d)\n",
           get_elasped_time(t_global_start, t_current),
           (restroom.get_status() == EMPTY) ? "empty" : "MenPresent",
           restroom.get_menPresent() + restroom.get_womenPresent(), restroom.get_menPresent(), restroom.get_womenPresent());

    // unlock the mutex
    pthread_mutex_unlock(&mutex);
}

void *handle_restroom_entry(void *arg)
{
    Person *p = (Person *)arg;
    usleep(MSEC(p->get_time())); // the person uses the restroom for their stay time
    if (p->get_gender() == 0)
    {
        man_leaves(*p);
    }
    else
    {
        woman_leaves(*p);
    }
    delete p; // we're done with this person, so delete the dynamically allocated memory
    return NULL;
}

void *queue_thread(void *parm)
{

    while (true)
    {

        // pthread_mutex_lock(&mutex);
        if (man_queue.size() + woman_queue.size() == 0 &&
            restroom.get_menPresent() == 0 &&
            restroom.get_womenPresent() == 0 &&
            isAddingDone == true)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }

        if (restroom.get_status() == EMPTY)
        {
            // Both queues are not empty
            if (!man_queue.empty() && !woman_queue.empty())
            {
                if (man_queue.front().get_order() < woman_queue.front().get_order())
                {
                    Person p = man_queue.front();

                    man_wants_to_enter();
                }
                else
                {
                    woman_wants_to_enter();
                }
            }
            // Only man queue is not empty
            else if (!man_queue.empty())
            {
                man_wants_to_enter();
            }
            // Only woman queue is not empty
            else if (!woman_queue.empty())
            {
                woman_wants_to_enter();
            }
        }
        else if (restroom.get_status() == MENPRESENT)
        {
            if (!man_queue.empty() && (woman_queue.empty() || man_queue.front().get_order() < woman_queue.front().get_order()))
            {
                man_wants_to_enter();
            }
        }
        else if (restroom.get_status() == WOMENPRESENT)
        {
            if (!woman_queue.empty() && (man_queue.empty() || woman_queue.front().get_order() < man_queue.front().get_order()))
            {
                woman_wants_to_enter();
            }
        }
    }

    return NULL;
}

/* void *restroom_thread(void *arg) {
    while (true) {
        pthread_mutex_lock(&mutex);
        if (restroom.get_status() != EMPTY) {
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
} */

void *input_thread(void *arg)
{
    int *num_persons_ptr = (int *)arg;
    int num_persons = *num_persons_ptr;
    srand(time(0));
    for (int i = 0; i < num_persons * 2; i++)
    {
        // Generate a random gender (0 for men, 1 for women)
        int gender = rand() % 2;

        if (gender == 0)
        {
            man_enter_queue();
        }
        else
        {
            woman_enter_queue();
        }

        // Display that the person goes into the queue

        // Wait for a random time interval (1 ms - 5 ms)
        usleep(MSEC((rand() % 5) + 1));
    }
    isAddingDone = true;
    return NULL;
}
