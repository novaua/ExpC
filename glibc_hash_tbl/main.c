/* 
 * Fixed version of https://gist.github.com/cirocosta/e61e52cc6afe2633500b
 * Created on November 22, 2017, 7:26 AM
 */
#include <search.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static char* keys[] = {"ciro", "hue", "br", "haha", "Lol"};
static char* values[] = {"cccc", "hhhh", "bbbb", "aaa", "oooo"};

typedef struct sz_data_
{
    size_t size;
    char * value;
} sz_data_t;

typedef struct table_t
{
    struct hsearch_data htab;
    size_t size;
    char ** keys;
} table_t;
#define TABLE_T_INITIALIZER                                                    \
  (table_t)                                                                    \
  {                                                                            \
    .htab = (struct hsearch_data){ 0 }, .size = 0                              \
  }

// fwd decl
sz_data_t* table_get(table_t* table, char* key);

table_t* table_create(size_t size)
{
    table_t* table = malloc(sizeof (*table));

    *table = TABLE_T_INITIALIZER;
    hcreate_r(size, &table->htab);
    table->size = size;
    int ks = size * sizeof (char *);
    table->keys = malloc(ks);

    memset(table->keys, 0, ks);

    return table;
}

void table_destroy(table_t* table)
{
    int i = 0;
    for (; i < table->htab.filled; ++i)
    {
        sz_data_t * dt = table_get(table, table->keys[i]);
        assert(dt && "no data!");

        if (dt->value)
            free(dt->value);
        free(dt);

        free(table->keys[i]);
    }

    free(table->keys);

    hdestroy_r(&table->htab);
    free(table);
    table = NULL;
}

int table_add(table_t* table, char* key, sz_data_t * data)
{
    unsigned n = 0;
    ENTRY e, *ep;

    e.key = strdup(key);
    e.data = data;
    unsigned int fb = table->htab.filled;
    n = hsearch_r(e, ENTER, &ep, &table->htab);
    if (fb < table->htab.filled)
    {
        assert(table->keys[fb] == 0 && "free space");
        *(table->keys + fb) = e.key;
    }
    else
    {
        assert(0 && "overwrite is not supported!");
    }

    return n;
}

sz_data_t* table_get(table_t* table, char* key)
{
    unsigned n = 0;
    ENTRY e, *ep;

    e.key = key;
    n = hsearch_r(e, FIND, &ep, &table->htab);

    if (!n)
        return NULL;

    return (sz_data_t*) ep->data;
}

int main(int argc, char* argv[])
{
    unsigned i = 0;
    table_t* table = table_create(5);
    sz_data_t* d = NULL;

    if (argc < 2)
    {
        fprintf(stderr, "%s\n", "Usage: ./hash-tables <name>\n");
        exit(EXIT_FAILURE);
    }

    for (; i < table->size; i++)
    {
        sz_data_t * dt = malloc(sizeof (sz_data_t));
        dt->value = strdup(values[i]);

        table_add(table, keys[i], dt);
    }


    if ((d = table_get(table, argv[1])) != NULL)
        fprintf(stdout, "%s\n", (char*) d->value);
    else
        fprintf(stdout, "%s\n", "Not found :(");

    table_destroy(table);

    return 0;
}
