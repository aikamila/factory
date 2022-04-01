#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define min(a, b) ((a < b) ? (a) : (b))

struct machine
{
    int id;
    int capacity;
    int percent_of_errors;
    int year_of_production;
    struct machine *next;
};

void add_all_machines(struct machine **all_machines_list, FILE *src_file)
{
    int _id;
    int _capacity;
    int _percent_of_errors;
    int _year_of_production;
    while (fscanf(src_file, "%d %d %d %d", &_id, &_capacity, &_percent_of_errors, &_year_of_production) != EOF)
    {
        struct machine *cur = (struct machine *)malloc(sizeof(struct machine));
        cur->id = _id;
        cur->capacity = _capacity;
        cur->percent_of_errors = _percent_of_errors;
        cur->year_of_production = _year_of_production;
        cur->next = *all_machines_list;
        (*all_machines_list) = cur;
    }
}

int count_machines(struct machine *list)
{
    int counter = 0;
    while (list != NULL)
    {
        counter++;
        list = list->next;
    }
    return counter;
}

void add_machine_to_a_list(struct machine **list, struct machine *machine_to_add)
{
    machine_to_add->next = *list;
    (*list) = machine_to_add;
}

void repair(struct machine **machines_under_repair_list, struct machine **working_machines_list, int free_places, int max_decrease)
{
    free_places = min(free_places, count_machines(*working_machines_list));
    int machines_already_added = 0;
    for (int i = 0; i < free_places; i++)
    {
        int id_of_the_best_choice = -1;
        int year_of_the_best_choice = 0;
        int max_error_percent = INT_MIN;
        struct machine *working_mashines_list_beginning = (*working_machines_list);
        while (working_mashines_list_beginning != NULL)
        {
            if (working_mashines_list_beginning->capacity <= max_decrease)
            {
                int faulty_items = (working_mashines_list_beginning->capacity) * (working_mashines_list_beginning->percent_of_errors);
                if (faulty_items > max_error_percent)
                {
                    max_error_percent = faulty_items;
                    year_of_the_best_choice = working_mashines_list_beginning->year_of_production;
                    id_of_the_best_choice = working_mashines_list_beginning->id;
                }
                if (faulty_items == max_error_percent && working_mashines_list_beginning->year_of_production < year_of_the_best_choice)
                {
                    year_of_the_best_choice = working_mashines_list_beginning->year_of_production;
                    id_of_the_best_choice = working_mashines_list_beginning->id;
                }
            }
            working_mashines_list_beginning = working_mashines_list_beginning->next;
        }
        if (id_of_the_best_choice == -1)
        {
            break;
        }
        else
        {
            working_mashines_list_beginning = (*working_machines_list);
            if (working_mashines_list_beginning->id == id_of_the_best_choice)
            {
                (*working_machines_list) = working_mashines_list_beginning->next;
                add_machine_to_a_list(machines_under_repair_list, working_mashines_list_beginning);
            }
            else
            {
                struct machine *prev;
                while (working_mashines_list_beginning->id != id_of_the_best_choice)
                {
                    prev = working_mashines_list_beginning;
                    working_mashines_list_beginning = working_mashines_list_beginning->next;
                }
                prev->next = working_mashines_list_beginning->next;
                add_machine_to_a_list(machines_under_repair_list, working_mashines_list_beginning);
            }
            machines_already_added++;
            max_decrease -= working_mashines_list_beginning->capacity;
        }
    }
    printf("Started to repair %d machine(s)\n", machines_already_added);
}

void list_machines(struct machine *machines)
{
    if (machines == NULL)
    {
        printf("The list is empty\n");
        return;
    }
    printf("|--------------------------------------------------------|\n");
    printf("| id | capacity | percent_of_errors | year_of_production |\n");
    printf("|--------------------------------------------------------|\n");
    do
    {
        printf("| %2.d |", machines->id);
        printf("%9.d |", machines->capacity);
        printf("%18.d |", machines->percent_of_errors);
        printf("%19.d |\n", machines->year_of_production);
        machines = machines->next;
    } while (machines != NULL);
    printf("|--------------------------------------------------------|\n");
}

int main()
{
    struct machine *all_machines_list;
    all_machines_list = NULL;
    struct machine *machines_under_repair_list;
    machines_under_repair_list = NULL;
    char filename[40];
    int choice;
    int free_places;
    int max_decrease;
    printf("Type the full name of a file in which data about machines is stored: ");
    scanf("%s", filename);
    FILE *src_file = fopen(filename, "rt");
    if (src_file == NULL)
    {
        printf("Error while opening the file");
        return 1;
    }
    add_all_machines(&all_machines_list, src_file);
    do
    {
        printf("What do you wanna do?\n");
        printf("1 - Print a list of all working mashines\n");
        printf("2 - Repair some machines\n");
        printf("3 - Print a list of all machines that are under repair\n");
        printf("Other choices end the program.\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            list_machines(all_machines_list);
            system("pause");
            break;
        case 2:
            do
            {
                printf("How many places for reparation are available? ");
                scanf("%d", &free_places);
            } while (free_places < 0);
            do
            {
                printf("\nWhat is the maximum decrease of production? ");
                scanf("%d", &max_decrease);
            } while (max_decrease < 0);
            repair(&machines_under_repair_list,  &all_machines_list, free_places, max_decrease);
            system("pause");
            break;
        case 3:
            list_machines(machines_under_repair_list);
            system("pause");
            break;
        default:
            break;
        }
    } while (choice == 1 || choice == 2 || choice == 3);
    while (all_machines_list != NULL)
    {
        struct machine *tmp = all_machines_list;
        all_machines_list = all_machines_list->next;
        free(tmp);
    }
    while (machines_under_repair_list != NULL)
    {
        struct machine *tmp = machines_under_repair_list;
        machines_under_repair_list = machines_under_repair_list->next;
        free(tmp);
    }
    fclose(src_file);
    return 0;
}