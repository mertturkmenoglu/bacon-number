#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TOKEN_DELIMITER "/"
#define MAX_INPUT 255
#define MAX_STDIN_LEN 255
#define BUFFER_SIZE 8192
#define NUMBER_BASE 10



/**
 * @struct Movie
 * @abstract a wrapping model for movies
 *
 * @discussion An abstraction level for expressing movies. An instance
 * of the struct holds the name of the movie, every actor's name that plays in that movie, number of the actors, and the
 * information about BFS(visited nodes).
 *
 * @field name is the string holds name of the movie
 * @field actors is the string array holds name of the actors that plays in the movie
 * @field actor_count is the number of actors plays in the movie
 * @field is_visited holds boolean value about visiting information for BFS
 */
struct Movie
{
    char *name;
    char **actors;
    int actor_count;
    int is_visited;
};


/**
 * @struct Actor
 * @abstract a wrapping model for actors
 *
 * @discussion An abstraction level for expressing actors. An instance
 * of the struct holds the name of the actor, every movie's name that actor plays, number of the movies, the
 * information about BFS(visited nodes), and parent actor information for backtracking at the BFS stage.
 *
 * @field name is the string holds name of the actor
 * @field movies is the string array holds name of the movies that actor plays
 * @field movie_count is the number of movie actor plays
 * @field is_visited holds boolean value about visiting information for BFS
 * @field parent_movie_name holds the movie name that this and parent Actor played together
 * @field parent holds a pointer to lastly visited node on the graph
 */
struct Actor
{
    char *name;
    char **movies;
    int movie_count;
    int is_visited;
    char *parent_movie_name;
    struct Actor *parent;
};


/**
 * @struct MapEntry
 * @abstract a wrapping model for map entries
 *
 * @discussion MapEntry is the unit element of the HashMap and it holds values as a Key - Value pair.
 *
 * @field key is the hash of the original key value. It is used for accessing element.
 * @field value is the pointer to hold information.
 * @field next is the pointer to collided entry.
 */
struct MapEntry
{
    long key;
    void *value;
    struct MapEntry *next;
};


/**
 * @struct HashTable
 *
 * @discussion HashTable holds values in an array and access to them via their key values. It uses key value as an index,
 * and when two entries collide, it uses separate chaining method to solve collisions.
 *
 * @field table_size is the number of cells in the table
 * @field count is number of how many entries at the given cell
 * @field head is the pointer to beginning of the table
 */
struct HashTable
{
    int table_size;
    int count;
    struct MapEntry *head;
};


/**
 * @struct Queue
 * @abstract a basic model for Queue data structure
 *
 * @discussion An abstract level for expressing queue. It points to
 * front element and rear element. It is implemented using linked list
 *
 * @field front is the pointer to front of the queue
 * @field rear is the pointer to rear of the queue
 */
struct Queue
{
    struct QueueNode *front;
    struct QueueNode *rear;
};


/**
 * @struct QueueNode
 * @abstract a queue element
 *
 * @discussion An element of the queue. Queue is implemented using linked list
 * and a QueueNode holds the information for the value and the address of the
 * next element of the queue.
 *
 * @field value is the value wrapped
 * @field next is the pointer to next queue element
 */
struct QueueNode
{
    void *value;
    struct QueueNode *next;
};


/**
 * Function prototypes
 */
char **read_file(char *path, int *line_count);

char **parse_line(char *line);

struct HashTable *create_hash_table(int m);

struct Movie *create_movie(char *name);

struct Actor *create_actor(char *name);

long hash(char *str);

struct MapEntry *create_map_entry(long key, void *value);

void build_hash_tables(char **lines, int line_count, struct HashTable *movies, struct HashTable *actors);

void insert(struct HashTable *ht, long key, void *value);

struct MapEntry *search(struct HashTable *ht, long key);

int find_bacon_number(char *start, struct HashTable *movies, struct HashTable *actors);

int find_distance(char *start, char *end, struct HashTable *movies, struct HashTable *actors);

struct QueueNode* create_queue_node(void* value);

struct Queue *create_queue();

void enqueue(struct Queue *q, void *value);

void *dequeue(struct Queue *q);


/**
 * Main entry point to program.
 */
int main()
{
    char path[MAX_STDIN_LEN];                           // File path
    char input[MAX_STDIN_LEN];                          // User input from stdin
    char start[MAX_STDIN_LEN];
    char end[MAX_STDIN_LEN];
    char *temp_str;
    int line_count;                                     // Line count (a.k.a. movie count) of the file
    int i;
    int j;
    int m;
    int choice;
    int result;
    char **lines;
    struct HashTable *movies;
    struct HashTable *actors;
    struct MapEntry *e;
    struct Movie *movie;
    struct Actor *actor;


    printf("\nPlease enter file path: \n");
    printf("(Example: /home/mert/input-mpaa.txt)\n");

    fscanf(stdin, "%s", path);

    lines = read_file(path, &line_count);               // Read line by line
    m = line_count;                                     // m is the hash table size

    movies = create_hash_table(m);
    actors = create_hash_table(m);

    build_hash_tables(lines, line_count, movies, actors);

    do
    {
        printf("Please enter your operation type: \n");
        printf("1. Find Bacon Number (Distance of an actor to Kevin Bacon)\n");
        printf("2. Find Distance (Distance of two actors)\n");

        scanf("%s", input);
        choice = strtol(input, &temp_str, NUMBER_BASE);
        getchar();

        printf("Please enter valid input\n");
        printf("Example: Bacon, Kevin\n");

        if (choice == 1)
        {
            printf("Please enter an actor name: \n");
            scanf("%[^\n]s", start);
            getchar();

            result = find_bacon_number(start, movies, actors);

            if (result == -1)
            {
                printf("Invalid input(s) or no connection\n");
            }
            else
            {
                printf("Bacon Number: %d\n", result);
            }
        }
        else if (choice == 2)
        {
            printf("Please enter first actor name: \n");
            scanf("%[^\n]s", start);
            getchar();

            printf("Please enter second actor name: \n");
            scanf("%[^\n]s", end);
            getchar();

            result = find_distance(start, end, movies, actors);

            if (result == -1)
            {
                printf("Invalid input(s) or no connection\n");
            }
            else
            {
                printf("Distance: %d\n", result);
            }
        }
        else
        {
            printf("Invalid choice\n");
        }
    } while (choice == 0);

    /** Free hash tables */
    for (i = 0; i < m; i++)
    {
        e = movies[i].head;

        while (e != NULL)
        {
            movie =  e->value;
            for (j = 0; j < movie->actor_count; j++)
            {
                free(movie->actors[j]);
            }

            free(movie->actors);
            free(movie);

            e = e->next;
        }

        e = actors[i].head;

        while (e != NULL)
        {
            actor = e->value;

            for (j = 0; j < actor->movie_count; j++)
            {
                free(actor->movies[j]);
            }

            free(actor->movies);
            free(actor);

            e = e->next;
        }
    }

    free(movies);
    free(actors);

    /** Free lines */
    for (i = 0; i < line_count; i++)
    {
        free(lines[i]);
    }

    free(lines);

    return 0;
}


/**
 * @function read_file
 *
 * @brief Read lines from file
 *
 * @discussion
 * <p>This function reads lines and returns a pointer to array of lines.
 *
 * @param path is the file path
 * @param line_count is the file's line count
 * @return array of string (lines)
 */
char **read_file(char *path, int *line_count)
{
    FILE *fptr;
    int j;
    int tmp;
    char **lines;
    char line[BUFFER_SIZE];

    fptr = fopen(path, "r");
    *line_count = 0;

    while (!feof(fptr))
    {
        tmp = fgetc(fptr);
        if (tmp == '\n')
        {
            (*line_count)++;
        }
    }

    rewind(fptr);
    (*line_count)++;

    lines = malloc(*line_count * sizeof(char*));
    for (j = 0; j < *line_count; j++)
    {
        lines[j] = malloc(BUFFER_SIZE * sizeof(char));
    }

    j = 0;

    while (fgets(line, BUFFER_SIZE- 1, fptr) != NULL)
    {
        int i = 0;
        while ((line[i] != '\n')) i++;
        memcpy(lines[j++], line, i);
    }

    fclose(fptr);

    return lines;
}


/**
 * @function parse_line
 *
 * @brief Tokenize a string
 *
 * @discussion
 * <p>This function tokenize a line and returns a pointer to tokenized string array.
 *
 * @param line is the string to tokenize
 * @return array of string (tokens)
 */
char **parse_line(char *line)
{
    int buffer_size;
    int i;
    char **tokens;
    char *token;

    buffer_size = 64;
    i = 0;

    tokens = malloc(buffer_size * sizeof(char*));

    /* Memory allocation control */
    if (tokens == NULL)
    {
        fprintf(stderr, "Token allocation error\n");
        exit(EXIT_FAILURE);
    }

    /* Get first token */
    token = strtok(line, TOKEN_DELIMITER);

    while (token != NULL)
    {
        tokens[i] = token;
        i++;

        if (i >= buffer_size)
        {
            /* Reallocation of the tokens array */
            buffer_size += 64;
            tokens = realloc(tokens, buffer_size * sizeof(char*));

            /* Memory allocation control */
            if (!tokens)
            {
                fprintf(stderr, "Token allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        /* Read next token */
        token = strtok(NULL, TOKEN_DELIMITER);
    }

    tokens[i] = NULL;

    return tokens;
}


/**
 * @function create_hash_table
 *
 * @brief Initialize a new instance of struct HashTable
 *
 * @discussion
 * <p>This function allocates memory for a new instance of struct HashTable, then initializes
 * fields and returns a pointer to new instance.
 *
 * @param m is the size of the hash table
 * @return pointer to HashTable instance
 */
struct HashTable *create_hash_table(int m)
{
    int i;
    struct HashTable *ht;

    ht = malloc(m * sizeof(struct HashTable));
    ht->table_size = m;

    for (i = 0; i < m; i++)
    {
        ht[i].table_size = m;
        ht[i].head = NULL;
        ht[i].count = 0;
    }

    return ht;
}


/**
 * @function create_movie
 *
 * @brief Initialize a new instance of struct Movie
 *
 * @discussion
 * <p>This function allocates memory for a new instance of struct Movie, then initializes
 * fields and returns a pointer to new instance.
 *
 * @param name is the movie's name
 * @return pointer to Movie instance
 */
struct Movie *create_movie(char *name)
{
    int i;
    struct Movie *m;

    m = malloc(sizeof(struct Movie));

    m->name = name;
    m->actor_count = 1;
    m->actors = malloc(m->actor_count * sizeof(char*));

    for (i = 0; i < m->actor_count; i++)
    {
        m->actors[i] = malloc(MAX_INPUT * sizeof(char));
    }

    m->is_visited = 0;

    return m;
}


/**
 * @function create_actor
 *
 * @brief Initialize a new instance of struct Actor
 *
 * @discussion
 * <p>This function allocates memory for a new instance of struct Actor, then initializes
 * fields and returns a pointer to new instance.
 *
 * @param name is the actor's name
 * @return pointer to Actor instance
 */
struct Actor *create_actor(char *name)
{
    int i;
    struct Actor *a;

    a = (struct Actor*) malloc(sizeof(struct Actor));

    a->name = name;
    a->movie_count = 1;
    a->movies = malloc(a->movie_count * sizeof(char*));

    for (i = 0; i < a->movie_count; i++)
    {
        a->movies[i] = malloc(MAX_INPUT * sizeof(char));
    }

    a->is_visited = 0;
    a->parent_movie_name = malloc(MAX_INPUT * sizeof(char));
    a->parent = NULL;

    return a;
}


/**
 * @function hash
 *
 * @brief Calculates hash of a string
 *
 * @discussion
 * <p>This function calculates hash value of the given string, and returns the hash value.
 *
 * @param str is the string to calculate hash value
 * @return hash value of the given string
 */
long hash(char *str)
{
    long hash_value;
    long p_pow;
    int i;

    const int p = 31;
    const long m = 10000000009;

    hash_value = 0;
    p_pow = 1;

    for (i = 0; i < strlen(str); i++)
    {
        hash_value = (hash_value + (int) str[i] * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }

    return hash_value;
}


/**
 * @function create_map_entry
 *
 * @brief Initialize a new instance of struct MapEntry
 *
 * @discussion
 * <p>This function allocates memory for a new instance of struct MapEntry, then initializes
 * fields and returns a pointer to new instance.
 *
 * @param key is the value to access MapEntry
 * @param value is the pointer to original value to hold
 * @return pointer to MapEntry instance
 */
struct MapEntry *create_map_entry(long key, void *value)
{
    struct MapEntry *new_node;

    new_node = (struct MapEntry*) malloc(sizeof(struct MapEntry));

    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;

    return new_node;
}


/**
 * @function build_hash_tables
 *
 * @brief Initialize Actor and Movie objects and insert them to HashTables
 *
 * @param lines is the array of string
 * @param line_count is the file's line count
 * @param movies is the pointer to movies hash table
 * @param actors is the pointer to actors hash table
 */
void build_hash_tables(char **lines, int line_count, struct HashTable *movies, struct HashTable *actors)
{
    struct Movie *movie;
    struct Actor *actor;
    struct MapEntry *result;
    char **tokens;
    char *current;
    int i;
    int j;

    for (i = 0; i < line_count; i++)
    {
        tokens = parse_line(lines[i]);
        if (tokens[0] == NULL)
        {
            continue;
        }

        /* Create a movie structure with a name */
        movie = create_movie(tokens[0]);
        j = 1;
        current = tokens[j];

        /* Traverse every actor */
        while (current != NULL)
        {
            strcpy(movie->actors[j-1], current);
            if (j >= movie->actor_count-1)
            {
                movie->actor_count += 1;
                movie->actors = realloc(movie->actors, movie->actor_count * sizeof(char*));
                movie->actors[movie->actor_count - 1] = malloc(MAX_INPUT * sizeof(char));
            }
            current = tokens[++j];
        }

        movie->actor_count -= 1;
        movie->actors = realloc(movie->actors, movie->actor_count * sizeof(char*));

        /* Creating movie completed. Insert to hashtable */
        insert(movies, hash(movie->name), movie);

        /* Inserting into actors hashtable */
        for (j = 0; j < movie->actor_count; j++)
        {
            result = search(actors, hash(movie->actors[j]));

            if (result != NULL)
            {
                actor = result->value;
                actor->movie_count++;
                actor->movies = realloc(actor->movies, actor->movie_count * sizeof(char*));
                actor->movies[actor->movie_count - 1] = malloc(MAX_INPUT * sizeof(char));
                strcpy(actor->movies[actor->movie_count -1], movie->name);
            }
            else
            {
                actor = create_actor(movie->actors[j]);
                insert(actors, hash(actor->name), actor);
                actor->movies[actor->movie_count - 1] = malloc(MAX_INPUT * sizeof(char));
                strcpy(actor->movies[actor->movie_count - 1], movie->name);
            }
        }

        free(tokens);
    }
}


/**
 * @function insert
 *
 * @brief Insert element to hash table
 *
 * @param ht is the hash table to operate on
 * @param key is the hash value
 * @param value is the pointer to original value
 */
void insert(struct HashTable *ht, long key, void *value)
{
    long i;
    struct MapEntry *entry;

    if (key < 0)
    {
        key = -key;
    }

    i = key % ht->table_size;
    entry = create_map_entry(key, value);

    if (ht[i].head == NULL)
    {
        ht[i].head = entry;
        ht[i].count = 1;
    }
    else
    {
        entry->next = ht[i].head;
        ht[i].head = entry;
        ht[i].count++;
    }
}


/**
 * @function search
 *
 * @brief Search an element on the hash table
 *
 * @param ht is the hash table to operate on
 * @param key is the hash value of the searched value
 * @return pointer to map entry that has the same key or NULL
 */
struct MapEntry *search(struct HashTable *ht, long key)
{
    long i;
    struct MapEntry *head;
    struct MapEntry *curr;

    if (key < 0)
    {
        key = -key;
    }

    i = key % ht->table_size;
    head = ht[i].head;
    curr = head;

    while ((curr != NULL) && (curr->key != key))
    {
        curr = curr->next;
    }

    return curr;
}


/**
 * @function find_bacon_number
 *
 * @brief Find the distance of an actor to Kevin Bacon
 *
 * @param start is the name of the starting actor
 * @param movies is the movies hash table
 * @param actors is the actors hash table
 * @return distance value
 */
int find_bacon_number(char *start, struct HashTable *movies, struct HashTable *actors)
{
    return find_distance(start, "Bacon, Kevin", movies, actors);
}


/**
 * @function find_distance
 *
 * @brief Find the distance of an actor to another
 *
 * @param start is the name of the starting actor
 * @param end is the name of the ending actor
 * @param movies is the movies hash table
 * @param actors is the actors hash table
 * @return distance value
 */
int find_distance(char *start, char *end, struct HashTable *movies, struct HashTable *actors)
{
    struct Queue *q;
    struct MapEntry *tmp_map_entry;
    struct Actor *s;
    struct Actor *curr_actor;
    struct Actor *next_actor;
    struct Actor *tmp_actor;
    struct Movie *curr_movie;
    struct QueueNode *node;
    char *v;
    int i;
    int j;
    int num;

    if ((search(actors, hash(start)) == NULL) || (search(actors, hash(end)) == NULL))
    {
        printf("Could not found one or two of actors in the table. Please check again.\n");
        return -1;
    }


    q = create_queue();
    enqueue(q, start);

    tmp_map_entry = search(actors, hash(start)); // tmp can not be NULL
    s = tmp_map_entry->value;
    s->is_visited = 1;

    while (q->front != NULL)
    {
        node = (struct QueueNode *) dequeue(q);
        v = node->value;

        if (strcmp(v, end) == 0)
        {
            num = 0;
            curr_actor = search(actors, hash(v))->value;
            next_actor = curr_actor->parent;

            while (curr_actor != NULL)
            {
                if (next_actor != NULL)
                {
                    printf("%s - %s: \"%s\"\n", curr_actor->name, next_actor->name, curr_actor->parent_movie_name);
                    next_actor = next_actor->parent;
                }

                curr_actor = curr_actor->parent;
                num++;
            }

            free(node);

            while (q->front != NULL)
            {
                node = dequeue(q);
                free(node);
            }

            free(q);

            return num - 1;
        }

        curr_actor = search(actors, hash(v))->value;

        /* Search every not searched movie, add not visited actors to queue */
        for (i = 0; i < curr_actor->movie_count; i++)
        {
            curr_movie = search(movies, hash(curr_actor->movies[i]))->value;

            if (curr_movie->is_visited == 0)
            {
                curr_movie->is_visited = 1;

                for (j = 0; j < curr_movie->actor_count; j++)
                {
                    tmp_actor = search(actors, hash(curr_movie->actors[j]))->value;

                    if (tmp_actor->is_visited == 0)
                    {
                        if (tmp_actor->parent == NULL)
                        {
                            tmp_actor->parent = curr_actor;
                            strcpy(tmp_actor->parent_movie_name, curr_movie->name);
                        }

                        enqueue(q, tmp_actor->name);
                    }
                }
            }
        }

        free(node);
    }

    free(q);

    return -1;
}


/**
 * @function create_queue_node
 *
 * @brief creates a new node for queue
 *
 * @discussion
 * <p>Function wraps a value with
 * necessary queue values and returns a pointer to it.
 *
 * @see {@code struct Queue}
 * @see {@code struct QueueNode}
 *
 * @author Mert Turkmenoglu
 *
 * @param value is the pointer to original value
 * @return a pointer to wrapped instance
 */
struct QueueNode* create_queue_node(void* value)
{
    struct QueueNode *temp;

    temp = (struct QueueNode *) malloc(sizeof(struct QueueNode));

    temp->value = value;
    temp->next = NULL;

    return temp;
}


/**
 * @function create_queue
 *
 * @brief creates a struct Queue instance
 *
 * @discussion
 * <p>Function allocates memory for a queue instance then
 * assigns front and rear pointers to NULL.
 *
 * @see {@code struct Queue}
 * @see {@code struct QueueNode}
 *
 * @author Mert Turkmenoglu
 *
 * @return pointer to new created queue instance
 */
struct Queue *create_queue()
{
    struct Queue *q;

    q = (struct Queue*) malloc(sizeof(struct Queue));

    q->front = NULL;
    q->rear = NULL;

    return q;
}


/**
 * @function enqueue
 *
 * @brief Adds new element to queue
 *
 * @discussion
 * <p>Function adds a new element to end of the queue.
 *
 * @see {@code struct Queue}
 * @see {@code struct QueueNode}
 *
 * @author Mert Turkmenoglu
 *
 * @param q is the queue instance to operate on
 * @param value is the pointer to new element to add queue
 */
void enqueue(struct Queue *q, void *value)
{
    struct QueueNode *temp;

    temp = create_queue_node(value);

    if (q->rear == NULL)
    {
        q->front = temp;
        q->rear = temp;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;
}


/**
 * @function dequeue
 *
 * @brief deques element
 *
 * @discussion
 * <p> Function removes the first element of the queue
 * and returns the element. If it is empty, returns NULL.
 *
 * @see {@code struct Queue}
 * @see {@code struct QueueNode}
 *
 * @author Mert Turkmenoglu
 *
 * @param q is the queue instance to operate on
 * @return the first element of the queue
 */
void *dequeue(struct Queue *q)
{
    void *temp;

    if (q->front == NULL)
    {
        return NULL;
    }

    temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    return temp;
}
