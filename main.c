#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define T_NIL (&t_nil)
#define get_num(character) ((character) == '-' ? 0 : \
                            ((character) == '_'? 1 : \
                            ((character) >= '0' && (character) <= '9') ? (2 + (character) - '0') : \
                            (((character) >= 'A' && (character) <= 'Z') ? (12 + (character) - 'A') : \
                            (((character) >= 'a' && (character) <= 'z') ? (38 + (character) - 'a') : -1))))
#define get_ascii(index) ((index) == 0 ? 45 : \
                            ((index) == 1 ? 95 : \
                            ((index) >= 2 && (index) <= 11) ? (46 + (index)) : \
                            ((index) >= 12 && (index) <= 37) ? (53 + (index)) : \
                            ((index) >= 38 && (index) <= 63) ? (59 + (index)) : -1))
#define TRUE (1)
#define FALSE (0)

enum Color {red, black};

// RB tree structure
typedef struct tree {
    char *word;
    enum Color color; // 'false' red, 'true' black
    struct tree *left, *right, *parent;
} tree;

struct tree t_nil;
struct tree *global = T_NIL; //puntatore all'albero delle parole
struct tree *w_ammissibili = T_NIL; //albero delle parole ammissibili
struct tree **nodes_to_delete = NULL; //array di puntatori ai nodi da eliminare

int global_size = 0;
int game_size = 0;
int w_len; // lunghezza delle parole
int array_dim = 0; //dimensione array
int array_idx = 0; //num parole da eliminare dall'albero
bool match;
bool primo_giro;

// Utility functions
struct tree* new_node(char* word, bool mem);
struct tree* insert(tree* root, tree* new);
struct tree* insert_fix(tree* root, tree* node);
struct tree* successor(tree* node);
struct tree* right_rotation(tree* root, tree* node);
struct tree* left_rotation(tree* root, tree* node);
void inorder(tree* root);
int search(tree* root, char word[]);
struct tree* delete(tree* root, tree* node);
struct tree* delete_fix(tree* root, tree* node);
struct tree* delete_nodes(tree *root);
struct tree* compare(tree* w_ammissibili, char* reference, char word[], int mtx1[4][64],
                     char mtx2[w_len+1][w_len]);
int filter_1(int val, struct tree *root, bool primo_giro);
int filter_2(int val, int idx, struct tree* root, bool primo_giro);
int filter_3(int val, int idx, struct tree* root, bool primo_giro);
int filter_4(int val, int num_occorrenze_trovate, struct tree* root, bool primo_giro);
int filter_4bis(int val, int num_occorrenze_trovate, struct tree* root, bool primo_giro);
struct tree* free_space_game(tree* root);
int previous_filter(tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]);
struct tree* filter_primogiro(tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]);
void filters(tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]);
int string_compare(const char *p0, const char *p1, int w_size);
struct tree** create_array_of_nodes_to_delete();
void add_node_to_array(struct tree* node);
// End utility functions

//
int main() {

    t_nil.color = black;
    t_nil.parent = NULL;
    t_nil.right = NULL;
    t_nil.left = NULL;

    int w_num = 0; //numero di parole confrontabili in una partita prima che finisca
    char *reference = NULL;
    char buffer[w_len+18];
    char c;

    bool done = FALSE;
    match = FALSE;

    if (scanf("%d%c", &w_len, &c) == 0) {
        done = TRUE;
    }

    int mtx1[4][64]; //prima matrice; 1=occurrences fisse; 2=num pos giuste trovate in totale ; 3=occurrences per calcolare / o |; 4=0ccorrenze di word(parola che cambia);
    // 5=num pos giuste trovate per ogni parola; 6=carattere controllato o no; 7 per 4bis

    char mtx2[w_len+1][w_len];

    //alloco spazio per la variabile in cui viene salvata la parola di riferimento e per la variabile in cui vengono salvati i vincoli
    reference = (char*) malloc((w_len+1) * sizeof(char));

    //lettura e inserimento nell'albero delle parole ammissibili
    while (!done) {
        if (fgets(buffer, w_len + 18, stdin) == NULL) {
            done = TRUE;
            break;
        }
        if (buffer[0] != '+') {
            buffer[w_len] = 0;
        } else {
            buffer[strcspn(buffer, "\r\n")] = 0;
        }
        if (strcmp(buffer, "+nuova_partita") == 0) {
            //w_ammissibili = copy(global, T_NIL);
            //fixParent(w_ammissibili);
            //game_size = global_size;

            match = FALSE;
            primo_giro = TRUE;

            //0. resetto le occurrences del vettore della parola di riferimento
            memset(mtx1, 0, sizeof(mtx1));

            memset(mtx2, '?', sizeof(mtx2));

            //1. leggo la parola di riferimento
            if (fgets(buffer, w_len + 18, stdin) == NULL) {
                done = TRUE;
                break;
            }
            if (buffer[0] != '+') {
                buffer[w_len] = 0;
            } else {
                buffer[strcspn(buffer, "\r\n")] = 0;
            }
            strcpy(reference, buffer);

            //2. leggo il numero di parole che si possono confrontare in questa partita
            if (scanf("%d%c", &w_num, &c) == 0) {
                done = TRUE;
            }

            //3. Nella partita faccio i confronti, aggiungo nuove parole e stampo le filtrate
            while (w_num > 0 && !match && !done) {

                if (fgets(buffer, w_len + 18, stdin) == NULL) {
                    done = TRUE;
                    break;
                }
                if (buffer[0] != '+') {
                    buffer[w_len] = 0;
                } else {
                    buffer[strcspn(buffer, "\r\n")] = 0;
                }

                if (strcmp(buffer, "+inserisci_inizio") == 0) {
                    while (strcmp(buffer, "+inserisci_fine") != 0) {
                        if (fgets(buffer, w_len + 18, stdin) == NULL) {
                            done = TRUE;
                            break;
                        }
                        if (buffer[0] != '+') {
                            buffer[w_len] = '\0';

                            tree *new = new_node(buffer, TRUE);

                            global = insert(global, new);
                            global_size++;

                            if (!primo_giro) {
                                int enter = previous_filter(new, reference, mtx1, mtx2);
                                if (enter == TRUE) {
                                    w_ammissibili = insert(w_ammissibili, new_node(new->word, FALSE));
                                    game_size++;
                                }
                            }
                        } else {
                            buffer[strcspn(buffer, "\r\n")] = 0;
                        }
                    }
                    if (array_dim < game_size) {
                        nodes_to_delete = create_array_of_nodes_to_delete();
                    }
                } else if (strcmp(buffer, "+stampa_filtrate") == 0) {
                    if (game_size == 0 && primo_giro) {
                        inorder(global);
                    } else {
                        inorder(w_ammissibili);
                    }
                } else {
                    bool temp = FALSE;
                    temp = search(global, buffer);
                    if (temp == FALSE) {
                        printf("not_exists\n");
                    } else {
                        w_ammissibili = compare(w_ammissibili, reference, buffer, mtx1, mtx2);
                        if (!match) {
                            w_num--;
                        }
                    }
                }
            }
            if (w_num == 0 && !match) {
                printf("ko\n");
            }
            w_ammissibili = free_space_game(w_ammissibili);
            game_size = 0;
        } else if (strcmp(buffer, "+inserisci_inizio") == 0) {
            while (strcmp(buffer, "+inserisci_fine") != 0) {
                if (fgets(buffer, w_len + 18, stdin) == NULL) {
                    done = TRUE;
                    break;
                }
                if (buffer[0] != '+') {
                    buffer[w_len] = '\0';
                    global = insert(global, new_node(buffer, TRUE));
                    global_size++;
                } else {
                    buffer[strcspn(buffer, "\r\n")] = 0;
                }
            }
        } else if (strcmp(buffer, "+stampa_filtrate") == 0) {
            inorder(global);
        } else {
            if (buffer[0] != '+') {
                global = insert(global, new_node(buffer, TRUE));
                global_size++;
            }
        }
    }

    return 0;
}

//Versione 2 con matrici
struct tree* compare(struct tree *w_ammissibili, char *reference, char word[], int mtx1[4][64],
                     char mtx2[w_len+1][w_len]) {
    int i;
    int z;
    //int ascii_value;
    //int ascii_value2;
    char output[w_len];
    int num;
    int num2;
    int p_num = 0; //numero di + nella parola
    bool ce;
    int local_mtx1[3][64];

    memset(local_mtx1, 0, sizeof(local_mtx1));



    for (i=0; i<w_len; i++) {
        output[i] = '#';
        //ascii_value = ;
        num = get_num((int)(reference[i]));
        //ascii_value2 = (int)(word[i]);
        num2 = get_num((int)(word[i]));
        local_mtx1[0][num]++;
        local_mtx1[2][num2]++;
        //mtx1[2][num]++;
        //mtx1[4][num2]++; //occurrences di word
        if (primo_giro) {
            mtx1[0][num]++;
        }
        if (reference[i] == word[i]) {
            output[i] = '+';
            if (mtx2[0][i] != '+') {
                if (mtx1[1][num] < mtx1[0][num] && mtx1[1][num] < local_mtx1[2][num]) {
                    mtx1[1][num]++; //numero di lettere di ascii "num" trovate in assoluto durante la partita
                }
                mtx2[0][i] = '+';
            }
            local_mtx1[1][num]++; //numero di lettere di ascii "num" trovate in pos giusta di word
            p_num++; //numero di +
        }
    }

    //Match with the right word
    if (p_num == w_len) {
        printf("ok\n"); //termina la partita
        match = TRUE;
        return w_ammissibili;
    }

    //Output and filters
    for (i=0; i<w_len; i++) {
        num = get_num((int)(word[i])); //posizione in matrice di ascii_value
        if (output[i] != '+') {
            if ((local_mtx1[0][num] - local_mtx1[1][num]) > 0) {
                if ((mtx1[1][num] < mtx1[0][num]) && (mtx1[1][num] < local_mtx1[2][num])) {
                    mtx1[1][num]++;
                }
                local_mtx1[0][num]--;
                output[i] = '|';
                ce = FALSE;
                z = 1; //indice per trovare la prima riga z alla colonna i in cui scrivere il vincolo
                while (mtx2[z][i] != '?' && z < w_len + 1) {
                    if (mtx2[z][i] == word[i]) {
                        ce = TRUE;
                        break;
                    }
                    z++;
                }
                if (!ce) {
                    mtx2[z][i] = word[i];
                }
            } else if (mtx1[0][num] == 0) {
                output[i] = '/';
                if (mtx1[3][num] == 0) {
                    mtx1[3][num] = 1; // carattere controllato e non ci deve essere
                }
            } else {
                output[i] = '/';
                ce = FALSE;
                z = 1; //indice per trovare la prima riga z alla colonna i in cui scrivere il vincolo
                while (mtx2[z][i] != '?' && z < w_len + 1) {
                    if (mtx2[z][i] == word[i]) {
                        ce = TRUE;
                        break;
                    }
                    z++;
                }
                if (!ce) {
                    mtx2[z][i] = word[i];
                }
                mtx1[1][num] = mtx1[0][num];
                mtx1[2][num] = 1; // posso usare il 4bis
            }
        } else if (mtx1[0][num] > 0 && mtx1[3][num] != 2) { //mtx1[3][num] = 0 : non controllato
            mtx1[3][num] = 2; // carattere controllato e ci deve essere in corrispondenza dei + e del numero esatto
        }
        printf("%c", output[i]);
    }

    if (primo_giro) {
        w_ammissibili = filter_primogiro(global, reference, mtx1, mtx2);
        nodes_to_delete = create_array_of_nodes_to_delete();
    } else if (game_size > 1) {
        filters(w_ammissibili, reference, mtx1, mtx2);
        w_ammissibili = delete_nodes(w_ammissibili);
    }

    printf("\n%d\n", game_size);

    primo_giro = FALSE;

    return w_ammissibili;
}

//Versione 2 inserimento a partita in corsa
int previous_filter(struct tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]) {

    if (root != T_NIL && root != NULL) {

        for (int i=0; i<w_len; i++) {
            int z = 1;
            while (mtx2[z][i] != '?' && z < w_len+1) {
                //int ascii_num = ; //ascii
                int value = get_num(mtx2[z][i]); //posizione in mtx1
                if (mtx1[0][value] > 0) {
                    if (filter_3(mtx2[z][i], i, root, TRUE)) {
                        return FALSE;
                    }
                    if (mtx1[0][value] == mtx1[1][value] && mtx1[2][value] == 1) {
                        if (filter_4bis(mtx2[z][i], mtx1[1][value], root, TRUE)) {
                            return FALSE;
                        }
                    } else {
                        if (filter_4(mtx2[z][i], mtx1[1][value], root, TRUE)) {
                            return FALSE;
                        }
                    }
                }
                z++;
            }
            //int ascii = ;
            int num = get_num((int) reference[i]);
            if (mtx1[0][num] > 0 && mtx1[3][num] == 2 && mtx2[0][i] == '+') {
                if (filter_2((int) reference[i], i, root, TRUE)) {
                    return FALSE;
                }
                if (mtx1[0][num] == mtx1[1][num] && mtx1[2][num] == 1) {
                    if (filter_4bis((int) reference[i], mtx1[1][num], root, TRUE)) {
                        return FALSE;
                    }
                } else {
                    if (filter_4((int) reference[i], mtx1[1][num], root, TRUE)) {
                        return FALSE;
                    }
                }
            }
        }

        for (int i=0; i<64; i++) {
            if (mtx1[0][i] == 0 && mtx1[3][i] == 1) {
                if (filter_1(get_ascii(i), root, TRUE)) {
                    return FALSE;
                }
            }
        }

        return TRUE;
    } else {
        return FALSE;
    }
}

//Versione 3
void filters(struct tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]) {
    bool stop = FALSE;

    if (root != T_NIL && root != NULL) {

        if (root->left != T_NIL && root->left != NULL) {
            filters(root->left, reference, mtx1, mtx2);
        }

        if (string_compare(root->word, reference, w_len) != 0) {

            for (int i = 0; i < w_len; i++) {
                if (!stop) {
                    int z = 1;
                    while (mtx2[z][i] != '?') {
                        //int ascii_num = ; //ascii del carattere in mtx2
                        int value = get_num(mtx2[z][i]); //posizione in mtx1
                        if (mtx1[0][value] > 0) {
                            if (filter_3(mtx2[z][i], i, root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                            if (mtx1[0][value] == mtx1[1][value] && mtx1[2][value] == 1) {
                                if (filter_4bis(mtx2[z][i], mtx1[1][value], root, primo_giro)) {
                                    stop = TRUE;
                                    break;
                                }
                            } else {
                                if (filter_4(mtx2[z][i], mtx1[1][value], root, primo_giro)) {
                                    stop = TRUE;
                                    break;
                                }
                            }
                        }
                        z++;
                    }
                    //int ascii = (int) reference[i];
                    int num = get_num((int) reference[i]);
                    if (mtx1[0][num] > 0 && mtx1[3][num] == 2 && mtx2[0][i] == '+' && !stop) {
                        if (filter_2((int) reference[i], i, root, primo_giro)) {
                            stop = TRUE;
                            break;
                        }
                        if (mtx1[0][num] == mtx1[1][num] && mtx1[2][num] == 1) {
                            if (filter_4bis((int) reference[i], mtx1[1][num], root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        } else {
                            if (filter_4((int) reference[i], mtx1[1][num], root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        }
                    }
                } else {
                    break;
                }
            }

            if (!stop) {
                for (int i = 0; i < 64; i++) {
                    if (!stop) {
                        if (mtx1[0][i] == 0 && mtx1[3][i] == 1) {
                            //int ascii = get_ascii(i);
                            if (filter_1(get_ascii(i), root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }

        }

        if (root->right != T_NIL && root->right != NULL) {
            filters(root->right, reference, mtx1, mtx2);
        }
    } else {
        return;
    }
}

//
struct tree* filter_primogiro(struct tree* root, char *reference, int mtx1[4][64], char mtx2[w_len+1][w_len]) {
    bool stop = FALSE;

    if (root != T_NIL && root != NULL) {

        if (root->left != T_NIL && root->left != NULL) {
            w_ammissibili = filter_primogiro(root->left, reference, mtx1, mtx2);
        }

        if (string_compare(root->word, reference, w_len) != 0) {

            for (int i = 0; i < w_len; i++) {
                if (!stop) {
                    int z = 1;
                    while (mtx2[z][i] != '?') {
                        //int ascii_num = ; //ascii del carattere in mtx2
                        int value = get_num(mtx2[z][i]); //posizione in mtx1
                        if (mtx1[0][value] > 0) {
                            if (filter_3(mtx2[z][i], i, root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                            if (mtx1[0][value] == mtx1[1][value] && mtx1[2][value] == 1) {
                                if (filter_4bis(mtx2[z][i], mtx1[1][value], root, primo_giro)) {
                                    stop = TRUE;
                                    break;
                                }
                            } else {
                                if (filter_4(mtx2[z][i], mtx1[1][value], root, primo_giro)) {
                                    stop = TRUE;
                                    break;
                                }
                            }
                        }
                        z++;
                    }
                    //int ascii = (int) reference[i];
                    int num = get_num((int) reference[i]);
                    if (mtx1[0][num] > 0 && mtx1[3][num] == 2 && mtx2[0][i] == '+' && !stop) {
                        if (filter_2((int) reference[i], i, root, primo_giro)) {
                            stop = TRUE;
                            break;
                        }
                        if (mtx1[0][num] == mtx1[1][num] && mtx1[2][num] == 1) {
                            if (filter_4bis((int) reference[i], mtx1[1][num], root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        } else {
                            if (filter_4((int) reference[i], mtx1[1][num], root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        }
                    }
                } else {
                    break;
                }
            }

            if (!stop) {
                for (int i = 0; i < 64; i++) {
                    if (!stop) {
                        if (mtx1[0][i] == 0 && mtx1[3][i] == 1) {
                            //int ascii = get_ascii(i);
                            if (filter_1(get_ascii(i), root, primo_giro)) {
                                stop = TRUE;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }

            if (!stop) {
                //struct tree *new_word2 = new_node(root->word, FALSE);
                w_ammissibili = insert(w_ammissibili, new_node(root->word, FALSE));
                game_size++;
            }
        } else {
            //struct tree *new_word2 = new_node(root->word, FALSE);
            w_ammissibili = insert(w_ammissibili, new_node(root->word, FALSE));
            game_size++;
        }

        if (root->right != T_NIL && root->right != NULL) {
            w_ammissibili = filter_primogiro(root->right, reference, mtx1, mtx2);
        }

        return w_ammissibili;
    } else {
        return T_NIL;
    }
}

// Delete words with the character val from the tree
int filter_1(int val, struct tree *root, bool primo) {

    if (root != T_NIL && root != NULL) {

        for (int i=0; i < w_len; i++) {
            if (root->word[i] == val) {
                if (!primo) {
                    add_node_to_array(root);
                }
                return TRUE;
            }
        }

        return FALSE;

    } else {
        return FALSE;
    }

}

// Delete words without the character val in the index idx from the tree
int filter_2(int val, int idx, struct tree* root, bool primo) {

    if (root != T_NIL && root != NULL) {

        if (root->word[idx] != val) {
            if (!primo) {
                add_node_to_array(root);
            }
            return TRUE;
        }

        return FALSE;
    } else {
        return FALSE;
    }

}

// Delete words with the character val in the index idx from the tree
int filter_3(int val, int idx, struct tree* root, bool primo) {

    if (root != T_NIL && root != NULL) {

        if (root->word[idx] == val) {
            if (!primo) {
                add_node_to_array(root);
            }
            return TRUE;
        }

        return FALSE;
    } else {
        return FALSE;
    }

}

// Delete words with the character val in a different num of the reference from the tree
int filter_4(int val, int num_occorrenze_trovate, struct tree* root, bool primo) {
    int count = 0;

    if (root != T_NIL && root != NULL) {

        for (int i=0; i<w_len; i++) {
            if (root->word[i] == val) {
                count++;
            }
        }
        if (count < num_occorrenze_trovate) {
            if (!primo) {
                add_node_to_array(root);
            }
            return TRUE;
        }

        return FALSE;
    } else {
        return FALSE;
    }

}

// Delete words with the character val in a different num of the reference from the tree
int filter_4bis(int val, int num_occorrenze_trovate, struct tree* root, bool primo) {
    int count = 0;

    if (root != T_NIL && root != NULL) {

        for (int i=0; i<w_len; i++) {
            if (root->word[i] == val) {
                count++;
            }
        }
        if (count != num_occorrenze_trovate) {
            if (!primo) {
                add_node_to_array(root);
            }
            return TRUE;
        }

        return FALSE;
    } else {
        return FALSE;
    }

}

//Delete nodes from root
struct tree* delete_nodes(struct tree *root) {

    for (int i=array_idx-1; i>=0; i--) {
        root = delete(root, nodes_to_delete[i]);
    }
    game_size = game_size - array_idx;

    array_idx = 0;

    return root;
}

//
struct tree** create_array_of_nodes_to_delete() {
    if (nodes_to_delete == NULL) {
        array_dim = game_size;
        nodes_to_delete = (struct tree**) malloc( array_dim * sizeof(struct node*));
    } else if (array_dim < game_size && array_idx == 0) {
        free(nodes_to_delete);
        array_dim = game_size;
        nodes_to_delete = (struct tree**) malloc(array_dim * sizeof(struct node*));
    }

    return nodes_to_delete;
}

//
void add_node_to_array(struct tree* node) {
    nodes_to_delete[array_idx] = node;
    array_idx++;
}

//true=global, false=w_ammissibili
struct tree* new_node(char* word, bool mem) {
    struct tree *node;

    if (mem) {
        node = (struct tree*) malloc(sizeof(struct tree)+(w_len+1));
        node->word = (char *) node + sizeof(struct tree);
        strcpy(node->word, word);
    } else {
        node = (struct tree*) malloc(sizeof(struct tree));
        node->word = word;
    }

    return node;
}

//
struct tree* right_rotation(struct tree* root, struct tree* node) {
    struct tree* x;

    x = node->left;
    node->left = x->right;

    if (x->right != T_NIL) {
        x->right->parent = node;
    }
    x->parent = node->parent;

    if (node->parent == T_NIL) {
        root = x;
    } else if (node == node->parent->right) {
        node->parent->right = x;
    } else {
        node->parent->left = x;
    }

    x->right = node;
    node->parent = x;

    return root;
}

//
struct tree* left_rotation(struct tree* root, struct tree* node) {
    struct tree *x;

    x = node->right;
    node->right = x->left;

    if (x->left != T_NIL) {
        x->left->parent = node;
    }
    x->parent = node->parent;

    if (node->parent == T_NIL) {
        root = x;
    } else if (node == node->parent->left) {
        node->parent->left = x;
    } else {
        node->parent->right = x;
    }

    x->left = node;
    node->parent = x;

    return root;
}

//
struct tree* insert(struct tree* root, struct tree* new) {
    struct tree *a, *d;

    d = T_NIL;
    a = root;

    while (a != T_NIL) {
        d = a;
        int x = string_compare(new->word, a->word, w_len);
        if (x < 0) {
            a = a->left;
        } else if (x > 0) {
            a = a->right;
        } else {
            return root;
        }
    }

    new->parent = d;

    if (d == T_NIL) {
        root = new;
    } else if (string_compare(new->word, d->word, w_len) < 0) {
        d->left = new;
    } else {
        d->right = new;
    }

    new->left = new->right = T_NIL;
    new->color = red;

    root = insert_fix(root, new);

    return root;
}

//
struct tree* insert_fix(struct tree* root, struct tree* node) {
    struct tree *x;

    while (node->parent->color == red) {
        if (node->parent == node->parent->parent->left) {
            x = node->parent->parent->right;
            if (x->color == red) {
                node->parent->color = black;
                x->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    root = left_rotation(root, node);
                }
                node->parent->color = black;
                node->parent->parent->color = red;
                root = right_rotation(root, node->parent->parent);
            }
        } else {
            x = node->parent->parent->left;
            if (x->color == red) {
                node->parent->color = black;
                x->color = black;
                node->parent->parent->color = red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    root = right_rotation(root, node);
                }
                node->parent->color = black;
                node->parent->parent->color = red;
                root = left_rotation(root, node->parent->parent);
            }
        }
    }

    root->color = black;

    return root;
}

// Returns the successor of node in the tree.
struct tree* successor(struct tree* node) {
    struct tree *a;
    struct tree *d = NULL;

    if (node->right != T_NIL) {
        for (a = node->right; a != T_NIL && a != NULL; a = a->left) {
            d = a;
        }
        return d;
    }
    d = node->parent;
    while (d != NULL && d != T_NIL && node == d->right) {
        node = d;
        d = d->parent;
    }
    return d;
}

// Return T_NIL if the word is not found
int search(struct tree *root, char word[]) {
    struct tree *temp = root;

    while (temp != T_NIL) {
        int x = string_compare(word, temp->word, w_len);
        if (x == 0) {
            return TRUE;
        }
        temp = (x < 0) ? temp->left : temp->right;
    }

    return FALSE;
}

//
struct tree* delete(struct tree* root, struct tree* node) {
    struct tree *a = T_NIL;
    struct tree *d = T_NIL;

    if (node->left == T_NIL || node->right == T_NIL) {
        d = node;
    } else {
        d = successor(node);
    }

    if (d->left != T_NIL) {
        a = d->left;
    } else {
        a = d->right;
    }

    a->parent = d->parent;

    if (d->parent == T_NIL) {
        root = a;
    } else if (d == d->parent->left){
        d->parent->left = a;
    } else {
        d->parent->right = a;
    }

    if (d != node) {
        node->word = d->word;
    }

    if (d->color == black) {
        root = delete_fix(root, a);
    }

    free(d);

    d = NULL;

    return root;
}

// Removes a node, keeping the properties
struct tree* delete_fix(struct tree* root, struct tree* node) {
    struct tree *a;

    while (node != root && node->color == black) {
        if (node == node->parent->left) {
            a = node->parent->right;
            if (a->color == red) {
                a->color = black;
                node->parent->color = red;
                root = left_rotation(root, node->parent);
                a = node->parent->right;
            }
            if (a->left->color == black && a->right->color == black) {
                a->color = red;
                node = node->parent;
            } else {
                if (a->right->color == black) {
                    a->left->color = black;
                    a->color = red;
                    root = right_rotation(root, a);
                    a = node->parent->right;
                }
                a->color = node->parent->color;
                node->parent->color = black;
                a->right->color = black;
                root = left_rotation(root, node->parent);
                node = root;
            }
        } else {
            a = node->parent->left;
            if (a->color == red) {
                a->color = black;
                node->parent->color = red;
                root = right_rotation(root, node->parent);
                a = node->parent->left;
            }
            if (a->right->color == black && a->left->color == black) {
                a->color = red;
                node = node->parent;
            } else {
                if (a->left->color == black) {
                    a->right->color = black;
                    a->color = red;
                    root = left_rotation(root, a);
                    a = node->parent->left;
                }
                a->color = node->parent->color;
                node->parent->color = black;
                a->left->color = black;
                root = right_rotation(root, node->parent);
                node = root;
            }
        }
    }

    node->color = black;

    return root;
}

// Print in lexicographic order the words in the tree
void inorder(struct tree* root) {
    if (root != T_NIL && root != NULL) {
        inorder(root->left);
        printf("%s\n", root->word);
        inorder(root->right);
    }
}

//Faster strcmp
int string_compare(const char *p0, const char *p1, int w_size) {
    int i;
    for (i = 0; i < w_size; i++) {
        if (p0[i] != p1[i]) {
            return p0[i] - p1[i];
        }
    }
    return 0;
}

//
struct tree* free_space_game(struct tree* root) {
    if (root != T_NIL && root != NULL) {
        free_space_game(root->left);
        free_space_game(root->right);
        free(root);
    }
    return T_NIL;
}