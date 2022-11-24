/**
 * 2. projekt IZP 2022/23
 * Autor: Kirill Shchetiniuk,
 * Login: xshche05
 * Datum: 2022-11-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*
 * Error codes
 */

#define ERR_INPUT_ARGS 101
#define ERR_INPUT_FILE 102
#define ERR_INPUT_OBJECTS 103
#define ERR_NULL_POINTER 104
#define ERR_ALLOC 105
#define ERR_FUNC_ARG 106
#define ERR_INTERNAL 1

int err_exit(int code, char *msg)
{
    fprintf(stderr, "%s\n", msg);
    return -code;
}

void* my_calloc(size_t num, size_t size) {
#ifdef CHECK_ALLOC
    int i = rand() % 50;
    if (i == 0) {
        return NULL;
    }
#endif
    void *ptr = calloc(num, size);
    if (ptr == NULL)
        err_exit(ERR_ALLOC, "Error: calloc() failed");
    return ptr;
}

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
int init_cluster(struct cluster_t *c, int cap) {
    if (c == NULL)
        return err_exit(ERR_NULL_POINTER, "Error: pointer is NULL");
    assert(c != NULL);
    if (cap < 0)
        return err_exit(ERR_FUNC_ARG, "Error: Function argument isnt acceptable");
    assert(cap >= 0);
    if (c->obj == NULL && cap == 0) {
        c->capacity = 0;
        c->obj = NULL;
    } else {
        c->capacity = cap;
        c->obj = my_calloc(c->capacity, sizeof(struct obj_t));
        if (c->obj == NULL)
            return err_exit(ERR_ALLOC, "Error: calloc() failed");
    }
    c->size = 0;
    return 0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
int clear_cluster(struct cluster_t *c)
{
    if (c == NULL) {
        return err_exit(ERR_NULL_POINTER, "Error: pointer is NULL");
    }
    assert(c != NULL);
    c->size = 0;
    c->capacity = 0;
    free(c->obj);
    c->obj = NULL;
    return init_cluster(c, c->capacity);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
int append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c == NULL)
        return err_exit(ERR_NULL_POINTER, "Error: pointer is NULL");
    assert(c != NULL);
    if (c->size >= c->capacity)
        if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL)
            return err_exit(ERR_ALLOC, "Error: Reallocation failed");
    c->obj[c->size] = obj;
    c->size++;
    return 0;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
int merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    if (c1 == NULL || c2 == NULL)
        return err_exit(ERR_NULL_POINTER, "Error: pointer is NULL");
    assert(c1 != NULL);
    assert(c2 != NULL);

    sort_cluster(c1);
    for (int i = 0; i < c2->size; i++) {
        if (append_cluster(c1, c2->obj[i]) != 0)
            return err_exit(ERR_INTERNAL, "Error: Internal error");
    }
    return 0;
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    if (idx >= narr || idx < 0 || narr < 0)
        return err_exit(ERR_FUNC_ARG, "Error: Function argument isnt acceptable");
    assert(idx < narr);
    assert(narr > 0);
    if (clear_cluster(&carr[idx]) != 0)
        return err_exit(ERR_INTERNAL, "Error: Internal error");
    for (int i = idx; i < narr - 1; i++) {
        carr[i] = carr[i + 1];
    }
    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float x1 = o1->x;
    float y1 = o1->y;
    float x2 = o2->x;
    float y2 = o2->y;
    float distance = sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
    return distance;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float min = INFINITY;
    for (int i = 0; i < c1->size; i++) {
        for (int j = 0; j < c2->size; j++) {
            float dist = obj_distance(&c1->obj[i], &c2->obj[j]);
            if (dist < min) {
                min = dist;
            }
        }
    }
    return min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
int find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    if (narr <= 0)
        return err_exit(ERR_FUNC_ARG, "Error: Function argument isnt acceptable");
    assert(narr > 0);

    float min = INFINITY;
    for (int i = 0; i < narr; i++) {
        for (int j = i + 1; j < narr; j++) {
            float dist = cluster_distance(&carr[i], &carr[j]);
            if (dist < min) {
                min = dist;
                *c1 = i;
                *c2 = j;
            }
        }
    }
    return 0;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

int check_unique_id(struct cluster_t *arr, int size, int id)
{
    for (int i = 0; i < size; i++) {
        if (arr[i].obj->id == id) {
            return 0;
        }
    }
    return 1;
}

void deallocate_clusters(struct cluster_t *arr, int n)
{
    for (int i = 0; i < n; i++)
        clear_cluster(&arr[i]);
    free(arr);
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    if (arr == NULL)
        return err_exit(ERR_NULL_POINTER, "Error: Pointer is NULL");
    assert(arr != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return err_exit(ERR_INPUT_FILE, "Error: File could not be opened.");
    }
    char buffer[102];
    fgets(buffer, 102, file);
    char *endPt = NULL;
    endPt = strchr(buffer, '=');
    if (endPt == NULL) {
        fclose(file);
        return err_exit(ERR_INPUT_FILE, "Error: File is not in the correct format. First line should be count=N");
    }
    int count = strtol(endPt+1, &endPt, 10);
    if (count <= 0) {
        fclose(file);
        return err_exit(ERR_INPUT_FILE, "Error: File is not in the correct format. Count < 0");
    }
    if (*endPt != '\0' && *endPt != '\n') {
        fclose(file);
        return err_exit(ERR_INPUT_FILE, "Error: File is not in the correct format. Sth is after count=N");
    }
    *arr = my_calloc(count, sizeof(struct cluster_t));
    int i = 0;
    while (fgets(buffer, 100, file) != NULL && i < count)
    {
        int id;
        float x, y;
        id = strtol(buffer, &endPt, 10);
        x = (float) strtol(endPt, &endPt, 10);
        y = (float) strtol(endPt, &endPt, 10);
        if (id < 0 || x < 0 || y < 0 || x > 1000 || y > 1000) {
            deallocate_clusters((*arr), i);
            return err_exit(ERR_INPUT_OBJECTS, "Error: File is not in the correct format. OBJ params are out of range.");
        }
        if (*endPt != '\0' && *endPt != '\n') {
            deallocate_clusters((*arr), i);
            return err_exit(ERR_INPUT_OBJECTS, "Error: File is not in the correct format. Sth is after OBJ in line");
        }
        if (!check_unique_id(*arr, i, id)) {
            deallocate_clusters((*arr), i);
            return err_exit(ERR_INPUT_OBJECTS, "Error: File is not in the correct format. OBJ ID is not unique.");
        }
        struct obj_t obj = {id, x, y};
        if (init_cluster(&(*arr)[i], 1) != 0) {
            deallocate_clusters((*arr), i);
            return err_exit(ERR_INTERNAL, "Error: Internal error");
        }
        if (append_cluster(&(*arr)[i], obj) != 0) {
            deallocate_clusters((*arr), i);
            return err_exit(ERR_INTERNAL, "Error: Internal error");
        }
        i++;
    }
    fclose(file);
    if (i < count) {
        deallocate_clusters((*arr), i);
        return err_exit(ERR_INPUT_OBJECTS, "Error: File is not in the correct format. Not enough objects.");
    }
    return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
 Parsuje argumenty aplikace.
 */
int parse_args(int argc, char *argv[], int *n, char **filename)
{
    char *endPt = NULL;
    if (argc > 1)
        *filename = argv[1];
    if (argc == 2)
        *n = 1;
    else if (argc == 3)
    {
        *n = strtol(argv[2], &endPt, 10);
        if (*endPt != '\0' || *n < 1)
            return err_exit(ERR_INPUT_ARGS, "Error: Invalid N argument.");
    }
    else
        return err_exit(ERR_INPUT_ARGS, "Error: Invalid arguments");
    return 0;
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters = NULL;
    char *filename = NULL;
    int cluster_amount;
    int check_code;
    check_code = parse_args(argc, argv, &cluster_amount, &filename);
    if (check_code != 0)
        return -check_code;
    int current_cluster_amount = load_clusters(filename, &clusters);
    if (current_cluster_amount < 0)
        return -current_cluster_amount;
    while (cluster_amount < current_cluster_amount)
    {
        int c1, c2;
        if (find_neighbours(clusters, current_cluster_amount, &c1, &c2) != 0) {
            deallocate_clusters(clusters, current_cluster_amount);
            return err_exit(ERR_INTERNAL, "Error: Internal error");
        }
        if (merge_clusters(&clusters[c1], &clusters[c2]) != 0) {
            deallocate_clusters(clusters, current_cluster_amount);
            return err_exit(ERR_INTERNAL, "Error: Internal error");
        }
        sort_cluster(&clusters[c1]);
        current_cluster_amount = remove_cluster(clusters, current_cluster_amount, c2);
        if (current_cluster_amount < 0) {
            deallocate_clusters(clusters, current_cluster_amount);
            return err_exit(ERR_INTERNAL, "Error: Internal error");
        }
    }
    print_clusters(clusters, current_cluster_amount);
    deallocate_clusters(clusters, current_cluster_amount);
    return 0;
}
