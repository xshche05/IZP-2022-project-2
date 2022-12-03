/**
 * 2. projekt IZP 2022/23
 * Autor: Kirill Shchetiniuk,
 * Login: xshche05
 * Datum: 04-12-2022
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

#define ERR_INPUT_ARGS 101          // Kod chyby pro chybny pocet argumentu/chybne argumenty
#define ERR_INPUT_FILE 102          // Kod chyby pro chybu pri cteni ze souboru
#define ERR_INPUT_OBJECTS 103       // Kod chyby pro chybu pri cteni objektu ze souboru
#define ERR_NULL_POINTER 104        // Kod chyby pro chybu pri praci s NULL ukazatelem
#define ERR_ALLOC 105               // Kod chyby pro chybu pri alokaci pameti
#define ERR_FUNC_ARG 106            // Kod chyby pro chybu pri praci s argumenty funkce
#define ERR_INTERNAL 1              // Kod chyby pro interni chybu programu

/**
 * @brief Funkce pro vypis chyboveho hlaseni
 *
 * @param code - kod chyby
 * @param msg - chybova zprava
 * @param line - cislo radku kodu, kde doslo k chybe
 *
 * @return zaporna hodnota kodu chyby
 */
int raise_error(int code, char *msg, int line) {
    fprintf(stderr, "CODE LINE %d: %s (code - %d)\n", line, msg, code); // Vypise chybovou hlasku na stderr
    return -code; // Vrati zaporny kod chyby
}

/**
 * @brief Funkce pro alokaci pameti pro pole
 *
 * @param num - pocet prvku pole
 * @param size - velikost prvku pole
 *
 * @return ukaazatel na alokovanou pamet, jinak NULL a vytiskne chybovou hlasku
 */
void *my_calloc(size_t num, size_t size) { // Vlastne funkce pro alokaci pameti
    void *ptr;
    ptr = calloc(num, size);
    if (ptr == NULL) {
        raise_error(ERR_ALLOC, "Allocation error", __LINE__);
    }
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

/**
 * @brief Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @param c - Shluk, ktery se ma inicializovat
 * @param cap - Kapacita shluku
 *
 * @return Vraci 0 pri uspechu, jinak vraci zaporny kod chyby
 */
int init_cluster(struct cluster_t *c, int cap) {
    if (c == NULL) // Kontrola ukazatele
        return raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
//    assert(c != NULL);
    if (cap < 0) // Kontrola kapacity
        return raise_error(ERR_FUNC_ARG, "Function argument isnt acceptable", __LINE__);
//    assert(cap >= 0);
    if (cap == 0) { // Pokud je kapacita 0, tak alokuje 0 pameti
        c->capacity = 0;
        c->obj = NULL;
    } else { // Jinak alokuje pamet pro cap objektu
        c->capacity = cap;
        c->obj = my_calloc(c->capacity, sizeof(struct obj_t));
        if (c->obj == NULL)
            return raise_error(ERR_ALLOC, "calloc() failed", __LINE__);
    }
    c->size = 0; // Nastavi velikost na 0
    return 0; // Vrati 0
}

/**
 * @brief Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 *
 * @param c - Ukazatel na shluk
 *
 * @return int - Vrati 0 pri uspechu, jinak zaporny kod chyby
 */
int clear_cluster(struct cluster_t *c) {
    if (c == NULL) { // Kontrola ukazatele
        return raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
    }
//    assert(c != NULL);
    free(c->obj); // Uvolni pamet
    c->obj = NULL; // Nastavi ukazatel na NULL
    return init_cluster(c, 0); // Inicializuje shluk s kapacitou 0
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
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

    c->obj = (struct obj_t *) arr;
    c->capacity = new_cap;
    return c;
}

/**
 * @brief Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 * nevejde.
 *
 * @param c - ukazatel na shluk, do ktereho se ma objekt pridat
 * @param obj - objekt, ktery se ma pridat
 *
 * @return int - 0 pri uspechu, jinak zaporny kod chyby
 */
int append_cluster(struct cluster_t *c, struct obj_t obj) {
    if (c == NULL) // Kontrola ukazatele
        return raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
//    assert(c != NULL);
    if (c->size >= c->capacity) // Pokud je velikost >= kapacite, tak zvetsi shluk
        if (resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL) // Pokud se nepodari zvetsit shluk, tak vraci chybu
            return raise_error(ERR_ALLOC, "Reallocation failed", __LINE__);
    c->obj[c->size] = obj; // Prida objekt na konec shluku
    c->size++; // Zvetsi velikost shluku
    return 0;
}

/**
 * @brief Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 *
 * @param c - Ukazatel na shluk, ktery se ma seradit.
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 *
 * @param c1 - Ukazatel na shluk, do ktereho se pridavaji objekty
 * @param c2 - Ukazatel na shluk, ze ktereho se pridavaji objekty
 *
 * @return Vraci 0 pri uspechu, jinak vraci zaporny kod chyby
 */
int merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
    if (c1 == NULL || c2 == NULL) // Kontrola ukazatele
        return raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
//    assert(c1 != NULL);
//    assert(c2 != NULL);

    sort_cluster(c1); // Seradi shluk c1
    for (int i = 0; i < c2->size; i++) { // Projde vsechny objekty v c2
        if (append_cluster(c1, c2->obj[i]) != 0) // Pokud se nepodari pridat objekt, tak vraci chybu
            return raise_error(ERR_INTERNAL, "Internal error", __LINE__);
    }
    return 0;
}

/**********************************************************************/
/* Prace s polem shluku */

/**
 * @brief Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 *
 * @param carr - ukazatel na pole shluku
 * @param narr - pocet shluku v poli
 * @param idx - index shluku pro odstraneni
 *
 * @return Vraci novy pocet shluku v poli, jinak vraci zaporny kod chyby
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    if (idx >= narr || idx < 0 || narr < 0) // Kontrola indexu
        return raise_error(ERR_FUNC_ARG, "Function argument isnt acceptable", __LINE__);
//    assert(idx < narr);
//    assert(narr > 0);
    if (clear_cluster(&carr[idx]) != 0) // Pokud se nepodari uvolnit pamet, tak vraci chybu
        return raise_error(ERR_INTERNAL, "Internal error", __LINE__);
    for (int i = idx; i < narr - 1; i++) { // Posune vsechny shluky o jedno doleva
        carr[i] = carr[i + 1];
    }
    return narr - 1;
}

/**
 * @brief Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @param o1 - Prvni objekt.
 * @param o2 - Druhy objekt.
 *
 * @return Vzdalenost mezi objekty. V pripade chyby vraci -1.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2) {
    if (o1 == NULL || o2 == NULL) { // Kontrola ukazatele
        raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
        return -1;
    }
//    assert(o1 != NULL);
//    assert(o2 != NULL);

    float x1 = o1->x;
    float y1 = o1->y;
    float x2 = o2->x;
    float y2 = o2->y;
    float distance = sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2)); // Vypocet vzdalenosti
    return distance; // Vraci vzdalenost
}

/**
 * @brief Pocita vzdalenost dvou shluku.
 *
 * @param c1 - Prvni shluk.
 * @param c2 - Druhy shluk.
 *
 * @return Vzdalenost mezi objekty. V pripade chyby vraci -1.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2) {
    if (c1 == NULL || c2 == NULL) { // Kontrola ukazatele
        raise_error(ERR_NULL_POINTER, "pointer is NULL", __LINE__);
        return -1;
    }
    if (c1->size <= 0 || c2->size <= 0) { // Kontrola velikosti shluku
        raise_error(ERR_FUNC_ARG, "Function argument isnt acceptable", __LINE__);
        return -1;
    }

//    assert(c1 != NULL);
//    assert(c1->size > 0);
//    assert(c2 != NULL);
//    assert(c2->size > 0);

    float min = INFINITY; // Nastavi min na nekonecno
    for (int i = 0; i < c1->size; i++) {
        for (int j = 0; j < c2->size; j++) {
            float dist = obj_distance(&c1->obj[i], &c2->obj[j]); // Vypocita vzdalenost mezi objekty
            if (dist < 0) // Pokud se nepodari vypocitat vzdalenost, tak vraci chybu
            {
                raise_error(ERR_INTERNAL, "Internal error", __LINE__);
                return -1;
            }
            if (dist < min) {
                min = dist;
            }
        }
    }
    return min;
}

/**
 * @brief Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 * hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 * 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 * adresu 'c1' resp. 'c2'.
 *
 * @param carr - ukazatel na pole shluku
 * @param narr - pocet shluku v poli
 * @param c1 - ukazatel na index prvniho nejblizsiho shluku
 * @param c2 - ukazatel na index druheho nejblizsiho shluku
 */
int find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    if (narr <= 0) // Kontrola velikosti pole
        return raise_error(ERR_FUNC_ARG, "Function argument isnt acceptable", __LINE__);
//    assert(narr > 0);

    float min = INFINITY; // Nastavi min na nekonecno
    for (int i = 0; i < narr; i++) {
        for (int j = i + 1; j < narr; j++) {
            float dist = cluster_distance(&carr[i], &carr[j]); // Vypocita vzdalenost mezi shluky
            if (dist < 0) // Pokud se nepodari vypocitat vzdalenost, tak vraci chybu
                return raise_error(ERR_INTERNAL, "Internal error", __LINE__);
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
static int obj_sort_compar(const void *a, const void *b) {
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *) a;
    const struct obj_t *o2 = (const struct obj_t *) b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c) {
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
  * @brief Tisk shluku 'c' na stdout.
  *
  * @param c - ukazatel na shluk, který se má vytisknout.
  */
void print_cluster(struct cluster_t *c) {
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++) {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * @brief Overeni id objektu
 *
 * @param arr - pole shluku (SHLUKY S 1 OBJ!!)
 * @param size - velikost pole shluku
 * @param id - id pro overeni
 *
 * @return 0 - neni unikatni, 1 - je unikatni
 */
int check_unique_id(struct cluster_t *arr, int size, int id) {
    for (int i = 0; i < size; i++) {
        if (arr[i].obj->id == id) {
            return 0; // Neni unikatni
        }
    }
    return 1; // Je unikatni
}

/**
 * @brief Funkce pro dealokaci pameti
 *
 * @param arr - Ukazaetel na pole shluku
 * @param n - Poce shluku v poli
 *
 * Funkce dealokuje pamet pro vsechny shluky v poli a pro kazdy shluk dealokuje a nastavi NULL u ukazatele na pole shluku
 */
void deallocate_clusters(struct cluster_t **arr, int n) {
    for (int i = 0; i < n; i++)
        clear_cluster(&(*arr)[i]); // Dealokuje pamet pro kazdy shluk
    free((*arr)); // Dealokuje pamet pro pole shluku
    *arr = NULL;
}

/**
 * @brief Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 * V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 *
 * @param filename - jmeno souboru
 * @param arr - ukazatel na pole shluku
 *
 * @return pocet nactenych objektu (shluku) nebo zaporny chybovy kod
 */
int load_clusters(char *filename, struct cluster_t **arr) {
    if (arr == NULL) // Kontrola ukazatele
        return raise_error(ERR_NULL_POINTER, "Pointer is NULL", __LINE__);
//    assert(arr != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL) { // Kontrola otevreni souboru
        return raise_error(ERR_INPUT_FILE, "File could not be opened.", __LINE__);
    }
    char buffer[102]; // Buffer pro nacitani radku
    fgets(buffer, 102, file); // Nacte prvni radek s poctem objektu
    char *endPt = NULL; // Ukazatel na konec nacteneho cisla
    endPt = strchr(buffer, '='); // Nalezne znak '='
    if (endPt == NULL) { // Pokud se nepodari najit znak '=', vraci chybu
        fclose(file);
        return raise_error(ERR_INPUT_FILE, "File is not in the correct format. First line should be count=N", __LINE__);
    }
    int count = strtol(endPt + 1, &endPt, 10); // Nacte cislo za znakem '=' a ulozi do promenne count
    if (count <= 0) { // Pokud se nepodari nacist cislo, vraci chybu
        fclose(file);
        return raise_error(ERR_INPUT_FILE, "File is not in the correct format. Count < 0", __LINE__);
    }
    if (*endPt != '\0' && *endPt != '\n') { // Pokud se na radku nachazi neco jineho nez cislo, vraci chybu
        fclose(file);
        return raise_error(ERR_INPUT_FILE, "File is not in the correct format. Sth is after count=N", __LINE__);
    }
    *arr = my_calloc(count, sizeof(struct cluster_t));
    int i = 0;
    while (fgets(buffer, 100, file) != NULL && i < count) {
        int id;
        float x, y;
        id = strtol(buffer, &endPt, 10);
        if (*endPt != ' ') {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INPUT_FILE,
                               "File is not in the correct format. ID is not followed by space or is not a int number",
                               __LINE__);
        }
        x = (float) strtol(endPt, &endPt, 10);
        if (*endPt != ' ') {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INPUT_FILE,
                               "File is not in the correct format. X is not followed by space or is not a int number",
                               __LINE__);
        }
        y = (float) strtol(endPt, &endPt, 10);
        if (*endPt != '\0' && *endPt != '\n') {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INPUT_OBJECTS,
                               "File is not in the correct format. Something is after OBJ in line or OBJ format is incorrect",
                               __LINE__);
        }
        if (id < 0 || x < 0 || y < 0 || x > 1000 || y > 1000) {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INPUT_OBJECTS, "File is not in the correct format. OBJ params are out of range.",
                               __LINE__);
        }
        if (!check_unique_id(*arr, i, id)) {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INPUT_OBJECTS, "File is not in the correct format. OBJ ID is not unique.", __LINE__);
        }
        struct obj_t obj = {id, x, y};
        if (init_cluster(&(*arr)[i], 1) != 0) {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INTERNAL, "Internal error", __LINE__);
        }
        if (append_cluster(&(*arr)[i], obj) != 0) {
            deallocate_clusters(&(*arr), i);
            return raise_error(ERR_INTERNAL, "Internal error", __LINE__);
        }
        i++;
    }
    fclose(file);
    if (i < count) {
        deallocate_clusters(&(*arr), i);
        return raise_error(ERR_INPUT_OBJECTS, "File is not in the correct format. Not enough objects.", __LINE__);
    }
    return count;
}

/**
 * @brief Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk). Tiskne se prvnich 'narr' shluku.
 *
 * @param carr - Ukazatel na prvni shluk v poli shluku.
 * @param narr - Pocet shluku k tisku.
*/
void print_clusters(struct cluster_t *carr, int narr) {
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++) {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/**
 * @brief Funkce pro parsrovani argumentu
 *
 * @param argc - Pocet argumentu
 * @param argv - Pole argumentu
 * @param n - Ukazatel na pocet shluku
 * @param filename - Ukazatel na nazev souboru
 *
 * @return 0 pokud vse probehlo v poradku, jinak -ERR_INPUT_ARGS
 */
int parse_args(int argc, char *argv[], int *n, char **filename) {
    char *endPt = NULL;
    if (argc > 1)
        *filename = argv[1];
    if (argc == 2)
        *n = 1;
    else if (argc == 3) {
        *n = strtol(argv[2], &endPt, 10);
        if (*endPt != '\0' || *n < 1) {
            return raise_error(ERR_INPUT_ARGS, "Invalid N argument.", __LINE__);
        }
    } else {
        return raise_error(ERR_INPUT_ARGS, "Invalid arguments", __LINE__);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct cluster_t *clusters = NULL;
    char *filename = NULL;
    int cluster_amount;
    if (parse_args(argc, argv, &cluster_amount, &filename) != 0)
        return -raise_error(ERR_INTERNAL, "Internal error", __LINE__);
    int current_cluster_amount = load_clusters(filename, &clusters);
    if (clusters == NULL)
        return -raise_error(ERR_INTERNAL, "Internal error", __LINE__);
    if (cluster_amount > current_cluster_amount)
        return -raise_error(ERR_INPUT_ARGS, "N is bigger than number of objects.", __LINE__);
    while (cluster_amount < current_cluster_amount) {
        int c1, c2;
        if (find_neighbours(clusters, current_cluster_amount, &c1, &c2) != 0) {
            deallocate_clusters(&clusters, current_cluster_amount);
            return -raise_error(ERR_INTERNAL, "Internal error", __LINE__);
        }
        if (merge_clusters(&clusters[c1], &clusters[c2]) != 0) {
            deallocate_clusters(&clusters, current_cluster_amount);
            return -raise_error(ERR_INTERNAL, "Internal error", __LINE__);
        }
        sort_cluster(&clusters[c1]);
        current_cluster_amount = remove_cluster(clusters, current_cluster_amount, c2);
        if (current_cluster_amount < 0) {
            deallocate_clusters(&clusters, current_cluster_amount);
            return -raise_error(ERR_INTERNAL, "Internal error", __LINE__);
        }
    }
    print_clusters(clusters, current_cluster_amount);
    deallocate_clusters(&clusters, current_cluster_amount);
    return 0;
}
