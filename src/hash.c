/*
 * Group: TODO - enter the Canvas group number before submission.
 * Authors / NetIDs: TODO - each contributor must enter their own details.
 * Description: First-half implementation: indexed target lookup with ordered,
 * single-threaded matching. The teammate's second half adds pthread batching.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "hash_functions.h"

#define KEEP 16
#define HASH_TEXT_LENGTH (2 * KEEP)
#define N_ALGORITHMS 4
#define NO_ROW SIZE_MAX

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
    lookup.bucket_count = lookup.count == 0 ? 1 : lookup.count;
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

void crack_hashed_passwords(char *password_list, char *hashed_list, char *output) {
    struct target_lookup lookup = load_targets(hashed_list);
    FILE *file = open_file(password_list, "r");
    char password[256];
    char hashes[N_ALGORITHMS][HASH_TEXT_LENGTH + 1];

    /* SECOND HALF: replace this serial loop with bounded pthread batches.
     * Keep apply_password_hashes on the main thread in password input order.
     */
    while (fscanf(file, "%255s", password) == 1) {
        compute_password_hashes(password, hashes);
        apply_password_hashes(&lookup, password, hashes);
    }
    if (ferror(file))
        fail("could not read passwords");
    fclose(file);
    write_results(&lookup, output);
    for (size_t i = 0; i < lookup.count; i++)
        free(lookup.rows[i].password);
    free(lookup.rows);
    free(lookup.buckets);
}
