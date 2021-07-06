#include "ex4_q1_given.h"

list_node* list_head;
list_node* list_tail;

int main(int argc, char* argv[])
{
	item* tempItemReceived = (item*)malloc(sizeof(item));

	while (read(3, tempItemReceived, sizeof(item)) > 0)
	{
		write(6, tempItemReceived, sizeof(item));
		add_to_list(tempItemReceived);
		tempItemReceived = (item*)malloc(sizeof(item));
	} 

	sort_list(list_head);
	print_list();

	close(3);
	close(6);
}
