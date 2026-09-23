# Pizza Order System (POSIX Threads)

A multithreaded pizza ordering system simulating a pizzeria's operations, developed in C using POSIX threads (pthreads). Each customer order runs as an independent thread, competing for a shared, limited pool of resources (phone operators, cooks, ovens, packers, and delivery drivers) synchronized with mutexes and condition variables.

Developed for the "Operating Systems" course, Spring Semester 2025. Group project.

## How It Works

Each order is represented by its own thread, which progresses through the following stages, each requiring exclusive access to a limited resource:

1. **Phone call** — waits for an available phone operator to place the order
2. **Payment** — simulated delay, with a random chance of payment failure
3. **Cooking** — waits for an available cook, then prepares the pizzas
4. **Baking** — waits for available oven capacity (multiple ovens can be used based on order size)
5. **Packing** — waits for an available packer
6. **Delivery** — waits for an available delivery driver, then delivers and returns

Resource availability is tracked with shared variables, protected by mutexes, with threads blocking on condition variables until the resource they need becomes available.

## Statistics Tracked

- Successful vs. failed orders
- Total revenue
- Pizza counts by type (Margherita, Pepperoni, Special)
- Average and maximum service time
- Average and maximum "cooling" time (between baking and delivery)

## Built With

- C
- POSIX Threads (`pthread`)
- Mutexes and condition variables for synchronization

## How to Build & Run

```bash
gcc -pthread -o pizza pizza.c
./pizza <number_of_customers> <seed>
```

- `number_of_customers` — number of customer order threads to simulate
- `seed` — random seed for reproducible order generation

## Team

Developed as a group project for the "Operating Systems" course, Spring 2025.
