/*  file: ex4_q1_given.h
  ****  do not change or modify this file ****
*/

#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define RAND_LO         1      // low bound for random numbers
#define RAND_HI         100    // hi  bound for random numbers
#define SEED            1001   // constant seed, yield fixed series of random

#define N_PROD          3   // number of producer threads
#define N_CONS          4   // number of consumer threads
#define ITEM_START_CNT  8   // consumer threads start only after so items created
#define TOTAL_ITEMS     20  // total number of messages to create

// print constants
#define ALL_PROD_CREATED "main thread created all producer threads\n"
#define ALL_CONS_CREATED "main thread created all consumer threads\n"
#define PROD_TERMINATED  "all producers terminated\n"
#define CONS_TERMINATED  "all consumers terminated\n"

#define SLEEP_FACTOR     150  // used by my_sleep
#define REPORTER_FILE    "items.log"
#define REPORTER_MSG    "<ITEM REPORTER> "

// *** type declarations
typedef unsigned uint;
typedef enum STATUS STATUS;
typedef struct item item;
typedef struct list_node list_node;

enum STATUS {NOT_DONE = 0, PROCESSING, DONE};

struct item {
  uint prod;
  uint n1, n2;
  enum STATUS status;
};
struct list_node
{
  item*  item;
  list_node* next;
};

//  Extern for global variables
extern list_node* list_head;


// *********** function prototypes ***********
// function dealing with numbers
int get_random_in_range();
int is_prime(int n);
int set_two_factors(item*);

// function dealing with "items" and the list
void add_to_list (item*);
item* get_undone_from_list();

// function dealing with writing to stdout
void write_adding_item(int thread_num, item*);
void write_getting_item(int thread_num, item*);
void print_one_item(item* item);
void print_list();
void write_producer_is_done(int thread_num);
void write_consumer_is_done(int thread_num);

// function dealing with list --> sorting
void sort_list(list_node* list);



// other misc. functions
void my_sleep();

//for debugging in case you need
void debug_print_one_item(const char* file, const char* func, int line, item* item);
