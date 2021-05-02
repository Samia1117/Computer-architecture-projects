# include <stdio.h>
# include <stdlib.h>
# include <string.h>

struct node{
    char name[20];
    struct node* left;
    struct node* right;
};

void traverse_and_free(struct node* head){
    if(head ==NULL){
        return;
    }
    if (head->left != NULL){
        traverse_and_free(head->left);
    }
    if(head-> right != NULL){
        traverse_and_free(head->right);
    }
    free(head);
}

void dfs_and_print(struct node* head){
    if(head ==NULL){
        return;
    }
    printf("%s\n", head->name);
    if (head->left !=NULL){
        dfs_and_print(head->left);
    }
    if(head->right !=NULL){
        dfs_and_print(head->right);
    }
    
}

struct node* traverse_and_find(struct node* root, char* student_name){
    if(root == NULL){
        return NULL;
    }
    if (strcmp(root->name, student_name) ==0){
        return root;
    }
    struct node* left_infector = traverse_and_find(root->left, student_name);
    if (left_infector == NULL){
        return traverse_and_find(root->right, student_name);
    }else{
        return left_infector;
    }
    return NULL;
    
}

int main(int argc, char* argv[]){
    char* filename = argv[1]; // filename is a character array
    FILE* fp;

    if (argc<2){ //check file argument was given
        //printf("missing filename\n");
        return 0;
    }else{
        filename = argv[1];
    }

    fp = fopen (filename, "r"); // fopen returns a file pointer 
    struct node* head = NULL;

    char name1[30];
    char name2[30];

    while(1){
        int scanfVal = fscanf(fp, "%s %s", name1, name2);
        if(scanfVal<2){
            break;
        }
        struct node* infector = (struct node*)malloc(sizeof(struct node));
        struct node* infected = (struct node*)malloc(sizeof(struct node));
        strcpy (infected->name, name1);
        
        if(head == NULL){
            strcpy (infector->name, name2);
            infector->left = infected;
            head = infector;
            printf("First time, infected is: %s\n", infected);
            printf("First time, infector is: %s\n", infector);
        }else{
            infector = traverse_and_find(head, name2);
            if (infector == NULL){
                //printf("Got unexpected NULL result from traversing down root looking for infector\n");
                break;}
            if(infector->left == NULL){
                infector->left = infected;
            }else{
                infector->right = infected;
            }
        }
        dfs_and_print(head);
    }
    dfs_and_print(head);
    traverse_and_free(head);
    return 0;
}