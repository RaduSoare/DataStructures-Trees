#ifndef TREEMAP_H_
#define TREEMAP_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))

//-------------------------------------------------------------------------

typedef struct node{
	void* elem; // Node key
	void* info; // Node satellite information
	struct node *pt; // Parent link
	struct node *lt; // left child link
	struct node *rt; // right child link
	struct node* next; // next link in the list of duplicates
	struct node* prev; // prev link in the list of duplicates
	struct node* end; // pointer to the end of the duplicates list
	long height;
}TreeNode;

typedef struct TTree{
	TreeNode *root; // pointer to the root of the tree
	void* (*createElement)(void*); // create element method
	void (*destroyElement)(void*); // destroy element method
	void* (*createInfo)(void*); // create info method
	void (*destroyInfo)(void*); // destroy info method
	int (*compare)(void*, void*); // comparison method between elements
	long size;
}TTree;




TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
		void* (*createInfo)(void*), void (*destroyInfo)(void*),
		int compare(void*, void*))
{
	TTree* tree = (TTree*)malloc(sizeof(TTree));
	if(tree == NULL) exit(-1);
	tree->size = 0;
	tree->root = NULL;
	tree->createElement = createElement;
	tree->destroyElement = destroyElement;
	tree->createInfo = createInfo;
	tree->destroyInfo = destroyInfo;
	tree->compare = compare;
	
	return tree; 


}

int isEmpty(TTree* tree)
{
	if(tree->root == NULL)
		return 1;
	return 0; 
}


TreeNode* search(TTree* tree, TreeNode* x, void* elem)
{
	if(isEmpty(tree)) return NULL;
	TreeNode* tracer = x;
	// parcurge arborele comparand valoarea cautata cu 
	// valoarea din subarborii nodului x
	while(tracer != NULL)
	{
		if(tree->compare(tracer->elem,elem) == 0)
			break;
		if(tree->compare(tracer->elem,elem) == 1 && tracer->lt != NULL)
			tracer = tracer->lt;
		else
			if(tree->compare(tracer->elem,elem) == -1 && tracer->rt != NULL)
				tracer = tracer->rt;
		else 
			break;

	}
	// daca nodul la care s-a ajuns are valoarea cautata atunci este returnat
	// altfel se returneaza NULL
	if(0 != tree->compare(tracer->elem,elem))
		return NULL;
	else
		return tracer; 
	
}


TreeNode* minimum(TreeNode* x)
{
	// se parcurge arborele pana la cel mai din stanga nod
	while(x->lt != NULL)
		x = x->lt;
	return x; 
	
}

TreeNode* maximum(TreeNode* x)
{
	// se parcurge arborele pana la cel mai din dreapta nod
	while(x->rt != NULL)
		x = x->rt;
	return x; 
	
}

TreeNode* successor(TreeNode* x)
{
	if(x == NULL) return NULL;
	// daca are subarbore drept, returneaza minimul din subarborele drept
	if(x->rt != NULL)
		return minimum(x->rt);
	// altfel parcurge arborele pe link-uri de parinti pana cand
	// nodul este copilul stang al unui parinte, pe care il returneaza
	else
	{
		while(x->pt != NULL)
		{
			if(x->pt->lt == x)
				return x->pt;
			x = x->pt;
		}
		
	}
	if(x == NULL)
		return NULL;
}

TreeNode* predecessor(TreeNode* x)
{
	if(x == NULL) return NULL;
	// daca are subarbore stang, returneaza maximul din subarborele stang
	if(x->lt != NULL)
		return maximum(x->lt);
	// altfel parcurge arborele pe link-uri de parinti pana cand
	// nodul este copilul drept al unui parinte, pe care il returneaza
	else
	{
		while(x->pt != NULL)
		{
			if(x->pt->rt == x)
				return x->pt;
			x = x->pt;
		}
	}
	if(x == NULL)
		return NULL;
}

void updateHeight(TreeNode* x)
{
	int leftHeight = 0;
	int rightHeight = 0;
	if(x != NULL)
	{
		if(x->lt != NULL) leftHeight = x->lt->height;
		if(x->rt != NULL) rightHeight = x->rt->height;
		x->height = MAX(leftHeight, rightHeight) + 1;
	}
}

void avlRotateLeft(TTree* tree, TreeNode* x)
{
	if(isEmpty(tree)) return;
	TreeNode* y = x->rt;
	
	if(x == tree->root)
	{
		tree->root = y;
	}
	else 
		if(x == x->pt->lt)
		{
			x->pt->lt = y;
		}
	else
	{
		x->pt->rt = y;
	}
	
	y->pt = x->pt;
	
	if(y->lt)
	{
		x->rt = y->lt;
		x->rt->pt = x;
	}
	else
		x->rt = NULL;

	y->lt = x;
	x->pt = y;

	//fix height
	updateHeight(x);
	updateHeight(y); 

}

void avlRotateRight(TTree* tree, TreeNode* y)
{
	if(isEmpty(tree)) return;
	TreeNode* x = y->lt;

	if(y == tree->root)
	{
		tree->root = x;
	}
	else
		if(y == y->pt->lt)
		{
			y->pt->lt = x;
		}
	else
	{
		y->pt->rt = x;
	}

	x->pt = y->pt;

	if(x->rt)
	{
		y->lt = x->rt;
		y->lt->pt = y;
	}
	else
	{
		y->lt = NULL;
	}

	x->rt = y;
	y->pt = x;

	//fix height
	updateHeight(x);
	updateHeight(y); 



}

/* Get AVL balance factor for node x */
int avlGetBalance(TreeNode *x)
{
	if(x == NULL)
		return 0;
	if(x->rt == NULL && x->lt == NULL)
		return 0;
	if(x->lt == NULL)
		return (-1)* x->rt->height;
	if(x->rt == NULL) 
		return x->lt->height;

	// face diferenta dintre inaltimile copiilor unui nod
	return x->lt->height - x->rt->height;

}


void avlFixUp(TTree* tree, TreeNode* y, void* elem)
{
	// daca root-ul nu are copii, nu e nimic de fixat
	if(tree->root->lt == NULL && tree->root->rt == NULL) return;
	

	while(y != NULL)
	{

		// recalculeaza inaltimile
		updateHeight(y);
		
		// case R-L
		if(avlGetBalance(y) < -1 && tree->compare(y->rt->elem,elem) == 1)
		{
			avlRotateRight(tree,y->rt);
			avlRotateLeft(tree,y);
		} 
		// case R-R
		if(avlGetBalance(y) < -1 && tree->compare(y->rt->elem,elem) == -1)
		{
			
			avlRotateLeft(tree,y);
		}
		// case L-R
		if(avlGetBalance(y) > 1 && tree->compare(y->lt->elem,elem) == -1)
		{
			
			avlRotateLeft(tree,y->lt);
			avlRotateRight(tree,y);
		}
		// case L-L
		if(avlGetBalance(y) > 1 && tree->compare(y->lt->elem,elem) == 1)
		{
			avlRotateRight(tree,y);
		} 
		y = y->pt;
	}  

}


TreeNode* createTreeNode(TTree *tree, void* value, void* info)
{
	// Allocate node
	TreeNode* newNode = (TreeNode*) malloc(sizeof(TreeNode));

	// Set its element field
	newNode->elem = tree->createElement(value);

	//Set its info
	newNode->info = tree->createInfo(info);

	// Set its tree links
	newNode->pt = newNode->rt = newNode->lt = NULL;

	// Set its list links
	newNode->next = newNode->prev = newNode->end = NULL;

	/*
	 *  The height of a new node is 1,
	 *  while the height of NULL is 0
	 */
	newNode->height = 1;

	return newNode;
}

void destroyTreeNode(TTree *tree, TreeNode* node)
{
	// Check arguments
	if(tree == NULL || node == NULL) return;

	// Use object methods to de-alocate node fields
	tree->destroyElement(node->elem);
	tree->destroyInfo(node->info);

	// De-allocate the node
	free(node);
}

void update_links(TreeNode* Tnode, TTree* tree)
{
	//reface legaturile dupa insert\delete
	if(Tnode == NULL) return;

	if(predecessor(Tnode) != NULL)
		Tnode->prev = predecessor(Tnode)->end;
	if(successor(Tnode))
		Tnode->end->next = successor(Tnode); 
	if(Tnode == maximum(tree->root))
		Tnode->end->next = NULL;
	if(Tnode == minimum(tree->root))
		Tnode->prev = NULL;
	

	
	update_links(Tnode->lt,tree);
	update_links(Tnode->rt,tree);
}

void insertList(TTree* tree, void* elem, void* info)
{
	// cauta nodul in arbore
	TreeNode* Tnode = search(tree,tree->root,elem);
	TreeNode* temp = Tnode;
	TreeNode* new_node = createTreeNode(tree,elem,info);
	// cazul in care nodul e primul duplicat
	if(Tnode->next == NULL)
	{
		Tnode->next = new_node;
		new_node->prev = Tnode;
		Tnode->end = new_node;
		return;
	}
	// parcurge lista si insereaza nodul la finalul listei de duplicate
	while(tree->compare(temp->next->elem, Tnode->elem) == 0)
		temp = temp->next;

	temp->next->prev = new_node;
	new_node->next = temp->next;
	temp->next = new_node;
	new_node->prev = temp;
	Tnode->end = new_node;

	


}
void insert(TTree* tree, void* elem, void* info) 
{
	//cazul in care elementul trebuie inserat in arbore
	if(search(tree,tree->root,elem) == NULL)
	{	
		// caz in care arborele e gol
		if(isEmpty(tree))
		{
			TreeNode* new_node = createTreeNode(tree,elem,info);
			tree->root = new_node;
			tree->size++;
			tree->root->end = tree->root;
			return;
		} 
		TreeNode* current = tree->root;
		TreeNode* current_prev = current;
		// cauta locul de inserare
		while(current != NULL)
		{
			current_prev = current;
			if(tree->compare(current->elem,elem) == 1)
				current = current->lt;
			else
				if(tree->compare(current->elem,elem) == -1)
					current = current->rt;
			else
				return;
		}

		TreeNode* new_node = createTreeNode(tree,elem,info);
		new_node->pt = current_prev;
		new_node->end = new_node;
		
		// alege ce fel de copil al nodului o sa fie nodul introdus
		if(tree->compare(current_prev->elem,elem) == 1)
			current_prev->lt = new_node;
		else
			current_prev->rt = new_node;
		
		
		tree->size++;
		avlFixUp(tree,current_prev,new_node->elem);
		update_links(tree->root,tree);
	}
	//cazul in care trebuie inserat in lista
	else
	{
		insertList(tree, elem, info);
		update_links(tree->root,tree);
	}





}
void remove_and_fix(TTree* tree, TreeNode* z)
{
	

	// verifica ce fel de copil a fost nodul sters
	int child_type = 0; //-1 pentru stanga si 1 pentru dreapta
	TreeNode* parent = z->pt;

	if(tree->compare(z->pt->elem,z->elem) == 1)
	{
		z->pt->lt = NULL;
		child_type = -1;
	}
	if(tree->compare(z->pt->elem,z->elem) <= 0)
	{
		z->pt->rt = NULL;
		child_type = 1;
	}
	
	destroyTreeNode(tree,z);
	
	if(child_type == -1 && parent->rt != NULL)
	{
		// daca a fost copil de stanga si cazul de debalansare LR
		if(avlGetBalance(parent) < -1 && 
			tree->compare(parent->rt->elem,minimum(parent->rt)->elem) == 1)
		{
			avlFixUp(tree,parent,minimum(parent->rt)->elem);
		}
		else
			avlFixUp(tree,parent,maximum(parent)->elem);
	}
	
	if(child_type == 1 && parent->lt != NULL)
	{
		// daca a fost copil dreapta si cazul de debalansare RL
		if(avlGetBalance(parent) > 1 && 
			tree->compare(parent->lt->elem,maximum(parent->lt)->elem) == 1)
		{
			avlFixUp(tree,parent,maximum(parent->lt)->elem);
		}
		else
			avlFixUp(tree,parent,minimum(parent)->elem);
	}
}

void delete(TTree* tree, void* elem)
{
	
	// caut elementul de sters
	TreeNode* z = search(tree,tree->root,elem);
	if(z == NULL) return;
	
	// cazul in care nodul trebuie sters doar din arbore
	if(z == z->end)
	{	
		// cazul in care arborele e format doar din root
		if(z == tree->root && z->lt == NULL && z->rt == NULL)
		{
			destroyTreeNode(tree,z);
			tree->root = NULL;
			tree->size = 0;
		}
		// cazul in care elementul e frunza
		else
			if(z->lt == NULL && z->rt == NULL)
		{

			remove_and_fix(tree,z);
			tree->size --;
		
		}
		// cazul in care elementul are ambii copii
		else
			if(z->lt != NULL && z->rt != NULL)
		{
			TreeNode* succ = successor(z);
			memcpy(z->elem, succ->elem, sizeof(void*));
			remove_and_fix(tree,succ);
			tree->size--;
		} 
		// cazul in care elementul are doar copil dreapta
		else
			if(z->lt == NULL && z->rt != NULL)
		{
			TreeNode* succ = z->rt;
			memcpy(z->elem, succ->elem, sizeof(void*));
			remove_and_fix(tree,succ);
			z->rt = NULL;
			updateHeight(z);
			tree->size--;
		}
		//cazul in care elementul are doar copil stanga
		else
			if(z->lt != NULL && z->rt == NULL)
		{
			TreeNode* succ = z->lt;
			memcpy(z->elem, succ->elem, sizeof(void*));
			remove_and_fix(tree,succ);
			z->lt = NULL;
			updateHeight(z);
			tree->size--;
		}
		update_links(tree->root,tree);
	}
	//cazul in care nodul trebuie sters din lista
	else
	{
		//nodul ce o sa fie sters
		TreeNode* last_end = z->end;
		if(z->end->next != NULL)
			z->end->next->prev = z->end->prev;
		z->end = z->end->prev;
		z->end->next = successor(z);
		destroyTreeNode(tree,last_end); 
	}
}

void destroyTree(TTree* tree)
{
	// parcurge lista de noduri si dezaloca fiecare nod
	TreeNode* start = minimum(tree->root);
	TreeNode* temp = NULL;
	while(start != NULL)
	{
		temp = start->next;
		destroyTreeNode(tree,start);
		start = temp;
	}
	free(tree); 
}


#endif /* TREEMAP_H_ */
