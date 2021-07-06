
#include "ex4_q1_given.h"


//------------------------------------------------------------------------
int get_random_in_range()
{
  static int first = 1;
  // static int num = 1;
  // num++;
  if (first)
  {
    srand(SEED);
    first = 0;
  }
  int num = (rand() % (RAND_HI - RAND_LO + 1)) + RAND_LO;
  return num;
}

//------------------------------------------------------------------------
int is_prime(int n)
{
  if (n < 2)
    return 0;
  const int sqrt_n = sqrt(n);
  for (int j=2; j <= sqrt_n; j++)
    if (n%j == 0) return 0;
  my_sleep();
  return 1;
}

//------------------------------------------------------------------------
int set_two_factors(item* item)
{
  // brute force method
  // there are somewhat better algorithms, but that's not the point in this exercise
  int prod = item->prod;  // for readability - shortness
  if(prod < 4) 
    return 0;
  int limit = (int) sqrt(prod);
  int m = 2;
  while (m <= limit)
  {
    if ((prod % m ) == 0)
    {
      item->n1 = m;
      item->n2 = prod/m;
      return 0;    // success
    }
    m++;
  }
  // if we got here, it means we did not set_two_factors
  return 1;  // failure
}



//------------------------------------------------------------------------
// previous name: void write_msg_and_add_to_list
void write_adding_item(int thread_num, item* item)
{
  printf("producer #%d:  adding product %d\n", thread_num, item->prod);
  my_sleep();
  fflush(stdout);
}

//------------------------------------------------------------------------
// previous name: void write_msg_and_remove_from_buf
void write_getting_item(int thread_num, item* item)
{
  my_sleep(); 
  printf("consumer #%d just got item (status UNDONE): %d\n", thread_num,item->prod);
}

//------------------------------------------------------------------------
void add_to_list (item* item)
{
  list_node* new_node = malloc(sizeof(list_node));
  new_node->item = item;
  new_node->next = NULL;
  
  if (! list_head)
    list_head = new_node;
  else
  {
    list_node* temp = list_head;  my_sleep();
    list_head = new_node;    my_sleep();
    new_node->next = temp;
  }
}

//------------------------------------------------------------------------
item* get_undone_from_list()
{
  list_node* temp = list_head;
  if (!list_head)
    return NULL;
  while(temp)
  {
    if (temp->item->status == NOT_DONE) // found the needed item
    {
      temp->item->status = PROCESSING;
      return temp->item;
    }
    temp = temp->next;
  }
  return NULL;
}

//------------------------------------------------------------------------
void print_one_item(item* item)
{
  char status[20];
  switch (item->status)
  {
    case NOT_DONE:    sprintf(status, "NOT DONE"); break;
    case PROCESSING:  sprintf(status, "PROCESSING"); break;
    case DONE:        sprintf(status, "DONE"); break;
  }
  printf("product: %6d = %5d X %5d  status: = %s\n",
          item->prod, item-> n1, item->n2, status);
  fflush(stdout);
}

//------------------------------------------------------------------------
void print_list()
{
  list_node* run = list_head;
  while(run)
  {
    print_one_item(run->item);
    run = run->next;
  }
}
//------------------------------------------------------------------------
void write_product(char* what, item* item)
{
  printf("%s ", what); fflush(stdout);
  // my_sleep();
  printf("product: "); fflush(stdout);
  my_sleep();
  if (item->n2 < item->n1)
  {
    int temp = item->n1;
    item->n1 = item->n2;
    item->n2 = temp;
  }
  printf("%d = %d x %d\n", item->prod, item->n1, item->n2); fflush(stdout);
  my_sleep();  
}


//------------------------------------------------------------------------
void write_is_done(char* who, int thread_num)
{
  printf("%s #%d ", who, thread_num);
  fflush(stdout);
  my_sleep();
  printf("is done\n");
  fflush(stdout);
}

//------------------------------------------------------------------------
void write_producer_is_done(int thread_num)
{
  write_is_done("producer", thread_num);
}

//------------------------------------------------------------------------
void write_consumer_is_done(int thread_num)
{
  write_is_done("consumer", thread_num);
}

//------------------------------------------------------------------------
// function dealing with list --> sorting
void sort_list(list_node* list)
{
  int swapped = 1;
  while(swapped)
  {
    swapped = 0;
    list_node* node_ptr = list;
    while (node_ptr && node_ptr->next)
    {
      if (node_ptr->item->prod > node_ptr->next->item->prod)
      {
        swapped = 1;
        item* temp = node_ptr->item;
        node_ptr->item = node_ptr->next->item;
        node_ptr->next->item = temp;
      }
      node_ptr = node_ptr->next;
    }
  }
}






//------------------------------------------------------------------------
void my_sleep()
{
  int p = (int)pthread_self();
  p = p % 30 + 150;
  const struct timespec ts = {0, 1000 * p * SLEEP_FACTOR};
  nanosleep(&ts, NULL);
}


//======================   ***** DEBUG ****   ===============
void debug_print_one_item(const char* file, const char* func, int line, item* item)
{
  char status[20];
  switch (item->status)
  {
    case NOT_DONE:    sprintf(status, "NOT DONE"); break;
    case PROCESSING:  sprintf(status, "PROCESSING"); break;
    case DONE:        sprintf(status, "DONE"); break;
  }
  fprintf(stderr, "FIle=%s func=%s Line=%d product: %6d = %5d X %5d  status: = %s\n",
          file, func, line, item->prod, item-> n1, item->n2, status);
  fflush(stderr);
}


