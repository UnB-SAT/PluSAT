#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUES 10

typedef struct Tree{
    struct Tree *left, *right;
    int value;
}Tree;
 
void dfs(Tree* root){

    if(root != NULL){
        printf("%d ", root->value);
        dfs(root->left);
        dfs(root->right);
    }
}




void dfsi(Tree *root){

    Tree* stack[MAX_VALUES];
    int stackItem = 0;

    stack[stackItem++] = root;

    Tree *onTop;

    while(stackItem)
    {

        //aqui decide e faz algo

        onTop = stack[stackItem-1];
        stackItem--;

        printf("%d ", onTop->value);


        if(onTop->right != NULL)
            stack[stackItem++] = onTop->right;
        
        if(onTop->left != NULL)
            stack[stackItem++] = onTop->left;


    }


}

Tree* newNode(int value){

    Tree *newNode = (Tree*)malloc(sizeof(Tree));

    newNode->value = value;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;

}

int main(){

    Tree *quatro = newNode(4);
    quatro->right = newNode(6);

    Tree *tres = newNode(3);
    tres->right = newNode(5);
    tres->left = quatro;

    Tree *tree = newNode(1);

    tree->left = newNode(2);
    tree->right = tres;

    clock_t start, end;
   
    start = clock();
    dfs(tree);
    end = clock();
    printf("\n");

    printf("%lf\n", (double)(end - start)/CLOCKS_PER_SEC);

    start = clock();
    dfsi(tree);
    end = clock();
    printf("\n");


    printf("%lf\n", (double)(end - start)/CLOCKS_PER_SEC);


    return 0;
}
