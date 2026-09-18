/* ============================================================
 * Program Name: assignment2.c
 *
 * Description:
 * This program prioritizes a CEO's unread emails using a
 * list-based MaxHeap implemented from scratch.
 *
 * Inputs:
 * Commands are read from standard input:
 * EMAIL <sender category>,<subject line>,<date MM-DD-YYYY>
 * NEXT
 * READ
 * COUNT
 *
 * Outputs:
 * The program displays the number of unread emails, the next
 * email to be read, or removes an email when READ is entered.
 *
 * Priority:
 * Boss > Subordinate > Peer > ImportantPerson > OtherPerson
 *
 * If two emails have the same sender category, the newest
 * email is given higher priority.
 *
 * Collaborators:
 * None.
 *
 * Other Sources:
 * Claude and ChatGPT.
 *
 * Author:
 * Nivedita Rekha
 *
 * Creation Date:
 * September 17, 2026
 *
 * Revision Date:
 * September 17, 2026
 *
 * Revisions:
 * - Changed the file name to assignment2.c.
 * - Modified COUNT output to match the assignment requirements.
 * - Modified NEXT output to match the assignment requirements.
 * - Added handling for empty queues.
 * - Improved heapPush ordering.
 * - Added detailed comments throughout the program.
 *
 * ============================================================ */


/* The following include provides printf, fprintf, and fgets.
 * Source: Claude-generated code, revised by the author. */
#include <stdio.h>

/* The following include provides malloc, realloc, free,
 * and exit for dynamic memory management.
 * Source: Claude-generated code, revised by the author. */
#include <stdlib.h>

/* The following include provides string functions such as
 * strcmp, strncpy, strlen, strtok, and memmove.
 * Source: Claude-generated code, revised by the author. */
#include <string.h>

/* The following include provides isspace for checking
 * whitespace characters.
 * Source: Claude-generated code, revised by the author. */
#include <ctype.h>


/* The maximum number of characters allowed for a sender category. */
#define MAX_CATEGORY_LEN 20

/* The maximum number of characters allowed for a subject line. */
#define MAX_SUBJECT_LEN 256

/* The maximum length of a date is 10 characters plus '\0'. */
#define MAX_DATE_LEN 11

/* The maximum number of characters allowed in one input line. */
#define MAX_LINE_LEN 512

/* The initial number of emails that can be stored in the heap. */
#define INITIAL_CAPACITY 16


/* Define the Email structure. */
typedef struct {

    /* Store the sender category. */
    char category[MAX_CATEGORY_LEN];

    /* Store the email subject. */
    char subject[MAX_SUBJECT_LEN];

    /* Store the email date in MM-DD-YYYY format. */
    char date[MAX_DATE_LEN];

    /* Store the numerical priority of the sender category. */
    int priority;

    /* Store the date as YYYYMMDD for comparison. */
    long dateValue;

} Email;


/* Define the MaxHeap structure. */
typedef struct {

    /* Dynamic array that stores the emails. */
    Email *data;

    /* Number of emails currently stored. */
    int size;

    /* Total number of email spaces currently allocated. */
    int capacity;

} MaxHeap;


/* Initialize an empty MaxHeap. */
void heapInit(MaxHeap *h, int initialCapacity)
{
    /* Allocate memory for the initial number of emails. */
    h->data = (Email *)malloc(sizeof(Email) * initialCapacity);

    /* Check whether the memory allocation was successful. */
    if (h->data == NULL)
    {
        /* Display an error message if allocation failed. */
        fprintf(stderr, "Fatal: could not allocate heap memory.\n");

        /* End the program because the heap cannot be created. */
        exit(EXIT_FAILURE);
    }

    /* Start the heap with zero emails. */
    h->size = 0;

    /* Store the initial capacity of the heap. */
    h->capacity = initialCapacity;
}


/* Free all dynamically allocated heap memory. */
void heapFree(MaxHeap *h)
{
    /* Release the memory used by the email array. */
    free(h->data);

    /* Set the pointer to NULL after freeing it. */
    h->data = NULL;

    /* Reset the number of emails to zero. */
    h->size = 0;

    /* Reset the capacity to zero. */
    h->capacity = 0;
}


/* Increase the size of the heap's dynamic array. */
static void heapGrow(MaxHeap *h)
{
    /* Double the current capacity. */
    int newCapacity = h->capacity * 2;

    /* Reallocate memory using the new capacity. */
    Email *newData =
        (Email *)realloc(h->data,
                         sizeof(Email) * newCapacity);

    /* Check whether realloc was successful. */
    if (newData == NULL)
    {
        /* Display an error if the heap could not grow. */
        fprintf(stderr, "Fatal: could not grow heap memory.\n");

        /* End the program because more memory is required. */
        exit(EXIT_FAILURE);
    }

    /* Store the new memory address. */
    h->data = newData;

    /* Store the new capacity. */
    h->capacity = newCapacity;
}


/* Return 1 when email a has higher priority than email b. */
static int isHigherPriority(const Email *a, const Email *b)
{
    /* Check whether the sender priorities are different. */
    if (a->priority != b->priority)
    {
        /* The larger priority number has higher priority. */
        return a->priority > b->priority;
    }

    /* If priorities are tied, the newer date has higher priority. */
    return a->dateValue > b->dateValue;
}

/* Swap two Email structures. */
static void swapEmails(Email *a, Email *b)
{
    /* Temporarily store the first email. */
    Email temp = *a;

    /* Move the second email into the first position. */
    *a = *b;

    /* Move the original first email into the second position. */
    *b = temp;
}

/* Restore the MaxHeap property after inserting an email. */
static void siftUp(MaxHeap *h, int index)
{
    /* Continue while the current email has a parent. */
    while (index > 0)
    {
        /* Calculate the parent index. */
        int parent = (index - 1) / 2;

        /* Check whether the current email outranks its parent. */
        if (isHigherPriority(&h->data[index],
                             &h->data[parent]))
        {
            /* Swap the current email with its parent. */
            swapEmails(&h->data[index],
                       &h->data[parent]);

            /* Continue checking from the parent's position. */
            index = parent;
        }
        else
        {
            /* Stop because the heap property is satisfied. */
            break;
        }
    }
}

/* Restore the MaxHeap property after removing an email. */
static void siftDown(MaxHeap *h, int index)
{
    /* Continue until the email reaches its correct position. */
    while (1)
    {
        /* Calculate the left child's position. */
        int left = 2 * index + 1;

        /* Calculate the right child's position. */
        int right = 2 * index + 2;

        /* Assume the current email has the highest priority. */
        int largest = index;

        /* Check whether the left child has higher priority. */
        if (left < h->size &&
            isHigherPriority(&h->data[left],
                             &h->data[largest]))
        {
            /* Store the left child's position. */
            largest = left;
        }

        /* Check whether the right child has higher priority. */
        if (right < h->size &&
            isHigherPriority(&h->data[right],
                             &h->data[largest]))
        {
            /* Store the right child's position. */
            largest = right;
        }

        /* Stop when the current position is correct. */
        if (largest == index)
        {
            break;
        }

        /* Swap with the higher-priority child. */
        swapEmails(&h->data[index],
                   &h->data[largest]);

        /* Continue from the child's position. */
        index = largest;
    }
}

/* Insert an email into the MaxHeap. */
void heapPush(MaxHeap *h, Email e)
{
    /* Check whether the heap is full. */
    if (h->size == h->capacity)
    {
        /* Double the capacity if the heap is full. */
        heapGrow(h);
    }

    /* Place the new email at the end of the heap. */
    h->data[h->size] = e;

    /* Increase the number of emails in the heap. */
    h->size++;

    /* Move the new email upward to restore heap order. */
    siftUp(h, h->size - 1);
}

/* Remove and return the highest-priority email. */
Email heapPop(MaxHeap *h)
{
    /* Save the highest-priority email before removing it. */
    Email top = h->data[0];

    /* Decrease the number of emails by one. */
    h->size--;

    /* Check whether there are still emails remaining. */
    if (h->size > 0)
    {
        /* Move the last email to the root position. */
        h->data[0] = h->data[h->size];

        /* Restore the MaxHeap property. */
        siftDown(h, 0);
    }

    /* Return the email that was removed. */
    return top;
}

/* Return the highest-priority email. */
Email *heapPeek(MaxHeap *h)
{
    /* Check whether the heap is empty. */
    if (h->size == 0)
    {
        /* Return NULL because no email exists. */
        return NULL;
    }

    /* Return the email at the root of the heap. */
    return &h->data[0];
}

/* Return 1 if the heap is empty and 0 otherwise. */
int heapIsEmpty(MaxHeap *h)
{
    /* Compare the current heap size with zero. */
    return h->size == 0;
}

/* Return the priority value for a sender category. */
int getCategoryPriority(const char *category)
{
    /* Boss has the highest priority. */
    if (strcmp(category, "Boss") == 0)
        return 5;

    /* Subordinate has the second-highest priority. */
    if (strcmp(category, "Subordinate") == 0)
        return 4;

    /* Peer has the third-highest priority. */
    if (strcmp(category, "Peer") == 0)
        return 3;

    /* ImportantPerson has the fourth-highest priority. */
    if (strcmp(category, "ImportantPerson") == 0)
        return 2;

    /* OtherPerson has the lowest required priority. */
    if (strcmp(category, "OtherPerson") == 0)
        return 1;

    /* Return zero for an unknown category. */
    return 0;
}

/* Convert a date string into a comparable number. */
long dateToValue(const char *date)
{
    /* Variables for the month, day, and year. */
    int month;
    int day;
    int year;

    /* Parse the three parts of the date. */
    if (sscanf(date,
               "%d-%d-%d",
               &month,
               &day,
               &year) != 3)
    {
        /* Return zero if the date is invalid. */
        return 0;
    }

    /* Combine the values into YYYYMMDD format. */
    return (long)year * 10000L +
           (long)month * 100L +
           (long)day;
}

/* Remove unnecessary whitespace from a string. */
void trim(char *str)
{
    /* Start at the first character. */
    int start = 0;

    /* Find the first non-whitespace character. */
    while (str[start] != '\0' &&
           isspace((unsigned char)str[start]))
    {
        /* Move to the next character. */
        start++;
    }

    /* Check whether leading whitespace was found. */
    if (start > 0)
    {
        /* Move the remaining string to the beginning. */
        memmove(str,
                str + start,
                strlen(str + start) + 1);
    }

    /* Find the length of the string. */
    int len = (int)strlen(str);

    /* Remove whitespace from the end of the string. */
    while (len > 0 &&
           isspace((unsigned char)str[len - 1]))
    {
        /* Replace the final whitespace with '\0'. */
        str[len - 1] = '\0';

        /* Reduce the length by one. */
        len--;
    }
}

/* Process an EMAIL command. */
void processEmailCommand(MaxHeap *h, char *rest)
{
    /* Create storage for the sender category. */
    char category[MAX_CATEGORY_LEN] = {0};

    /* Create storage for the subject. */
    char subject[MAX_SUBJECT_LEN] = {0};

    /* Create storage for the date. */
    char date[MAX_DATE_LEN] = {0};

    /* Create a temporary buffer for parsing the command. */
    char buffer[MAX_LINE_LEN];

    /* Safely copy the command into the buffer. */
    strncpy(buffer,
            rest,
            MAX_LINE_LEN - 1);

    /* Guarantee that the buffer is null terminated. */
    buffer[MAX_LINE_LEN - 1] = '\0';

    /* Extract the sender category before the first comma. */
    char *field1 = strtok(buffer, ",");

    /* Extract the subject before the second comma. */
    char *field2 = strtok(NULL, ",");

    /* Extract the date after the second comma. */
    char *field3 = strtok(NULL, ",");

    /* Check whether all three required fields were found. */
    if (field1 == NULL ||
        field2 == NULL ||
        field3 == NULL)
    {
        /* Display an error for an incomplete EMAIL command. */
        fprintf(stderr,
                "Malformed EMAIL command, ignoring: %s\n",
                rest);

        /* Do not add an incomplete email to the heap. */
        return;
    }

    /* Remove extra whitespace from the category. */
    trim(field1);

    /* Remove extra whitespace from the subject. */
    trim(field2);

    /* Remove extra whitespace from the date. */
    trim(field3);

    /* Safely copy the category into its storage array. */
    strncpy(category,
            field1,
            MAX_CATEGORY_LEN - 1);

    /* Guarantee that category is null terminated. */
    category[MAX_CATEGORY_LEN - 1] = '\0';

    /* Safely copy the subject into its storage array. */
    strncpy(subject,
            field2,
            MAX_SUBJECT_LEN - 1);

    /* Guarantee that subject is null terminated. */
    subject[MAX_SUBJECT_LEN - 1] = '\0';

    /* Safely copy the date into its storage array. */
    strncpy(date,
            field3,
            MAX_DATE_LEN - 1);

    /* Guarantee that date is null terminated. */
    date[MAX_DATE_LEN - 1] = '\0';

    /* Create an Email structure. */
    Email e;

    /* Copy the category into the Email structure. */
    strncpy(e.category,
            category,
            MAX_CATEGORY_LEN - 1);

    /* Guarantee that the category is null terminated. */
    e.category[MAX_CATEGORY_LEN - 1] = '\0';

    /* Copy the subject into the Email structure. */
    strncpy(e.subject,
            subject,
            MAX_SUBJECT_LEN - 1);

    /* Guarantee that the subject is null terminated. */
    e.subject[MAX_SUBJECT_LEN - 1] = '\0';

    /* Copy the date into the Email structure. */
    strncpy(e.date,
            date,
            MAX_DATE_LEN - 1);

    /* Guarantee that the date is null terminated. */
    e.date[MAX_DATE_LEN - 1] = '\0';

    /* Convert the sender category into a priority number. */
    e.priority = getCategoryPriority(e.category);

    /* Convert the date into a comparable numerical value. */
    e.dateValue = dateToValue(e.date);

    /* Add the completed email to the MaxHeap. */
    heapPush(h, e);
}

/* Process the NEXT command. */
void processNextCommand(MaxHeap *h)
{
    /* Get the highest-priority email without removing it. */
    Email *top = heapPeek(h);

    /* Check whether there are no unread emails. */
    if (top == NULL)
    {
        /* Handle NEXT when the heap is empty. */
        printf("No emails in queue.\n");

        /* Stop processing the command. */
        return;
    }

    /* Display the required heading. */
    printf("Next email:\n");

    /* Display the sender category. */
    printf("Sender: %s\n",
           top->category);

    /* Display the subject line. */
    printf("Subject: %s\n",
           top->subject);

    /* Display the date. */
    printf("Date: %s\n",
           top->date);
}

/* Process the READ command. */
void processReadCommand(MaxHeap *h)
{
    /* Check whether the heap is empty. */
    if (heapIsEmpty(h))
    {
        /* Handle READ when there are no emails. */
        printf("No emails to read.\n");

        /* Stop processing the command. */
        return;
    }

    /* Remove the highest-priority email from the heap. */
    heapPop(h);
}

/* Process the COUNT command. */
void processCountCommand(MaxHeap *h)
{
    /* Display the current number of unread emails. */
    printf("There are %d emails to read.\n",
           h->size);
}

/* Main function where program execution begins. */
int main(void)
{
    /* Create a MaxHeap variable for the CEO's inbox. */
    MaxHeap heap;

    /* Initialize the heap with the starting capacity. */
    heapInit(&heap,
             INITIAL_CAPACITY);

    /* Create an array for storing each input line. */
    char line[MAX_LINE_LEN];

    /* Continue reading commands until the input ends. */
    while (fgets(line,
                 sizeof(line),
                 stdin) != NULL)
    {
        /* Find the length of the input line. */
        size_t len = strlen(line);

        /* Remove newline and carriage-return characters. */
        while (len > 0 &&
               (line[len - 1] == '\n' ||
                line[len - 1] == '\r'))
        {
            /* Replace the newline with the null terminator. */
            line[len - 1] = '\0';

            /* Decrease the recorded length. */
            len--;
        }

        /* Check whether the input line is blank. */
        if (len == 0)
        {
            /* Ignore blank lines. */
            continue;
        }

        /* Check whether the command begins with "EMAIL ". */
        if (strncmp(line,
                    "EMAIL ",
                    6) == 0)
        {
            /* Process the EMAIL command. */
            processEmailCommand(&heap,
                                line + 6);
        }

        /* Check whether the command is NEXT. */
        else if (strcmp(line,
                        "NEXT") == 0)
        {
            /* Display the next email without removing it. */
            processNextCommand(&heap);
        }

        /* Check whether the command is READ. */
        else if (strcmp(line,
                        "READ") == 0)
        {
            /* Remove the highest-priority email. */
            processReadCommand(&heap);
        }

        /* Check whether the command is COUNT. */
        else if (strcmp(line,
                        "COUNT") == 0)
        {
            /* Display the current unread email count. */
            processCountCommand(&heap);
        }

        /* Handle any command that does not match the required format. */
        else
        {
            /* Display an error for an unrecognized command. */
            fprintf(stderr,
                    "Unrecognized command: %s\n",
                    line);
        }
    }

    /* Release the memory used by the heap. */
    heapFree(&heap);

    /* Return zero to indicate successful execution. */
    return 0;
}
