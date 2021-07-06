#include "ex4_q1_given.h"

int open_file(char* name);
list_node* list_head;
list_node* list_tail;

int main(int argc, char* argv[])
{
	close(1);
	open_file(REPORTER_FILE);
	item itemReceived;
	int read_bytes;

	while ((read_bytes = read(5, &itemReceived, sizeof(item))) > 0)
	{
		printf("%s", REPORTER_MSG);
		print_one_item(&itemReceived);
	}

	close(1);
	close(5);
}

int open_file(char* name)
{
	int fd = 0;
	fd = open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (fd < 0)  // open failed
	{
		fprintf(stderr, "ERROR: open \"%s\" failed (%d). Exiting\n", name, fd);
		exit(2);
	}

	return(fd);
}