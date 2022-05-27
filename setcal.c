/***********
Samuel Lencses & Vojtech Adamek
Matrix file calulator
2021
***********/
#define _GNU_SOURCE


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define R 10
#define F 21

char forbid[][F] = {
		"true",
		"false",
		"empty",
		"card",
		"complement",
		"union",
		"intersect",
		"minus",
		"subset",
		"subseteq",
		"equals",
		"reflexive",
		"symmetric",
		"antisymmetric",
		"transitive",
		"function",
		"domain",
		"codomain",
		"injective",
		"surjective",
		"bijective"
	};

/* Defining structures */
typedef struct {
	char **elms;
	int word_count;
} uni_t;

typedef struct {
	int *elms_idx;
	int word_count;
} set_t;

typedef struct {
	int sets_count;
	set_t *sets;
} msets_t;

typedef struct {
	bool is_rel;
	int operand;
	char *operation;
} un_oper;

typedef struct {
	int operand1;
	int operand2;
	char *operation;
} bin_oper; 

typedef struct {
	int set_a;
	int set_b;
	int rel;
	char *operation;
} ter_oper; 

typedef struct {
    int **pairs;
    int pair_count;
} rel_t;

typedef struct {
	rel_t *rels;
	int rels_count;
} mrels_t;

int count_pairs_count(char *str){
    int l_brackets = 0;
    int r_brackets = 0;

    for (unsigned int i = 0; str[i] != '\0'; i++){
        if (str[i] == '('){
            l_brackets++;
        }
        else if (str[i] == ')'){
            r_brackets++;
        }
    }

    if (l_brackets != r_brackets){
        return -1;
    }

	return l_brackets;
}

bool is_pair_unique(rel_t *rel){
	int same;

	for(int i = 0; i < rel->pair_count; i++){
		same = 0;
		for(int j= 0; j < rel->pair_count; j++){
			if(rel->pairs[i][0] == rel->pairs[j][0] &&
			   rel->pairs[i][1] == rel->pairs[j][1]) same++;

			if(same == 2) return false;
		}
		
	}

	return true;
}

void error(int code){
	switch(code){
		case 1: 
			fprintf(stderr, "Not allowed whitespaces\n");
			exit(1);

		case 2: 
			fprintf(stderr, "Universum not defined\n");
			exit(1);
	}
}

/* Swaps values of two variables */
void swap(int *a, int *b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

bool alpha_check(char str[]){
    for (unsigned int i = 0; i < strlen(str); i++)
        if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z'))
            return false;

    return true;
}

/* Sorting elements in a set with seleciton sort */
void my_sort(set_t *set){ 
	int min_idx, i, j;

	for (i = 0; i < set->word_count - 1; i++){
		min_idx = i;

		//finding min index	
		for(j = i + 1; j < set->word_count; j++)
			if(set->elms_idx[j] < set->elms_idx[i])
				min_idx = j;

		swap(&set->elms_idx[min_idx], &set->elms_idx[i]);
	}
}

bool is_unique(set_t *set){
	for(int i  = 0; i < set->word_count; i++){
		int same = 0;

		for(int j = 0; j < set->word_count; j++){
			if(set->elms_idx[i] == set->elms_idx[j]){
				same++;
				if(same == 2) return false;
			}
		}
	}

	return true;
}

/* Calculating number of words in a string */
int count_word_count(char str[]){
	int words = 0;
	bool in_word = false;
	char c = str[0];

	for (int i = 0; str[i] != '\0';c = str[++i]){

		if (c != ' ' && c != '\n'  && in_word == false){
			words++;
			in_word = true;
		}

		else if (c == ' ') in_word = false;
	}

	return words - 1;
}

				/* UNIVERSUM */
/*********************************************/

/* Universum constructor */
void uni_ctor(uni_t *universum, char *str){
	universum->word_count = count_word_count(str);
	universum->elms = malloc(universum->word_count * sizeof(char*));
}

/* Filling universum with elements */
bool fill_uni(uni_t *universum, char *str){

	strtok(str, " \n"); //skipping the first string (letter S)
	char *element = strtok(NULL, " \n");

	for (int i = 0; element != NULL; i++){
		if(!alpha_check(element)){
					fprintf(stderr, "Element contains non alphabetic characters\n");
					return false;
				}

		for(int i = 0; i < F; i++)
			if(!strcmp(element, forbid[i])){
				fprintf(stderr, "Element \"%s\" is forbidden\n", forbid[i]);
				return false;
			}

		if(strlen(element)>30){
			fprintf(stderr, "Element of universum too long, max 30\n");
			return false;
		}

		universum->elms[i] = malloc(strlen(element)+1*sizeof(char));
		strcpy(universum->elms[i], element);
		element = strtok(NULL, " \n");
	}

	return true;
}

/* Universum destructor */
void uni_dtor(uni_t *universum){
	for (int i = 0; i<universum->word_count; i++)
		free(universum->elms[i]);

	free(universum->elms);
}

				  /* SETS */
/*********************************************/

/* Constructor of set structure */
void set_ctor(set_t *set, char *str){
	set->word_count = count_word_count(str);
	set->elms_idx = malloc(set->word_count * sizeof(int));
}

/* Filling set with indexes of elements in universum */
bool fill_set(set_t *set, char *str, uni_t *universum){
	char *element = NULL;
	int counter;
	strtok(str, " \n"); //skipping the first string (letter S)

	for (int i = 0; i < set->word_count; i++){
		counter = 0;
		element = strtok(NULL, " \n");

		for (int j = 0; j < universum->word_count; j++){
			if (!strcmp(element, universum->elms[j])){
				set->elms_idx[i] = j;
				break;
			}
			else counter++;

			if(counter == universum->word_count) return false;
		}
	}
	//sorting set by order of elements in universum (for easier operations)
	my_sort(set);

	return true;
}

/* Set destructor */
void set_dtor(set_t *set){
	free(set->elms_idx);
}

/* Constructor of msets structure */
void msets_ctor(msets_t *msets){
	msets->sets = malloc(sizeof(set_t));
}

/* Adding set to msets structure */
void add_set(msets_t *msets, set_t *set){
	//allocating memory to msets->sets
	if(msets->sets_count == 0)
		msets_ctor(msets);

	else msets->sets = realloc(msets->sets, (msets->sets_count + 1) * sizeof(set_t));

	//copying values
	msets->sets[msets->sets_count].word_count = set->word_count;
	msets->sets[msets->sets_count].elms_idx = malloc(set->word_count * sizeof(int));

	for (int i = 0; i < set->word_count; i++)
		msets->sets[msets->sets_count].elms_idx[i] = set->elms_idx[i];
}

/* Multiple sets structure destructor */
void msets_dtor(msets_t *msets){
	if(msets->sets_count == 0) return; // if no allocations were made, do not free
	for(int i = 0; i < msets->sets_count; i++)
		free(msets->sets[i].elms_idx);

	free(msets->sets);
}


				/* RELATIONS */
/*********************************************/

/* Constructor of mrels structure */
void mrels_ctor(mrels_t *mrels){
	mrels->rels = malloc(sizeof(rel_t));
}

/* Constructor of rels structure */
bool rel_ctor(rel_t *rel, char *str, uni_t *universum){
	rel->pair_count = count_pairs_count(str);
	char *pair;
	strtok(str, " ");
	int counter;

	//allocates memory for int array
	rel->pairs = malloc(rel->pair_count * sizeof(int*));

	//allocates memory for two elements
	for(int i = 0; i < rel->pair_count; i++)
			rel->pairs[i] = malloc(2 * sizeof(int));

	for(int j = 0; j < rel->pair_count; j++){
		for(int l = 0; l < 2; l++){
			counter = 0;
			pair = strtok(NULL, "( )");

			//copies index of element in universum
			for (int k = 0; k < universum->word_count; k++){
				if (!strcmp(pair, universum->elms[k])){
					rel->pairs[j][l] = k;
					break;
				}
				else counter++;

				if(counter == universum->word_count) return false;
			}
		}
	}
	return true;
}

/* Destructor of rel structure */
void rel_dtor(rel_t *rel){
	for(int i = 0; i < rel->pair_count; i++){
		free(rel->pairs[i]);
	}

	free(rel->pairs);
}

/* Adds rel to "mrels->rels" array */
void add_rel(mrels_t *mrels, rel_t *rel){
	//allocating/reallocating memory for array of structures
	if(mrels->rels_count == 0)
		mrels_ctor(mrels);
	else mrels->rels = realloc(mrels->rels, (mrels->rels_count + 1) * sizeof(rel_t));

	//copying number of pair_count
	mrels->rels[mrels->rels_count].pair_count = rel->pair_count;

	//allocating memory for array of pairs/pairs
	mrels->rels[mrels->rels_count].pairs = malloc(rel->pair_count * sizeof(int*));
	for(int i = 0; i < rel->pair_count; i++)
		mrels->rels[mrels->rels_count].pairs[i] = malloc(sizeof(int*));

	for(int i = 0; i < rel->pair_count; i++){
		for(int j = 0; j < 2; j++){
			mrels->rels[mrels->rels_count].pairs[i][j] = rel->pairs[i][j];
		}
	}

}

/* Destructor of mrels structure */
void mrels_dtor(mrels_t *mrels){
	//if no memory allocated, do not free
	if(mrels->rels_count == 0) return;

	for(int i = 0; i < mrels->rels_count; i++){
		for(int j = 0; j < mrels->rels[i].pair_count; j++){
			free(mrels->rels[i].pairs[j]);
		}
	}
	for(int i = 0; i < mrels->rels_count; i++){
		free(mrels->rels[i].pairs);
	}

	free(mrels->rels);
}

				/* MISC */
/*********************************************/

void print_uni(uni_t *universum){
	printf("U");
	for(int i = 0; i < universum->word_count; i++){
		printf(" %s", universum->elms[i]);
	}
	printf(("\n"));
}

/* Printing set */
void print_set(set_t set, uni_t *universum){
	printf("S");
	for (int i = 0; i < set.word_count; i++){
		printf(" %s", universum->elms[set.elms_idx[i]]);
	}

	printf("\n");
}

/* Printing set without the first character "S" (used for operations) */
void print_set_o(set_t set, uni_t *universum){
	for (int i = 0; i < set.word_count; i++){
		printf(" %s", universum->elms[set.elms_idx[i]]);
	}
	printf("\n");
}

/* Printing rel */
void print_rel(rel_t *rel, uni_t *universum){
	printf("R");
	for (int i = 0; i < rel->pair_count; i++){
		printf(" (%s %s)", universum->elms[rel->pairs[i][0]], universum->elms[rel->pairs[i][1]]);
	}
	printf("\n");
}

/* Initializing unary operation values */
bool unary_init(un_oper *oper, char *str, char list[][15], msets_t *msets, char *specifiers){
	oper->is_rel = false;
	strtok(str, " ");
	oper->operation = strtok(NULL, " ");

	for(int i = 0; i < R; i++){
		if(!strcmp(oper->operation, list[i])){
			//if operand is not a relation
			int number = atoi(strtok(NULL, " "));
			if (specifiers[number] != 'R') return false;

			oper->operand = number - 2 - msets->sets_count;
			oper->is_rel = true;
			return true;
		}
	}

	//if operand is not a set
	int number = atoi(strtok(NULL, " "));
	if (specifiers[number] != 'S') return false;

	oper->operand = number - 2;
	return true;
}

/* Initializing binary operation values */
bool binary_init(bin_oper *oper, char *str, char *specifiers){
	int number1, number2;
	strtok(str, " ");
	oper->operation = strtok(NULL, " ");
	number1 = atoi(strtok(NULL, " "));
	number2 = atoi(strtok(NULL, " "));
	if (specifiers[number1] != 'S' || specifiers[number2] != 'S') return false;

	oper->operand1 = number1 - 2;
	oper->operand2 = number2 - 2;
	return true;
}

/* Initializing ternary operation values */
void ternary_init(ter_oper *oper, char *str, msets_t *msets){
	strtok(str, " ");
	oper->operation = strtok(NULL, " ");
	oper->rel = atoi(strtok(NULL, " ")) - 2 - msets->sets_count;
	oper->set_a = atoi(strtok(NULL, " ")) - 2;
	oper->set_b = atoi(strtok(NULL, " ")) - 2;

}

				/* OPERATIONS */
/*********************************************/

/* Operations with sets */
void empty(msets_t *msets, int a){
	if (msets->sets[a].word_count == 0) printf("true\n");
	printf("false");
}

void card(msets_t *msets, int a){
	printf("%d", msets->sets[a].word_count);
}

void complement(msets_t *msets, int a, uni_t *universum){
	int last_index = 0;
	printf("S");
	
	for(int j = 0; j <= msets->sets[a].word_count; j++){
		for(int i = last_index; i < universum->word_count; i = last_index){
			last_index++;

			if(universum->elms[i] == universum->elms[msets->sets[a].elms_idx[j]])
				break;

			else printf(" %s", universum->elms[i]);
		}
	}
}

void intersect(msets_t *msets, int a, int b, uni_t *universum){
	printf("S");

	//finding min and max size set
	set_t *min, *max;

	if(msets->sets[a].word_count > msets->sets[b].word_count){
		max = &msets->sets[a];
		min = &msets->sets[b];
	}

	else{
		max = &msets->sets[b];
		min = &msets->sets[a];
	}

	//printing intersect
	for(int j = 0; j < min->word_count; j++)
		for(int i = 0; i < max->word_count; i++)
			if(max->elms_idx[i] == min->elms_idx[j]) 
				printf(" %s", universum->elms[min->elms_idx[j]]);
}

void minus(msets_t *msets, int a, int b, uni_t *universum){
	printf("S");
	int potential_idx;

	//printing minus
	for (int i = 0; i < msets->sets[a].word_count; i++){
		for(int j = 0; j < msets->sets[b].word_count; j++){

			if(msets->sets[a].elms_idx[i] == msets->sets[b].elms_idx[j])
				break;

			else if (msets->sets[a].elms_idx[i] != msets->sets[b].elms_idx[j])
				potential_idx = msets->sets[a].elms_idx[i];

			if(msets->sets[b].word_count-1 == j){
				printf(" %s", universum->elms[potential_idx]);
				break;
			}
		}
	}
}

void uni(msets_t *msets, int a, int b, uni_t *universum){
	minus(msets, a, b, universum);
	print_set_o(msets->sets[b], universum);
}

bool equals(msets_t *msets, int a, int b){
	if(msets->sets[a].word_count != msets->sets[b].word_count)
		return false;

	for (int i = 0; i < msets->sets[a].word_count; i++)
		if (msets->sets[a].elms_idx[i] != msets->sets[b].elms_idx[i])
			return false;

	return true;
}

bool subseteq(msets_t *msets, int a, int b){
	if (msets->sets[a].word_count > msets->sets[b].word_count) return false;

	for (int i = 0; i < msets->sets[a].word_count; i++){
		for (int j = 0; j < msets->sets[b].word_count; j++){
			if (msets->sets[a].elms_idx[i] == msets->sets[b].elms_idx[j]) 
				break;

			else if(msets->sets[a].elms_idx[i] != msets->sets[b].elms_idx[j] && j == msets->sets[b].word_count - 1) 
				return false;
		}
	}

	return true;
}

bool subset(msets_t *msets, int a, int b){
	if (msets->sets[a].word_count == msets->sets[b].word_count) return false;
	if (msets->sets[a].word_count > msets->sets[b].word_count) return false;

	for (int i = 0; i < msets->sets[a].word_count; i++){
		for (int j = 0; j < msets->sets[b].word_count; j++){
			if (msets->sets[a].elms_idx[i] == msets->sets[b].elms_idx[j]) 
				break;

			else if(msets->sets[a].elms_idx[i] != msets->sets[b].elms_idx[j] && j == msets->sets[b].word_count - 1)
				return false;
		}
	}

	return true;
}

/* Operations with relations */
bool reflexive(mrels_t *mrels, int rel_id, uni_t *universum){
	int counter = 0;

	for(int i = 0; i < mrels->rels[rel_id].pair_count; i++){
			 if(mrels->rels[rel_id].pairs[i][0] == 
			 	mrels->rels[rel_id].pairs[i][1]) counter++;
	}
	
	if(counter != universum->word_count)  return false;

	return true;
}

bool symmetric(mrels_t *mrels, int rel_id){
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		for (int j= 0; j < mrels->rels[rel_id].pair_count; j++){
			if (mrels->rels[rel_id].pairs[i][0] == mrels->rels[rel_id].pairs[j][1] &&
			    mrels->rels[rel_id].pairs[i][1] == mrels->rels[rel_id].pairs[j][0]) break;

			if (j == mrels->rels[rel_id].pair_count - 1) return false;
		}
	}

	return true;
}

bool antisymmetric(mrels_t *mrels, int rel_id){
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		if (mrels->rels[rel_id].pairs[i][0] != mrels->rels[rel_id].pairs[i][1]){ // jestli jsou prvky v relaci rozdilne, tak hledame symetrii
			for(int j = 0; j < mrels->rels[rel_id].pair_count; j++){
				if (mrels->rels[rel_id].pairs[i][0] == mrels->rels[rel_id].pairs[j][1] &&
			    	mrels->rels[rel_id].pairs[i][1] == mrels->rels[rel_id].pairs[j][0]) return false;
			}
		}
	}

	return true;
}

bool transitive(mrels_t *mrels, int rel_id){
	//searching for non-reflexive pair
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		if (mrels->rels[rel_id].pairs[i][0] != mrels->rels[rel_id].pairs[i][1]){

			//searching for pair, which starts with second element from first pair
			for(int j = 0; j < mrels->rels[rel_id].pair_count; j++){
				if (mrels->rels[rel_id].pairs[i][1] == mrels->rels[rel_id].pairs[j][0]){

					//searching for transitive pair
					for(int k = 0; k < mrels->rels[rel_id].pair_count; k++){
						if(mrels->rels[rel_id].pairs[k][0] == mrels->rels[rel_id].pairs[i][0] &&
						   mrels->rels[rel_id].pairs[k][1] == mrels->rels[rel_id].pairs[j][1]) {
						   	j = mrels->rels[rel_id].pair_count + 1;
							break;
						}

						else if (k == mrels->rels[rel_id].pair_count - 1) return false; //if not found, return false
					}
				}

				else if (j == mrels->rels[rel_id].pair_count - 1) return false; //if not found, return false
			}
		}
	}

	return true;
}

bool function(mrels_t *mrels, int rel_id){
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		int counter = 0;

		for (int j = 0; j < mrels->rels[rel_id].pair_count; j++){
			if (mrels->rels[rel_id].pairs[i][0] == mrels->rels[rel_id].pairs[j][0]) counter++;

			if (counter == 2) return false;
		}
	}

	return true;

}

void domain(mrels_t *mrels, int rel_id, uni_t *universum){
	int *el_found = malloc(universum->word_count * sizeof(int));
	for (int i = 0; i < universum->word_count; i++){
		el_found[i] = -1;
	} 
	printf("S");
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		for (int j = 0; j < universum->word_count; j++){
			if (mrels->rels[rel_id].pairs[i][0] == el_found[j]) break;
			if (el_found[j] == -1){
				printf(" %s", universum->elms[mrels->rels[rel_id].pairs[i][0]]);
				el_found[j] = mrels->rels[rel_id].pairs[i][0];
				break;
			}
		}
	}

	free(el_found);
}

void codomain(mrels_t *mrels, int rel_id, uni_t *universum){
	int *el_found = malloc(universum->word_count * sizeof(int));
	for (int i = 0; i < universum->word_count; i++){
		el_found[i] = -1;
	} 
	printf("S");
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		for (int j = 0; j < universum->word_count; j++){
			if (mrels->rels[rel_id].pairs[i][1] == el_found[j]) break;
			if (el_found[j] == -1){
				printf(" %s", universum->elms[mrels->rels[rel_id].pairs[i][1]]);
				el_found[j] = mrels->rels[rel_id].pairs[i][1];
				break;
			}
		}
	}

	free(el_found);
}

bool injective(mrels_t *mrels, int rel_id, set_t *a, set_t *b){
	if (!function(mrels, rel_id)) return false;
	if (a->word_count > b->word_count) return false;
	int counter_a = 0;

	//domain check
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		for (int j = 0; j < a->word_count; j++){
			if (a->elms_idx[j] == mrels->rels[rel_id].pairs[i][0]) break;
			if (j == a->word_count-1) return false;
		}
	}

	//codomain check
	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		for (int j = 0; j < b->word_count; j++){
			if (b->elms_idx[j] == mrels->rels[rel_id].pairs[i][1]){
				counter_a++;
				break;
			}
		}
		if (counter_a == a->word_count) break;
	}

	if(counter_a < a->word_count) return false;

	for (int i = 0; i < mrels->rels[rel_id].pair_count; i++){
		int counter = 0;
		for (int j = 0; j < mrels->rels[rel_id].pair_count; j++){
			if (mrels->rels[rel_id].pairs[i][1] == mrels->rels[rel_id].pairs[j][1]) counter++;
			if (counter == 2) return false;
		}
	}
	return true;
}

bool surjective(mrels_t *mrels, int rel_id, set_t *a, set_t *b){
	if (!function(mrels, rel_id)) return false;

	//domain check
	for (int i = 0; i < a->word_count; i++){
		for (int j = 0; j < mrels->rels[rel_id].pair_count; j++){
			if (a->elms_idx[i] == mrels->rels[rel_id].pairs[j][0]) break;
			if (j == mrels->rels[rel_id].pair_count-1) return false;
		}
	}

	//codomain check
	for (int i = 0; i < b->word_count; i++){
		for (int j = 0; j < mrels->rels[rel_id].pair_count; j++){
			if (b->elms_idx[i] == mrels->rels[rel_id].pairs[j][1]) break;
			if (j == mrels->rels[rel_id].pair_count-1) return false;
		}
	}
	return true;
}

bool bijective(mrels_t *mrels, int rel_id, set_t *a, set_t *b){
	if (surjective(mrels, rel_id, a, b) && injective(mrels, rel_id, a, b) == true) return true;

	return false;
}

				/* MAIN PROGRAM */
/***********************************************/

int main(int argc, char **argv){
	
	if (argc!=2){
		fprintf(stderr,"Wrong number of arguments (expected 2), Usage: ./setcal FILENAME\n");
		return 1;
	}

	//Opening file
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL){
		fprintf(stderr, "Cannot open '%s': No such file\n", argv[1]);
		return 1;
	}
	char rel_oper_list[R][15] = {"reflexive", "symmetric", "antisymmetric", "transitive", 
								"function", "domain", "codomain", "injective", "surjective", "bijective"}; 
								//List of functions with relations in order to determine relation index operation initializers
	int row = 1;
	char specifiers[1001];
	char *buff = NULL;
	bool error_found = false;

	//Declaring structures
	un_oper unary;
	bin_oper binary;
	ter_oper ternary;

	uni_t universum; 

	msets_t msets;
	msets.sets_count = 0;

	mrels_t mrels;
	mrels.rels_count = 0;

	size_t size;

	/* READING FILE */
	while (getline(&buff, &size, fp) > 0 && row < 1001){
		buff[strcspn(buff, "\r")] = 0;//windows files go brrrrrr
		specifiers[row] = buff[0];

		if (buff[0] == 'U' && (buff[1] == ' ' || buff[1] == '\n')){
			if(row != 1) error(2);
			uni_ctor(&universum, buff);

			//filling universum, checking for length
			if(!fill_uni(&universum, buff)){

				//if there is a fault in filling universum, free memory and exit
				uni_dtor(&universum);
				free(buff);
				fclose(fp);
				return 1;
			}
			print_uni(&universum);

		}

		else if (buff[0] == 'S' && (buff[1] == ' ' || buff[1] == '\n')){
			set_t set;
			set_ctor(&set, buff);

			if(!fill_set(&set, buff, &universum)){
				fprintf(stderr,"Element is not from universum\n");
				error_found = true;
				set_dtor(&set);
				break;
			}

			if(!is_unique(&set)){
				fprintf(stderr, "Two or more same elements in set\n");
				error_found = true;
				set_dtor(&set);
				break;
			}

			add_set(&msets, &set);
			print_set(msets.sets[msets.sets_count], &universum);
			msets.sets_count++;
			set_dtor(&set);
		}

		else if(buff[0] == 'R' && (buff[1] == ' ' || buff[1] == '\n')){
			rel_t rel;

			if (!rel_ctor(&rel, buff, &universum)){
				fprintf(stderr, "Element in rel not from universum\n");
				error_found = true;
				rel_dtor(&rel);
				break;
			}

			if(!is_pair_unique(&rel)){
				fprintf(stderr, "Two or more same elements in a relation\n");
				error_found = true;
				rel_dtor(&rel);
				break;
			}

			add_rel(&mrels, &rel);
			print_rel(&mrels.rels[mrels.rels_count], &universum);
			mrels.rels_count++;
			rel_dtor(&rel);
		}

		else if (buff[0] == 'C' && (buff[1] == ' ' || buff[1] == '\n')){

			bool is_unary = false;
			bool is_binary = false;
			bool is_ternary = false;

			if (count_word_count(buff) == 2){
				if(!unary_init(&unary, buff, rel_oper_list, &msets, specifiers)){
					fprintf(stderr, "Wrong operand\n");
					break;
				}
				is_unary = true;
			}

			else if (count_word_count(buff) == 3){
				if(!binary_init(&binary, buff, specifiers)){
					fprintf(stderr, "Wrong operand\n");
					break;
				}
				is_binary = true;
			}

			else if(count_word_count(buff) == 4){
				ternary_init(&ternary, buff, &msets);
				is_ternary = true;
			}

			else {
					fprintf(stderr, "Wrong operation format\n");
					break;
				}

			if(is_unary && !unary.is_rel){

				if (!strcmp(unary.operation, "empty"))
					empty(&msets, unary.operand);

				else if (!strcmp(unary.operation, "card"))
					card(&msets, unary.operand);

				else if (!strcmp(unary.operation, "complement"))
					complement(&msets, unary.operand, &universum);

				else {
					fprintf(stderr, "Wrong operation format\n");
					break;
				}
			}

			else if(is_unary && unary.is_rel){
				if (!strcmp(unary.operation, "reflexive"))
					printf("%s", reflexive(&mrels, unary.operand, &universum) ? "true" : "false");

				else if (!strcmp(unary.operation, "symmetric"))
					printf("%s", symmetric(&mrels, unary.operand) ? "true" : "false");

				else if (!strcmp(unary.operation, "antisymmetric"))
					printf("%s", antisymmetric(&mrels, unary.operand) ? "true" : "false");

				else if (!strcmp(unary.operation, "transitive"))
					printf("%s", transitive(&mrels, unary.operand) ? "true" : "false");
				
				else if (!strcmp(unary.operation, "function"))
					printf("%s", function(&mrels, unary.operand) ? "true" : "false");

				else if (!strcmp(unary.operation, "domain"))
					domain(&mrels, unary.operand, &universum);

				else if (!strcmp(unary.operation, "codomain"))
					codomain(&mrels, unary.operand, &universum);

				else {
					fprintf(stderr, "Wrong operation format\n");
					break;
				}
			}

			else if(is_binary){

				if (!strcmp(binary.operation, "intersect"))
					intersect(&msets, binary.operand1, binary.operand2, &universum);

				else if (!strcmp(binary.operation, "minus"))
					minus(&msets, binary.operand1, binary.operand2, &universum);

				else if (!strcmp(binary.operation, "union"))
					uni(&msets, binary.operand1, binary.operand2, &universum);

				else if (!strcmp(binary.operation, "equals"))
					printf("%s", equals(&msets, binary.operand1, binary.operand2) ? "true" : "false");

				else if (!strcmp(binary.operation, "subseteq"))
					printf("%s", subseteq(&msets, binary.operand1, binary.operand2) ? "true" : "false");

				else if (!strcmp(binary.operation, "subset"))
					printf("%s", subset(&msets, binary.operand1, binary.operand2) ? "true" : "false");

				else {
					fprintf(stderr, "Wrong operation format\n");
					break;
				}
			}

			else if(is_ternary){

				if (!strcmp(ternary.operation, "injective"))
					printf("%s",injective(&mrels, ternary.rel, &(msets.sets[ternary.set_a]), &(msets.sets[ternary.set_b])) ? "true" : "false");
				
				else if (!strcmp(ternary.operation, "surjective"))
					printf("%s",surjective(&mrels, ternary.rel, &(msets.sets[ternary.set_a]), &(msets.sets[ternary.set_b])) ? "true" : "false");

				else if (!strcmp(ternary.operation, "bijective"))
					printf("%s",bijective(&mrels, ternary.rel, &(msets.sets[ternary.set_a]), &(msets.sets[ternary.set_b])) ? "true" : "false");

				else {
					fprintf(stderr, "Wrong operation format\n");
					break;
				}
			}

			else{
				fprintf(stderr, "Wrong operand\n");
				break;
			}
			printf("\n");
		}
		
		else {
			fprintf(stderr, "Wrong specifier on line %d (only U,S,R,C allowed)\n", row);
			error_found = true;
			break;
		}

		row++;
	}

	/* FREEING MEMORY */
	uni_dtor(&universum);
	msets_dtor(&msets);
	mrels_dtor(&mrels);

	free(buff);
	fclose(fp);

	if(error_found) return 1;

	return 0;
}