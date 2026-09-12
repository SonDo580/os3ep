#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// global ticket count
int gtickets = 0;

struct node_t
{
    int tickets; // tickets assigned to this process
    struct node_t *next;
};

struct node_t *head = NULL;

void insert(int tickets)
{
    // insert to head of linked-list
    struct node_t *tmp = malloc(sizeof(struct node_t));
    assert(tmp != NULL);
    tmp->tickets = tickets;
    tmp->next = head;
    head = tmp;
    gtickets += tickets;
}

void print_list()
{
    struct node_t *curr = head;
    printf("List: ");
    while (curr)
    {
        printf("[%d] ", curr->tickets);
        curr = curr->next;
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: lottery <seed> <loops>\n");
        exit(1);
    }

    int seed = atoi(argv[1]);
    int loops = atoi(argv[1]);
    srand(seed);

    // insert some jobs (with tickets)
    insert(50);
    insert(100);
    insert(25);

    print_list();

    for (int i = 0; i < loops; i++)
    {
        // pick winning ticket
        int winner = rand() % gtickets;

        // find winning process
        int count = 0;
        struct node_t *curr = head;
        while (curr)
        {
            count += count + curr->tickets;
            if (count > winner)
                break; // found
            curr = curr->next;
        }

        // 'curr' is the winner -> schedule it
        print_list();
        printf("winner: %d %d\n\n", winner, curr->tickets);
    }

    return 0;
}
