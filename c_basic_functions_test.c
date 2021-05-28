# include<stdio.h>

struct node {
    char myName[20];
    struct node* left;
    struct node* right;   // doubly linked list
};

struct node* build_linkedlist(struct node* head, char* names){


    return NULL;

}

/*struct node* dfs(struct node* currentNode, char* target){
    char* currentName = currentNode->myName;
    if (str_cmp(target, currentName) ==0){
        printf("found it!");
        if (currentNode->left != NULL){
            currentNode->left = struct node* 
            {
                
            };
            
        }
    }
    return 0;
}*/

void str_cpy(char* from, char* to){ // assumes name2 has enough space to accommodate all chars in name1
    char* save_at = to; // first value of to = address where first character of from is to be saved
    int j = 0;
    while(from[0] != '\0'){
        *to = *from; // address value at 
        to ++; from ++;
        j ++;
        if (j>10){
            break;
        }
    }
    printf("j was: %d\n", j);
}

int str_cmp(char name1[], char name2[]){
    int i = 0;
    while (name1[i] != '\0' || name2[i] != '\0'){
        //printf("Same character!\n");
        if (name1[i]!= name2[i]){
            if (name1[i]>name2[i]){
                return 1;
            }
            return -1;
        }
        i++;
    }
    return 0;
}

int main(int argc, char* argv[]){
    /*char* name1 = "ab";
    char* name2 = "aa";
    char* name3 = "ba";
    //printf("starting dfs!\n");
    int result = str_cmp(name1, name2);
    printf("the result is: %d\n", result);
    int result2 = str_cmp(name1, name3);
    printf("the result2 is: %d\n", result2);*/

    /*char* filename = argv[1]; // filename is a character array
    FILE* fp;

    fp = fopen (filename, "r"); // fopen returns a file pointer 
    struct node* head = NULL;
    char name1[20];
    char name2[20];

    while(1){
        int scanfVal = fscanf(fp, "%s %s", name1, name2);
        if(scanfVal<2){
            break;
        }
    */

    char name1[10] = "Samia";
    char name2[10];
    str_cpy(name1, name2);
    printf("name1: %s\n", name1);
    printf("name2: %s\n", name2);
    return 0;
}