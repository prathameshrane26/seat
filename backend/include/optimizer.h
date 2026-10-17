#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#define MAX_GUESTS 100
#define MAX_TABLES 20
#define MAX_SEATS_PER_TABLE 10

typedef struct {
    int id;
    char name[64];
    char email[64];
} Guest;

typedef struct {
    int id;
    char name[32];
    int capacity;
    int current_count;
    Guest guests[MAX_SEATS_PER_TABLE];
} Table;

// Main seating optimization engine function prototype
void optimize_seating(void);

#endif // OPTIMIZER_H
