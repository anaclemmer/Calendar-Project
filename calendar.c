/*
Ana Clemmer
117592283
aclemmer
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calendar.h"

/* Prototypes */
int init_calendar(const char *name, int days,
                  int (*comp_func) (const void *ptr1, const void *ptr2),
                  void (*free_info_func) (void *ptr), Calendar ** calendar);
int print_calendar(Calendar *calendar, FILE *output_stream, int print_all);
int add_event(Calendar *calendar, const char *name, int start_time,
              int duration_minutes, void *info, int day);
int find_event(Calendar *calendar, const char *name, Event **event);
int find_event_in_day(Calendar *calendar, const char *name, int day,
                      Event **event);
int remove_event(Calendar *calendar, const char *name);
void *get_event_info(Calendar *calendar, const char *name);
int clear_calendar(Calendar *calendar);
int clear_day(Calendar *calendar, int day);
int destroy_calendar(Calendar *calendar);

/* Functions */
/*
This function initializes a Calendar structure based on the parameters provided.
The function allocates memory for the following items: 
Calendar structure.
name field will be assigned memory necessary to store a copy of the name 
    parameter. The name represents the calendar's name.
events field will be assigned memory necessary to represent an array of pointers 
    to Event structures. The size of this array corresponds to the days 
    parameter.
The out parameter calendar will provide access to the new Calendar structure.
Returns FAILURE if calendar or name are NULL, number of days is less than 1, or 
    if memory allocation fails.
Returns SUCCESS otherwise.
*/
int init_calendar(const char *name, int days, 
                  int (*comp_func) (const void *ptr1, const void *ptr2),
                  void (*free_info_func) (void *ptr), Calendar ** calendar) {
    Calendar *cal;

    if (calendar == NULL || name == NULL || days < 1) {
        return FAILURE;
    }
    if ((cal = malloc(sizeof(Calendar))) == NULL) {
        return FAILURE;
    }
    if ((cal->name = malloc(strlen(name) + 1)) == NULL) { 
        return FAILURE;
    }
    strcpy(cal->name, name);
    if ((cal->events = calloc(days, sizeof(Event *))) == NULL) {
        return FAILURE;
    }
    
    cal->days = days;
    cal->total_events = 0;
    cal->comp_func = comp_func;
    cal->free_info_func = free_info_func;
    *calendar = cal;
    return SUCCESS;
}

/*
This function prints, to the designated output stream, the calendar's name, 
    days, and total number of events if print_all is true; otherwise this 
    information will not be printed. Information about each event (name, start
    time, and duration) will be printed regardless the value of print_all. 
Returns FAILURE if calendar or output_stream are NULL.
Returns SUCCESS otherwise.
*/
int print_calendar(Calendar *calendar, FILE *output_stream, int print_all) {
    Event *curr;
    int i;

    if (calendar == NULL || output_stream == NULL) {
        return FAILURE;
    }
    if (print_all) {
        fprintf(output_stream, "Calendar's Name: \"%s\"\n", calendar->name); 
        fprintf(output_stream, "Days: %d\n", calendar->days);
        fprintf(output_stream, "Total Events: %d\n", calendar->total_events);
    }
    fprintf(output_stream, "\n**** Events ****\n");
    if (calendar->total_events > 0) {
        for (i = 0; i < calendar->days; i++) {
            printf("Day %d\n", i + 1);
            for (curr = calendar->events[i]; curr != NULL; curr = curr->next) {
                fprintf(output_stream, 
                    "Event's Name: \"%s\", Start_time: %d, Duration: %d\n", 
                    curr->name, curr->start_time, curr->duration_minutes);            
            }
        }
    }
    return SUCCESS;
}

/*
This function adds the specified event to the list associated with the day 
    parameter. The event is added in increasing sorted order using the comparison 
    function (comp_func) that allows comparison of two events. The function 
    allocates memory for the new event and for the event's name. Other fields of 
    the event structure are initialized based on the parameter values.
Returns FAILURE if calendar or name are NULL, start time invalid- should be  
    [0,2400], duration_minutes <= 0, day < 1 or > calendar days, event already 
    exists, or if any memory allocation fails.
Returns SUCCESS otherwise.
*/
int add_event(Calendar *calendar, const char *name, int start_time,
              int duration_minutes, void *info, int day) {
    Event *curr = NULL, *new = NULL, *prev = NULL;

    if (calendar == NULL || name == NULL || start_time < 0 || start_time > 2400
        || duration_minutes <= 0 || day < 1 || day > calendar->days) {
        return FAILURE;
    }

    /* Initializing event to add */
    if ((new = malloc(sizeof(Event))) == NULL) {
        return FAILURE;
    }
    if ((new->name = malloc(strlen(name) + 1)) == NULL) {
        return FAILURE;
    }
    strcpy(new->name, name);
    new->duration_minutes = duration_minutes;
    new->info = info; 
    new->start_time = start_time;
    new->next = NULL;

    curr = calendar->events[day - 1];
    /* if no events */
    if (curr == NULL) {
        calendar->events[day - 1] = new;
        new->next = NULL;
        calendar->total_events++;
    } else {
        prev = NULL;
        for (curr = calendar->events[day - 1]; curr != NULL; curr = curr->next) {
            /* check for duplicate event */
            if (strcmp(new->name, curr->name) == 0) {
                return FAILURE;
            }
            if ((calendar->comp_func(curr, new)) >= 0) { /* curr > new */
                if (prev == NULL) { /* beginning of list */
                    new->next = calendar->events[day - 1];
                    calendar->events[day - 1] = new;
                    calendar->total_events++;
                    return SUCCESS;
                } else {
                    prev->next = new;
                    new->next = curr;
                    calendar->total_events++;
                    return SUCCESS;
                }
            } else { /* comp < 0 which means curr < new */
                /* move on to next */
                prev = curr;
                if (curr->next == NULL) { /* end of list */
                    curr->next = new;
                    new->next = NULL;
                    calendar->total_events++;
                    return SUCCESS;
                }
            }
        }
    }
    return SUCCESS;
}

/*
This function will return a pointer (via the out parameter event) to the event 
    with the specified name (if any). If the event parameter is NULL, no pointer
    will be returned. Event will not be modified until the event is found.
Returns SUCCESS if event was found.
Returns FAILURE if calendar or name are NULL and otherwise.
*/
int find_event(Calendar *calendar, const char *name, Event **event) {
    int i;

    if (calendar == NULL || name == NULL || event == NULL) {
        return FAILURE;
    }
    for (i = 1; i <= calendar->days; i++) {
        if (find_event_in_day(calendar, name, i, event) == SUCCESS) {
            return SUCCESS;
        }
    }
    return FAILURE;
}

/*
This function will return a pointer (via the out parameter event) to the event 
    with the specified name in the specified day (if such event exists). If the 
    event parameter is NULL, no pointer will be returned. The out parameter 
    event will not be modified unless the event is found.
Returns SUCCESS if the event was found.
Returns FAILURE if calendar or name is NULL, if the day parameter < 1 or > 
    calendar days, and otherwise.
*/
int find_event_in_day(Calendar *calendar, const char *name, int day,
                      Event **event) {
    Event *curr;

    if (calendar == NULL || name == NULL || day < 1 || day > calendar->days) {
        return FAILURE;

    }
    for (curr = calendar->events[day - 1]; curr != NULL; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            (*event) = curr;
            return SUCCESS;
        }
    }
    return FAILURE;
}

/*
This function will remove the specified event from the calendar returning any 
    memory allocated for the event. If the event has an info field other than 
    NULL, and a free_info_func exists, the function will be called on the info 
    field. The number of calendar events must be adjusted accordingly.
Returns FAILURE if calendar or name are NULL or if event cannot be found.
Returns SUCCESS otherwise.
*/
int remove_event(Calendar *calendar, const char *name) {
    int i;
    Event *curr, *temp;

    if (calendar == NULL || name == NULL) {
        return FAILURE;
    }

    /* search each day in calendar for name */
    for (i = 0; i < calendar->days; i++) {
        for (curr = calendar->events[i]; curr != NULL;) {
            if (strcmp(curr->name, name) == 0) { /* only happens at head */
                temp = curr;
                if (temp->info != NULL && calendar->free_info_func != NULL) {
                    calendar->free_info_func(temp->info);
                    temp->info = NULL;
                }
                free(temp->name);
                temp->name = NULL;
                calendar->events[i] = curr->next;
                free(temp);
                temp = NULL;
                calendar->total_events--;
                return SUCCESS;
            } else if (curr->next != NULL && 
                       ((strcmp(curr->next->name, name)) == 0)) { 
                temp = curr->next;
                curr->next = curr->next->next;
                if (temp->info != NULL && calendar->free_info_func != NULL) {
                    calendar->free_info_func(temp->info);
                    temp->info = NULL;
                }
                free(temp->name);
                temp->name = NULL;
                free(temp);
                temp = NULL;
                calendar->total_events--;
                return SUCCESS;
            }
            /* advance */
            curr = curr->next;
        }

    }
    /* if event is not found */
    return FAILURE;
}

/*
This functions returns the info pointer associated with the specified event. 
It is assumed that calendar and name parameters are different than NULL.
Returns NULL if event is not found.
*/
void *get_event_info(Calendar *calendar, const char *name) {
    Event *event;

    if (find_event(calendar, name, &event) == FAILURE) {
        return NULL;
    }
    return event->info; 
}

/*
This function will remove all the event lists associated with the calendar and 
    set them to empty lists. The array holding the event lists will not be removed. 
    The total number of events will be set to 0. If an event has an info field 
    other than NULL and a free_info_func exists, the function will be called on 
    the info field.
Returns FAILURE if calendar is NULL.
Reutrns SUCCESS otherwise.
*/
int clear_calendar(Calendar *calendar) {
    int i;

    if (calendar == NULL) {
        return FAILURE;
    }
    /* for loop freeing/removing all events */
    for (i = 0; i < calendar->days; i++) {
        while (calendar->events[i] != NULL) {
            remove_event(calendar, calendar->events[i]->name);
        }
    }
    return SUCCESS;
}

/*
This function will remove all the events for the specified day, setting the 
    event list to empty. The total number of events will be adjusted accordingly. 
    If an event has an info field other than NULL and a free_info_func exists, 
    the function will be called on the info field. 
Returns FAILURE if calendar is NULL, day < 1 or > calendar days.
Returns SUCCESS otherwise.
*/
int clear_day(Calendar *calendar, int day) {

    if (calendar == NULL || day < 1 || day > calendar->days) {
        return FAILURE;
    }
    while (calendar->events[day - 1] != NULL) {
        remove_event(calendar, calendar->events[day - 1]->name);
    }

    return SUCCESS;
}

/*
This function will return the memory that was dynamically-allocated for the 
    calendar. If an event has an info field other than NULL and a free_info_func 
    exists, the function will be called on the info field.
Returns FAILURE if calendar is NULL.
Returns SUCCESS otherwise.
*/
int destroy_calendar(Calendar *calendar) {
    if (calendar == NULL) {
        return FAILURE;
    }
    clear_calendar(calendar);
    free(calendar->events);
    calendar->events = NULL;
    free(calendar->name);
    calendar->name = NULL;
    free(calendar);
    calendar = NULL;
    return SUCCESS;
}