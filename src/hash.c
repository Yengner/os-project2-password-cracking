/*
 * Group: 22
 * Members: 
 * Project 2: Optimize password cracking using multithreading to match
 * passwords against MD5, SHA1, SHA256, and SHA512 hashes efficiently.
 * Description: First-half implementation: indexed target lookup with ordered,
 * single-threaded matching. The teammate's second half adds pthread batching.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "hash_functions.h"

#define KEEP 16
#define HASH_TEXT_LENGTH (2 * KEEP)
#define N_ALGORITHMS 4
#define NO_ROW SIZE_MAX
#define NUM_THREADS 8
#define BATCH_SIZE 4096

typedef unsigned char *(*hashing)(unsigned char *, unsigned int);

static hashing const functions[N_ALGORITHMS] = {
    calculate_md5, calculate_sha1, calculate_sha256, calculate_sha512
};
static const char *const algorithms[N_ALGORITHMS] = {
    "MD5", "SHA1", "SHA256", "SHA512"
};

struct cracked_hash {
    char hash[HASH_TEXT_LENGTH + 1];
    char *password;
    const char *algorithm;
    size_t next;
};

struct target_lookup {
    struct cracked_hash *rows;
    size_t count;
    size_t remaining;
    size_t *buckets;
    size_t bucket_count;
};

static void fail(const char *message) {
    fprintf(stderr, "project2: %s\n", message);
    exit(EXIT_FAILURE);
}

static void *allocate_array(size_t count, size_t size) {
    if (count > SIZE_MAX / size)
        fail("input is too large");
    /* A zero-row input still gets a valid allocation for portable cleanup. */
    void *memory = calloc(count == 0 ? 1 : count, size);
    if (memory == NULL)
        fail("memory allocation failed");
    return memory;
}

static FILE *open_file(const char *path, const char *mode) {
    FILE *file = fopen(path, mode);
    if (file == NULL) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    return file;
}

static size_t bucket_for(const char *hash, size_t bucket_count) {
    /* Unsigned arithmetic intentionally wraps; compare the full key on lookup. */
    size_t value = 5381;
    for (size_t i = 0; i < HASH_TEXT_LENGTH; i++)
        value = value * 33 + (unsigned char)hash[i];
    return value % bucket_count;
}

static struct target_lookup load_targets(const char *path) {
    struct target_lookup lookup = {0};
    char hash[HASH_TEXT_LENGTH + 1];
    FILE *file = open_file(path, "r");
    while (fscanf(file, "%32s", hash) == 1) {
        if (lookup.count == SIZE_MAX)
            fail("too many target hashes");
        lookup.count++;
    }
    if (ferror(file))
        fail("could not read target hashes");
    rewind(file);
    lookup.rows = allocate_array(lookup.count, sizeof(*lookup.rows));
    lookup.remaining = lookup.count;
    lookup.bucket_count = lookup.count;
    if (lookup.count <= (SIZE_MAX - 1) / 2)
        lookup.bucket_count = lookup.count * 2 + 1;
    if (lookup.bucket_count == 0)
        lookup.bucket_count = 1;
    lookup.buckets = allocate_array(lookup.bucket_count, sizeof(*lookup.buckets));
    for (size_t i = 0; i < lookup.bucket_count; i++)
        lookup.buckets[i] = NO_ROW;
    for (size_t i = 0; i < lookup.count; i++) {
        if (fscanf(file, "%32s", lookup.rows[i].hash) != 1)
            fail("target file changed while reading");
    }
    fclose(file);

    /* Prepending in reverse row order puts each duplicate's earliest row first. */
    for (size_t i = lookup.count; i > 0; i--) {
        size_t row = i - 1;
        size_t bucket = bucket_for(lookup.rows[row].hash, lookup.bucket_count);
        lookup.rows[row].next = lookup.buckets[bucket];
        lookup.buckets[bucket] = row;
    }
    return lookup;
}

/* Pure per-password computation: future workers write to their own result slot. */
static void compute_password_hashes(const char *password,
                                    char hashes[N_ALGORITHMS][HASH_TEXT_LENGTH + 1]) {
    static const char hex[] = "0123456789abcdef";
    unsigned int length = (unsigned int)strlen(password);
    for (size_t algorithm = 0; algorithm < N_ALGORITHMS; algorithm++) {
        unsigned char *digest = functions[algorithm]((unsigned char *)password, length);
        if (digest == NULL)
            fail("hash calculation failed");
        for (size_t byte = 0; byte < KEEP; byte++) {
            hashes[algorithm][2 * byte] = hex[digest[byte] >> 4];
            hashes[algorithm][2 * byte + 1] = hex[digest[byte] & 15];
        }
        hashes[algorithm][HASH_TEXT_LENGTH] = '\0';
        free(digest);
    }
}

/* Only the ordered caller mutates the lookup table or assigns output rows. */
static void apply_password_hashes(struct target_lookup *lookup, const char *password,
                                  const char hashes[N_ALGORITHMS][HASH_TEXT_LENGTH + 1]) {
    for (size_t algorithm = 0; algorithm < N_ALGORITHMS; algorithm++) {
        size_t bucket = bucket_for(hashes[algorithm], lookup->bucket_count);
        size_t *link = &lookup->buckets[bucket];
        while (*link != NO_ROW) {
            struct cracked_hash *row = &lookup->rows[*link];
            if (memcmp(row->hash, hashes[algorithm], HASH_TEXT_LENGTH) == 0) {
                size_t length = strlen(password) + 1;
                row->password = allocate_array(length, sizeof(char));
                memcpy(row->password, password, length);
                row->algorithm = algorithms[algorithm];
                /* Remove just this row; another occurrence may fill a duplicate. */
                *link = row->next;
                lookup->remaining--;
                break;
            }
            link = &row->next;
        }
    }
}

static void write_results(const struct target_lookup *lookup, const char *path) {
    FILE *file = open_file(path, "w");
    for (size_t i = 0; i < lookup->count; i++) {
        const struct cracked_hash *row = &lookup->rows[i];
        int written;
        if (row->password == NULL)
            written = fprintf(file, "not found\n");
        else
            written = fprintf(file, "%s:%s\n", row->password, row->algorithm);
        if (written < 0)
            fail("could not write results");
    }
    if (fclose(file) != 0)
        fail("could not close output file");
}
/* Store each password and its computed hashes. */
struct password_task {
    char password[256];
    char hashes[N_ALGORITHMS][HASH_TEXT_LENGTH + 1];
};

/* Shared batch state and thread synchronization. */
struct worker_pool {
    struct password_task *tasks;
    size_t count;
    size_t completed;
    size_t generation;
    int stop;
    pthread_mutex_t mutex;
    pthread_cond_t ready;
    pthread_cond_t done;
};

struct worker_args {
    struct worker_pool *pool;
    size_t id;
};

/* Workers wait for a batch, hash their assigned passwords, and report completion. */
static void *hash_worker(void *arg) {
    struct worker_args *worker = arg;
    struct worker_pool *pool = worker->pool;
    size_t generation = 0;

    while (1) {
        if (pthread_mutex_lock(&pool->mutex) != 0)
            fail("could not lock mutex");

        /* Wait until a new batch is ready or shutdown is requested. */
        while (!pool->stop && generation == pool->generation) {
            if (pthread_cond_wait(&pool->ready, &pool->mutex) != 0)
                fail("could not wait for work");
        }

        if (pool->stop) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        /* Split the current batch into nonoverlapping worker ranges. */
        generation = pool->generation;
        struct password_task *tasks = pool->tasks;
        size_t start = worker->id * pool->count / NUM_THREADS;
        size_t end = (worker->id + 1) * pool->count / NUM_THREADS;

        if (pthread_mutex_unlock(&pool->mutex) != 0)
            fail("could not unlock mutex");

        for (size_t i = start; i < end; i++)
            compute_password_hashes(tasks[i].password, tasks[i].hashes);

        /* Notify main when all workers finish. */
        if (pthread_mutex_lock(&pool->mutex) != 0)
            fail("could not lock mutex");

        pool->completed++;
        if (pool->completed == NUM_THREADS)
            pthread_cond_signal(&pool->done);

        if (pthread_mutex_unlock(&pool->mutex) != 0)
            fail("could not unlock mutex");
    }

    return NULL;
}

/* Read one bounded batch while preserving password order. */
static size_t read_batch(FILE *file, struct password_task *tasks) {
    size_t count = 0;
    while (count < BATCH_SIZE &&
           fscanf(file, "%255s", tasks[count].password) == 1)
        count++;
    return count;
}

/* Publish the next batch and wake the workers. */
static void start_batch(struct worker_pool *pool,
                        struct password_task *tasks, size_t count) {
    if (pthread_mutex_lock(&pool->mutex) != 0)
        fail("could not lock mutex");

    pool->tasks = tasks;
    pool->count = count;
    pool->completed = 0;
    pool->generation++;

    if (pthread_cond_broadcast(&pool->ready) != 0)
        fail("could not notify workers");

    if (pthread_mutex_unlock(&pool->mutex) != 0)
        fail("could not unlock mutex");
}

/* Block until all workers finish the batch. */
static void wait_batch(struct worker_pool *pool) {
    if (pthread_mutex_lock(&pool->mutex) != 0)
        fail("could not lock mutex");

    while (pool->completed < NUM_THREADS) {
        if (pthread_cond_wait(&pool->done, &pool->mutex) != 0)
            fail("could not wait for workers");
    }

    if (pthread_mutex_unlock(&pool->mutex) != 0)
        fail("could not unlock mutex");
}

/* Run parallel hashing, ordered matching, and worker cleanup. */
void crack_hashed_passwords(char *password_list, char *hashed_list, char *output) {
    struct target_lookup lookup = load_targets(hashed_list);
    FILE *file = open_file(password_list, "r");
    struct password_task *tasks[2];

    /* Alternate between two fixed size batch buffers. */
    tasks[0] = allocate_array(BATCH_SIZE, sizeof(*tasks[0]));
    tasks[1] = allocate_array(BATCH_SIZE, sizeof(*tasks[1]));

    pthread_t threads[NUM_THREADS];
    struct worker_args args[NUM_THREADS];
    struct worker_pool pool = {0};

    if (pthread_mutex_init(&pool.mutex, NULL) != 0)
        fail("could not initialize mutex");
    if (pthread_cond_init(&pool.ready, NULL) != 0)
        fail("could not initialize condition");
    if (pthread_cond_init(&pool.done, NULL) != 0)
        fail("could not initialize condition");

    /* Create a fixed worker pool once. */
    for (size_t i = 0; i < NUM_THREADS; i++) {
        args[i].pool = &pool;
        args[i].id = i;
        if (pthread_create(&threads[i], NULL, hash_worker, &args[i]) != 0)
            fail("could not create thread");
    }

    size_t current = 0;
    size_t count = 0;

    if (lookup.remaining != 0)
        count = read_batch(file, tasks[current]);

    if (count != 0)
        start_batch(&pool, tasks[current], count);

    /* Overlap hashing of the next batch with ordered matching of the previous one. */
    while (count != 0) {
        wait_batch(&pool);

        size_t next = 1 - current;
        size_t next_count = read_batch(file, tasks[next]);

        if (next_count != 0)
            start_batch(&pool, tasks[next], next_count);

        for (size_t i = 0; i < count; i++) {
            apply_password_hashes(&lookup,
                                  tasks[current][i].password,
                                  tasks[current][i].hashes);
            if (lookup.remaining == 0)
                break;
        }

        if (lookup.remaining == 0) {
            if (next_count != 0)
                wait_batch(&pool);
            break;
        }

        current = next;
        count = next_count;
    }

    if (ferror(file))
        fail("could not read passwords");

    /* Signal shutdown and join all workers. */
    if (pthread_mutex_lock(&pool.mutex) != 0)
        fail("could not lock mutex");

    pool.stop = 1;
    if (pthread_cond_broadcast(&pool.ready) != 0)
        fail("could not notify workers");

    if (pthread_mutex_unlock(&pool.mutex) != 0)
        fail("could not unlock mutex");

    for (size_t i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0)
            fail("could not join thread");
    }

    /* Release synchronization objects, buffers, and lookup data. */
    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.ready);
    pthread_cond_destroy(&pool.done);

    free(tasks[0]);
    free(tasks[1]);
    fclose(file);
    write_results(&lookup, output);
    for (size_t i = 0; i < lookup.count; i++)
        free(lookup.rows[i].password);
    free(lookup.rows);
    free(lookup.buckets);
}

