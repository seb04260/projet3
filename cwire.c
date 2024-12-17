#include "cwire.h"

AVLNode *create_node(int station_id, long capacity, long consumption) {
    AVLNode *node = malloc(sizeof(AVLNode));
    node->station_id = station_id;
    node->capacity = capacity;
    node->consumption = consumption;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

int height(AVLNode *node) {
    return node ? node->height : 0;
}

int get_balance(AVLNode *node) {
    return node ? height(node->left) - height(node->right) : 0;
}

AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));

    return x;
}

AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));

    return y;
}

AVLNode *insert_node(AVLNode *node, int station_id, long capacity, long consumption) {
    if (!node) return create_node(station_id, capacity, consumption);

    if (station_id < node->station_id)
        node->left = insert_node(node->left, station_id, capacity, consumption);
    else if (station_id > node->station_id)
        node->right = insert_node(node->right, station_id, capacity, consumption);
    else {
        node->consumption += consumption;
        return node;
    }

    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));

    int balance = get_balance(node);

    if (balance > 1 && station_id < node->left->station_id)
        return rotate_right(node);

    if (balance < -1 && station_id > node->right->station_id)
        return rotate_left(node);

    if (balance > 1 && station_id > node->left->station_id) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    if (balance < -1 && station_id < node->right->station_id) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

void free_tree(AVLNode *node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

void save_in_order(AVLNode *root, FILE *output) {
    if (root) {
        save_in_order(root->left, output);
        fprintf(output, "%d:%ld:%ld\n", root->station_id, root->capacity, root->consumption);
        save_in_order(root->right, output);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <input_csv>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    FILE *file = fopen(input_file, "r");
    if (!file) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier %s.\n", input_file);
        return 1;
    }

    AVLNode *root = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *token;
        int col = 0;
        int station_id = 0, lv_station = 0, company = 0;
        long capacity = 0, load = 0;
        printf("Découpage de la ligne : %s\n", line);
        token = strtok(line, ";");
        while (token != NULL) {
            printf("Colonne %d : %s\n", col, token);
            switch (col) {
                case 0: station_id = atoi(token); break;
                case 3: lv_station = atoi(token); break;
                case 4: company = atoi(token); break;
                case 6: 
                    if (strcmp(token, "-") != 0) {
                        capacity = atol(token); 
                    } else {
                        capacity = 0;
                    }
                    break;
                case 7: 
                    load = atol(token);
                    break;             
            }
            col++;
            token = strtok(NULL, ";");
        }

        if (lv_station > 0 && load > 0) {
            printf("Insertion : LV Station: %d, Capacity: %ld, Load: %ld\n", lv_station, capacity, load);
            root = insert_node(root, lv_station, capacity, load);
        }
    }

    fclose(file);

    FILE *output = fopen("output/result.csv", "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        free_tree(root);
        return 1;
    }
    save_in_order(root, output);
    fclose(output);

    free_tree(root);

    return 0;
}