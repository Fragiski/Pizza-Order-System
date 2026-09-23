#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "p3230070-3230160-pizza.h"


//Resources
int available_tel = Ntel , available_cook = Ncook , available_ovens = Noven , available_packer = Npacker , available_deliverer = Ndeliverer;


// Mutex and condition variables
pthread_mutex_t tel_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cook_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oven_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t packer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t deliverer_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t tel_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cook_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t oven_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t packer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t deliverer_cond = PTHREAD_COND_INITIALIZER;


//Statistics

int total_margarita = 0 , total_pepperoni = 0 , total_special = 0;
double revenue = 0;
int success_orders = 0 , failed_orders = 0;
double max_service_time = 0, total_service_time = 0;
double max_cool_time = 0 , total_cool_time = 0;


pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;

void *order_thread(void *arg) {
    order_t *myorder = (order_t *) arg; //casting arg to the right variable type

    struct timespec start_time, end_time, bake_end_time;
    clock_gettime(CLOCK_REALTIME, &start_time); // Αρχή χρόνου

    int pizzas;
    int margarita = 0, pepperoni = 0, special = 0;
    int pay_time, fail_chance;

    /*Τηλεφωνητής*/
    pthread_mutex_lock(&tel_mutex);
    while (available_tel == 0)
        pthread_cond_wait(&tel_cond, &tel_mutex);

    available_tel--;
    pthread_mutex_unlock(&tel_mutex);

    /*Ποσότητα και είδος πίτσας*/
    pizzas = (rand_r(&myorder->seed) % (Norderhigh - Norderlow + 1)) + Norderlow; // rand_r = τυχαιος αριθμός, 
                                                                                //%(Norderhigh - Norderlow +1) περιορίζει τον αριθμό από το 0 εως το 4, 
                                                                                //+1 το μετατρεπει απο το 1 εως το 5

    for (int i = 0; i < pizzas; i++) {
        int kind = rand_r(&myorder->seed) % 100;
        if (kind < Pm ) margarita++;
        else if (kind < Pm + Pp) pepperoni++;
        else special++;
    }

    /*Χρόνος Πληρωμής*/
    pay_time = (rand_r(&myorder->seed) % (Tpaymenthigh - Tpaymentlow + 1)) + Tpaymentlow;
    sleep(pay_time);

    /*Πιθανότητα αποτυχίας κατά την πληρωμή*/
    fail_chance = rand_r (&myorder->seed) % 100;
    if (fail_chance < Pfail) {
        pthread_mutex_lock(&screen_mutex);
        printf("Η παραγγελία %d απετυχε.\n", myorder-> id);
        pthread_mutex_unlock(&screen_mutex);

        pthread_mutex_lock(&stats_mutex);
        failed_orders++;
        pthread_mutex_unlock(&stats_mutex);

        //Επιστροφή τηλεφωνητή

        pthread_mutex_lock(&tel_mutex);
        available_tel++;
        pthread_cond_signal(&tel_cond);
        pthread_mutex_unlock(&tel_mutex);

        pthread_exit(NULL);
    }

    /*Καταχώρηση Παραγγελίας*/
    pthread_mutex_lock(&screen_mutex);
    printf("Η παραγγελία %d καταχωρήθηκε επιτυχώς.\n", myorder->id);
    pthread_mutex_unlock(&screen_mutex);

    /*Ενημέρωση στατιστικών: Επιτυχημένες Παραγγελίες*/
    pthread_mutex_lock(&stats_mutex);
    success_orders++;
    total_margarita += margarita;
    total_pepperoni += pepperoni;
    total_special += special;
    revenue += margarita *Cm + pepperoni * Cp + special * Cs;
    pthread_mutex_unlock(&stats_mutex);

    /*Απελευθέρωση τηλεφωνητή*/
    pthread_mutex_lock(&tel_mutex);
    available_tel++;
    pthread_cond_signal(&tel_cond);
    pthread_mutex_unlock(&tel_mutex);

    /*Παρασκευαστής*/
    pthread_mutex_lock(&cook_mutex);
    while (available_cook == 0)
        pthread_cond_wait(&cook_cond, &cook_mutex);
    available_cook--;
    pthread_mutex_unlock(&cook_mutex);

    sleep(pizzas * Tprep); //Παρασκευή πίτσας 

    /*Διαθέσιμοι φούρνοι*/
    pthread_mutex_lock(&oven_mutex);
    while(available_ovens < pizzas)
        pthread_cond_wait(&oven_cond, &oven_mutex);
    available_ovens -= pizzas;
    pthread_mutex_unlock(&oven_mutex);

    /*Παρασκεύη επόμενης πίτσας*/
    pthread_mutex_lock(&cook_mutex);
    available_cook++;
    pthread_cond_signal(&cook_cond);
    pthread_mutex_unlock(&cook_mutex);

    sleep(Tbake); //Ψήσιμο
    clock_gettime(CLOCK_REALTIME, &bake_end_time); //Τέλος ψησίματος

    /*Πακετάρισμα*/
    pthread_mutex_lock(&packer_mutex);
    while(available_packer == 0)
        pthread_cond_wait(&packer_cond, &packer_mutex);
    available_packer--;
    pthread_mutex_unlock(&packer_mutex);

    sleep(pizzas * Tpack);  // Πακετάρισμα

    /*Απελευθέρωση υπαλλήλου πακεταρίσματος*/
    pthread_mutex_lock(&packer_mutex);
    available_packer++;
    pthread_cond_broadcast(&packer_cond);
    pthread_mutex_unlock(&packer_mutex);

    /*Απελευθέρωση φούρνων*/
    pthread_mutex_lock(&oven_mutex);
    available_ovens += pizzas;
    pthread_cond_broadcast(&oven_cond);
    pthread_mutex_unlock(&oven_mutex);

    clock_gettime(CLOCK_REALTIME, &end_time); //Τέλος παραγγελίας (πακεταρισμένη)


    /*Διανομή*/
    pthread_mutex_lock(&deliverer_mutex);
    while (available_deliverer == 0)
        pthread_cond_wait(&deliverer_cond, &deliverer_mutex);
    available_deliverer--;
    pthread_mutex_unlock(&deliverer_mutex);

    int delivery_time = (rand_r(&myorder->seed) % (Tdelhigh - Tdellow +1)) + Tdellow;

    sleep(delivery_time); //Παράδοση

    sleep(delivery_time); //Επιστροφή

    /*Απελευθέρωση διανομέα*/
    pthread_mutex_lock(&deliverer_mutex);
    available_deliverer++;
    pthread_cond_signal(&deliverer_cond);
    pthread_mutex_unlock(&deliverer_mutex);

    /* Υπολογισμός χρόνων και ενημέρωση στατιστικών  */

    double service_time = difftime(end_time.tv_sec, start_time.tv_sec);
    double cooling_time = difftime(end_time.tv_sec, bake_end_time.tv_sec);

    pthread_mutex_lock(&stats_mutex);
    total_service_time += service_time;
    total_cool_time += cooling_time;
    if (service_time > max_service_time)
        max_service_time = service_time;
    if (cooling_time > max_cool_time)
        max_cool_time = cooling_time;
    pthread_mutex_unlock(&stats_mutex);


    /* Εμφάνιση μηνυμάτων */

    pthread_mutex_lock(&screen_mutex);
    printf("Η παραγγελία %d ετοιμάστηκε σε %.0f λεπτά.\n", myorder->id , service_time);
    printf("H παραγγελία %d παραδόθηκε σε %0.f λεπτά.\n", myorder->id , service_time + delivery_time);
    pthread_mutex_unlock(&screen_mutex);

    pthread_exit(NULL);

}


int main (int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Χρήση: %s <αριθμός_πελατών> <σπόρος>\n", argv[0]);

        return 1;
    }

    int Ncust = atoi(argv[1]);
    int Seed = atoi (argv[2]);

    pthread_t threads[Ncust];
    order_t orders[Ncust];

    for (int i = 0; i < Ncust; i++) {
        orders[i].id = i + 1;
        orders[i].seed = Seed + i;

    

        int call_delay = (rand_r(&orders[i].seed) % (Torderhigh - Torderlow + 1)) + Torderlow;
        sleep(call_delay);

        if (pthread_create(&threads[i], NULL, order_thread, &orders[i]) != 0) {
            perror("pthread_create failed");
            return 2;
        }
    }


    for (int i = 0; i < Ncust; i++) {
        pthread_join(threads[i],NULL);
    }

    printf("\n--- Στατιστικά ---\n");
    printf("Επιτυχημένες παραγγελίες: %d\n", success_orders);
    printf("Αποτυχημένες παραγγελίες: %d\n", failed_orders);
    printf("Σύνολο εσόδων: %.2f ευρώ\n", revenue);
    printf("Πίτσες: Μαργαρίτα=%d, Πεπερόνι=%d, Σπέσιαλ=%d\n", total_margarita, total_pepperoni, total_special);

    if (success_orders > 0) {
        printf("Μέσος χρόνος εξυπηρέτησης: %.2f λεπτά\n", total_service_time / success_orders);
        printf("Μέγιστος χρόνος εξυπηρέτησης: %.2f λεπτά\n", max_service_time);
        printf("Μέσος χρόνος κρυώματος: %.2f λεπτά\n", total_cool_time / success_orders);
        printf("Μέγιστος χρόνος κρυώματος: %.2f λεπτά\n", max_cool_time);
    }


    return 0;

}