typedef struct Node{
    const char* key;
    void* data;
    struct Node* next;
} Node;

typedef struct HashMap{
    Node** map;
    size_t key_space;
} HashMap;

HashMap* create_hashmap (size_t key_space);

unsigned int hash (const char* key);

void insert_data (HashMap* hm, const char* key, void* data, void*(*resolve_collision)(void* old_data, void* new_data));

void* get_data (HashMap* hm, const char* key);

void iterate (HashMap* hm, void(*callback)(const char* key, void* data));

void remove_data (HashMap* hm, const char* key, void(*destroy_data)(void* data)); 

void delete_hashmap (HashMap* hm, void(*destroy_data)(void* data));

