#ifndef ALTAR_UTILS_HIGHRESCLOCK_H
#define ALTAR_UTILS_HIGHRESCLOCK_H

struct altar_highresclock;

struct altar_highresclock *altar_utils_highresclock_create(void);
unsigned long altar_utils_highresclock_resetTime(struct altar_highresclock *clock);
double altar_utils_highresclock_timeInSeconds(struct altar_highresclock *clock);
unsigned long altar_utils_highresclock_timeInNanoseconds(struct altar_highresclock *clock);
void altar_utils_highresclock_destroy(struct altar_highresclock *clock);

#endif

