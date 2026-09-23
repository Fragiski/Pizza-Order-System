#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define Ntel 2
#define Ncook 2
#define Noven 10
#define Npacker 2
#define Ndeliverer 10

#define Torderlow 1
#define Torderhigh 5

#define Norderlow 1
#define Norderhigh 5

#define Pm 45
#define Pp 35
#define Ps 20

#define Tpaymentlow 1
#define Tpaymenthigh 3

#define Pfail 5

#define Cm 12
#define Cp 14
#define Cs 15

#define Tprep 1
#define Tbake 10
#define Tpack 1

#define Tdellow 10
#define Tdelhigh 15

extern int available_tel, available_cook, available_ovens, available_packer, available_deliverer;

extern pthread_mutex_t tel_mutex, cook_mutex, oven_mutex, packer_mutex, deliverer_mutex;
extern pthread_cond_t tel_cond, cook_cond, oven_cond, packer_cond, deliverer_cond;

extern int total_margarita, total_pepperoni, total_special;
extern double revenue;
extern int success_orders, failed_orders;
extern double max_service_time, total_service_time;
extern double max_cool_time, total_cool_time;
extern pthread_mutex_t stats_mutex;

extern pthread_mutex_t screen_mutex;

typedef struct {
    int id;
    unsigned int seed;
} order_t;
