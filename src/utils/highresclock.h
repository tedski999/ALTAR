#ifndef ALTAR_UTILS_HIGHRESCLOCK_H
#define ALTAR_UTILS_HIGHRESCLOCK_H

struct altar_highresclock;

struct altar_highresclock *altar_highresclock_create(void);
unsigned long altar_highresclock_resetTime(struct altar_highresclock *clock);
double altar_highresclock_timeInSeconds(struct altar_highresclock *clock);
unsigned long altar_highresclock_timeInNanoseconds(struct altar_highresclock *clock);
void altar_highresclock_destroy(struct altar_highresclock *clock);

#endif

