#include "ex4_q1_given.h"

#define ITEM_REPORTER "item_reporter"
#define LIST_SORTER "list_sorter"
// SEMAPHORES
#define SEM_ACCESS 1
#define SEM_OCCUPY 0
#define SEM_FREE 20
#define SEM_CONS_WAIT 0
#define SEM_RAND 1
#define SEM_PRINT 1
#define SEM_COUNT_CREATED 20
#define SEM_ALL_THREADS_CREATED 0
//---------------------------------
// semaphores as global variables
sem_t* sem_access;
sem_t* sem_print;
sem_t* sem_occupy;
sem_t* sem_free;
sem_t* sem_cons_wait;
sem_t* sem_rand;
sem_t* sem_count_created;
sem_t* sem_all_threads_created;

void unlink_all_sem();
void createThreadsAndWait(pthread_t* consumers, pthread_t* producers);
int randNumber();
void* produce(void* thread_number);
void* consume(void* thread_number);
void finishProducer(int threadNumber);
void finishConsumer(int threadNumber);
void open_all_sem();
void startConsumersWork();

list_node* list_head;
list_node* list_tail;
int first_pp[2];   // pp[0] - read   pp[1] - write
int second_pp[2];   // pp[0] - read   pp[1] - write
// first_pp[0] = 3, first_pp[1] = 4, second_pp[0] = 5, second_pp[1] = 6
char* my_argv[5] = { "list_sorter", "item_reporter", NULL };

int main(int argc, char* argv[])
{
	int pid;
	pthread_t consumers[N_CONS];
	pthread_t producers[N_PROD];
	pipe(first_pp);
	pipe(second_pp);

	// create seamaphores
	open_all_sem();

	pid = fork();
	if (pid == 0)
	{
		close(first_pp[1]);
		close(second_pp[0]);
		execve(LIST_SORTER, my_argv, NULL);
	}

	close(first_pp[0]);
	close(second_pp[1]);

	pid = fork();
	if (pid == 0)
	{
		close(first_pp[1]);
		execve(ITEM_REPORTER, my_argv, NULL);
	}

	close(second_pp[0]);

	// create threads
	createThreadsAndWait(consumers, producers);
	unlink_all_sem();
}

void unlink_all_sem()
{
	char* semaphores[] = { "/sem_access", "/sem_print", "/sem_occupy","/sem_free", "/sem_cons_wait", "/sem_rand",
							"/sem_count_created", "/sem_all_threads_created" };

	for (int i = 0; i < 8; i++)
		sem_unlink(semaphores[i]);
}

void createThreadsAndWait(pthread_t* consumers, pthread_t* producers)
{
	int i = 0;
	for (i = 0; i < N_PROD; i++)
	{
		pthread_create(&producers[i], NULL, produce, (void*)(long)i + 1);
	}
	printf(ALL_PROD_CREATED);

	for (i = 0; i < N_CONS; i++)
	{
		pthread_create(&consumers[i], NULL, consume, (void*)(long)i + 1);
	}
	printf(ALL_CONS_CREATED);

	// update threads created to enable producers to start
	sem_post(sem_all_threads_created);
	sem_post(sem_all_threads_created);
	sem_post(sem_all_threads_created);

	for (i = 0; i < N_PROD; i++)
	{
		pthread_join(producers[i], NULL);
	}
	sem_wait(sem_print);
	printf(PROD_TERMINATED);
	sem_post(sem_print);

	for (i = 0; i < N_CONS; i++)
	{
		pthread_join(consumers[i], NULL);
	}

	printf(CONS_TERMINATED);
	close(first_pp[1]);
}

int randNumber()
{
	int randNum;
	do {
		sem_wait(sem_rand);
		randNum = get_random_in_range();
		sem_post(sem_rand);
	} while (!is_prime(randNum));
	return randNum;
}

void* produce(void* thread_number)
{
	int threadNumber = (long)(thread_number);
	int items_created, firstRand, secondRand;

	sem_wait(sem_all_threads_created);
	while (!sem_trywait(sem_count_created))
	{
		firstRand = randNumber();		// rand 2 numbers
		secondRand = randNumber();
		sem_wait(sem_access); // when accessing same data structur

		sem_getvalue(sem_count_created, &items_created);
		if (items_created == (TOTAL_ITEMS - ITEM_START_CNT - 1))
			startConsumersWork();

		// create new item
		item* newItem = (item*)malloc(sizeof(item));
		newItem->prod = firstRand * secondRand;
		newItem->status = (STATUS)NOT_DONE;

		// add the new item and print
		sem_wait(sem_print);
		write_adding_item(threadNumber, newItem);
		add_to_list(newItem);
		sem_post(sem_print);

		sem_post(sem_occupy);
		sem_post(sem_access);
	}

	finishProducer(threadNumber);
	pthread_exit(NULL);
}

void* consume(void* thread_number)
{
	int threadNumber = (long)(thread_number);

	sem_wait(sem_cons_wait);
	while (!sem_trywait(sem_free))
	{
		sem_wait(sem_occupy);
		sem_wait(sem_access);

		item* itemReceived = get_undone_from_list();
		sem_wait(sem_print);
		write_getting_item(threadNumber, itemReceived);
		set_two_factors(itemReceived);
		itemReceived->status = (STATUS)DONE;
		// send data to item reporter
		write(first_pp[1], itemReceived, sizeof(item));

		sem_post(sem_print);
		sem_post(sem_access);
	}

	finishConsumer(threadNumber);
	pthread_exit(NULL);
}

void finishProducer(int threadNumber)
{
	sem_wait(sem_print);
	write_producer_is_done(threadNumber);
	sem_post(sem_print);
}

void finishConsumer(int threadNumber)
{
	sem_wait(sem_print);
	write_consumer_is_done(threadNumber);
	sem_post(sem_print);
}

void startConsumersWork()
{
	sem_post(sem_cons_wait);
	sem_post(sem_cons_wait);
	sem_post(sem_cons_wait);
	sem_post(sem_cons_wait);
	sem_post(sem_cons_wait);
}

void open_all_sem()
{
	// init sem_access
	if (sem_unlink("/sem_access") == 0)
		fprintf(stderr, "successful unlink sem /sem_access\n");
	sem_access = sem_open("/sem_access", O_CREAT, S_IRWXU, SEM_ACCESS);
	if (sem_access == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_access\n");
		exit(EXIT_FAILURE);
	}

	// init sem_occupy
	if (sem_unlink("/sem_occupy") == 0)
		fprintf(stderr, "successful unlink sem /sem_occupy\n");
	sem_occupy = sem_open("/sem_occupy", O_CREAT, S_IRWXU, SEM_OCCUPY);
	if (sem_occupy == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_occupy\n");
		exit(EXIT_FAILURE);
	}

	// init sem rand
	if (sem_unlink("/sem_rand") == 0)
		fprintf(stderr, "successful unlink sem /sem_rand\n");
	sem_rand = sem_open("/sem_rand", O_CREAT, S_IRWXU, SEM_RAND);
	if (sem_rand == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_rand\n");
		exit(EXIT_FAILURE);
	}

	// init sem wait
	if (sem_unlink("/sem_cons_wait") == 0)
		fprintf(stderr, "successful unlink sem /sem_cons_wait\n");
	sem_cons_wait = sem_open("/sem_cons_wait", O_CREAT, S_IRWXU, SEM_CONS_WAIT);
	if (sem_cons_wait == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_cons_wait\n");
		exit(EXIT_FAILURE);
	}

	// init sem free
	if (sem_unlink("/sem_free") == 0)
		fprintf(stderr, "successful unlink sem /sem_free\n");
	sem_free = sem_open("/sem_free", O_CREAT, S_IRWXU, TOTAL_ITEMS);
	if (sem_free == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_free\n");
		exit(EXIT_FAILURE);
	}

	// init sem print
	if (sem_unlink("/sem_print") == 0)
		fprintf(stderr, "successful unlink sem /sem_print\n");
	sem_print = sem_open("/sem_print", O_CREAT, S_IRWXU, SEM_PRINT);
	if (sem_print == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_print\n");
		exit(EXIT_FAILURE);
	}

	// init sem prod threads created
	if (sem_unlink("/sem_count_created") == 0)
		fprintf(stderr, "successful unlink sem /sem_count_created\n");
	sem_count_created = sem_open("/sem_count_created", O_CREAT, S_IRWXU, SEM_COUNT_CREATED);
	if (sem_count_created == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_count_created\n");
		exit(EXIT_FAILURE);
	}

	// init sem all threads created
	if (sem_unlink("/sem_all_threads_created") == 0)
		fprintf(stderr, "successful unlink sem /sem_all_threads_created\n");
	sem_all_threads_created = sem_open("/sem_all_threads_created", O_CREAT, S_IRWXU, SEM_ALL_THREADS_CREATED);
	if (sem_all_threads_created == SEM_FAILED)
	{
		perror("faild to open semaphore /sem_all_threads_created\n");
		exit(EXIT_FAILURE);
	}
}