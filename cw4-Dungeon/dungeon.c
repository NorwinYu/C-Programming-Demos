#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

/* Definition of the structure to store point information. */
struct point_list {
   int p_x;				//x coordinate
   int p_y;				//y coordinate
   int cost_cur;		//cost from the start point
   int cost_c_add_h;	//cost added by heuristic_distance used in A star algorithm
};
typedef struct point_list PointList;

/* Definition of the structure to store path using simple quad_tree. */
struct simple_quadtree {
   struct point_list p_info;			//store point information
   struct simple_quadtree *father_p;	//point to father point
   struct simple_quadtree *up_p;		//point to up point in map
   struct simple_quadtree *right_p;		//point to right point in map
   struct simple_quadtree *down_p;		//point to down point in map
   struct simple_quadtree *left_p;		//point to left point in map
};
typedef struct simple_quadtree S_quadtree;

/* Definition of the structure to store points in stack. */
struct int_list {
   struct simple_quadtree *tree_p;		//pointers to node in tree
   struct int_list *next;				//pointers to next node in list
};
typedef struct int_list IntList;

/* Define our stack type just as a pointer to the start of a list. */
struct stack {
   IntList *start;	//start node
};
typedef struct stack Stack;

/* Check input string is represent for non-negative integers or not, and if is, convert to integer */
int check_int(int *value, char *argument)
{
	char *endptr;
	long v = strtol(argument, &endptr, 10);
	if(endptr == argument) 	// error in converting to a number
	{
		return 0; 	//return 0 to say error in converting to a number
	}
	else 	// got a number, return it if within int bounds
	{
		if(v > INT_MAX || v < INT_MIN)
		{
			return 0;
		}
		else
		{
			*value = (int)v;
			if (v < 0)
			{
				return -1;
			}
			else
			{
				return 1; 	//return 1 to say sucessfully in converting to a non-negative number
			}
		}
	}
}

/* return the maximum of two numbers */
int max(int a, int b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

/* prompt error if command is invalid */
void error_invalid_cl()
{
	printf("Invalid command line arguments. Usage: [filename] <x> <y>\n");
	exit(1);
}

/* prompt error if is unable to allocate memory */
void allocate_error()
{
	printf("Unable to allocate memory.\n"); 	//error when allocate memory with exit code -1 
	exit(5);
}

/* prompt error if dungeon file is invalid */
void error_invalid_fl()
{
	printf("Invalid dungeon file!\n");
	exit(2);
}

/* Return the length of the list. */
int list_length(IntList *start)
{
	if(start == NULL)
	{
		return 0;
	}

	int total = 1;
	IntList *cur = start;
	while(cur->next != NULL)
	{
		cur = cur->next;
		total++;
	}
	return total;
}

/*adds a new node to a sorted list */
void sorted_list_insert(IntList **start, S_quadtree *newval_tree_p)
{
	IntList *newel = malloc(sizeof(IntList));	//malloc for new node
	if(newel == NULL)
	{
		allocate_error();	//prompt if error
		free(newel);
	}
	newel->tree_p = newval_tree_p;	//store point to the node in tree

	//if case for the head end or the predic cost of the new node is small than the predic cost of the head node
	if (*start == NULL || (*start)->tree_p->p_info.cost_c_add_h >= newel->tree_p->p_info.cost_c_add_h)
	{
		newel->next = *start;	//add to the head of the list
		*start = newel;
	}
	else	 //Locate the node in the sorted list depending on predic cost
	{
		IntList *cur = *start;
		while(cur->next != NULL && cur->tree_p->p_info.cost_c_add_h < newel->tree_p->p_info.cost_c_add_h)
		{
			cur = cur->next;
		}
		newel->next = cur->next;
		cur->next = newel;
	}
}

/*takes a pointer to a pointer to the first element of a list and an index, and removes that elements at that index from the list.*/
void list_remove(IntList **start, int index)
{
	if(*start == NULL)	//if start node is empty, return
	{
		return;
	}

	int counter = 0;
	IntList *cur = *start;

	if (index == 0)	//remove from the start of the list
	{
		*start = cur->next;
		free(cur);
	}
	else
	{
		while((index - 1) > counter)	//locate the position 
		{
			if (cur->next == NULL)
			{
				break;
			}
			cur = cur->next;
			counter++;
		}
		if ((cur->next)->next == NULL)	//if the target node is end of list
		{
			free(cur->next);
			cur->next = NULL;
		}
		else							//if in the list
		{
			free(cur->next);
			cur->next = (cur->next)->next;
		}
	}
}

/* Creates a new, empty stack.
** Returns NULL on error. */
Stack *stack_create()
{
	Stack *st = malloc(sizeof(Stack));
	if(st == NULL)
	{
		free(st);
		allocate_error();
		return NULL;
	}

	st->start = NULL;

	return st;
}

/* Destroys a stack previously returned by stack_create. */
void stack_destroy(Stack *st)
{
	if(st == NULL)
	{
		return;
	}

	// Free any of the elements that are in the List we are using.
	while(list_length(st->start) > 0)
	{
		list_remove(&(st->start), 0);
	}

	// Free the memory for the Stack type itself. 
	free(st);
}

/* use Manhattan Distance to make heuristic function */
int heuristic_distance(int start_x, int start_y, int exit_x, int exit_y)
{
	int x_distance = abs(start_x - exit_x);
	int y_distance = abs(start_y - exit_y);
	int Manhattan_distance = x_distance + y_distance;
	return Manhattan_distance;
}

/* push node from sorted stack */
void sorted_stack_push(Stack *st, S_quadtree *push_tree_p)
{
	if(st == NULL)
	{
		return;
	}

	sorted_list_insert(&(st->start), push_tree_p);
}

/* add node to the quad_tree to represent the search path and father_child points relation */
void tree_add_node(S_quadtree **root_node, S_quadtree *fa_node, int change_x, int change_y, int op, int exit_x, int exit_y)
{
	S_quadtree *new_node = malloc(sizeof(S_quadtree));		//malloc for new node
	if(new_node == NULL)
	{
		allocate_error();
		free(new_node);
	}

	new_node->father_p = NULL;	//initialize pointers to NULL
	new_node->up_p = NULL;
	new_node->right_p = NULL;
	new_node->down_p = NULL;
	new_node->left_p = NULL;

	if (op == 0)	//if add root node
	{
		new_node->p_info.p_x = change_x;	//store point information
		new_node->p_info.p_y = change_y;
		new_node->p_info.cost_cur = 0;	//initialize the current cost

		*root_node = new_node;	//to be root node
	}
	else		//if add child node
	{
		new_node->p_info.p_x = fa_node->p_info.p_x + change_x;	//store point information
		new_node->p_info.p_y = fa_node->p_info.p_y + change_y;
		new_node->p_info.cost_cur = fa_node->p_info.cost_cur + 1;	//current cost add

		new_node->father_p = fa_node;	//store father node

		if (op == 1)		//add up node
		{
			fa_node->up_p = new_node;
		}
		else if (op == 2)	//add right node
		{
			fa_node->right_p = new_node;
		}
		else if (op == 3)	//add down node
		{
			fa_node->down_p = new_node;
		}
		else				//add left node
		{
			fa_node->left_p = new_node;
		}
	}
	//calculate the manhettan distance and save to be used in A star algorithm
	int Manhattan_distance_start = heuristic_distance(new_node->p_info.p_x, new_node->p_info.p_y, exit_x, exit_y);
	new_node->p_info.cost_c_add_h = new_node->p_info.cost_cur + Manhattan_distance_start;
}

/* Destroys a quad_tree previously. */
void tree_destroy(S_quadtree *tree_root)
{
	if (tree_root == NULL)
	{
		return;
	}
	
	tree_destroy(tree_root->up_p);
	tree_destroy(tree_root->right_p);
	tree_destroy(tree_root->down_p);
	tree_destroy(tree_root->left_p);

	free(tree_root);
}

/* check point if is in open list */
int is_in_stack(Stack *st, int target_x, int target_y, int *cur_cost)
{
	IntList *cur = st->start;
	for (int i = 0; i < list_length(st->start); i++)
	{
		if (target_x == cur->tree_p->p_info.p_x && target_y == cur->tree_p->p_info.p_y)
		{
			*cur_cost = cur->tree_p->p_info.cost_cur;
			return i;
		}
		cur = cur->next;
	}
	return -1;
}

int main(int argc, char *argv[])
{
	/*command lines check, if do not match, call error_invalid_cl to prompt error and exit with code 1 */
	if (argc == 3 || argc == 4)	//check arguments number, if not 3 or 4,  error 
	{
		int argv_x_index = 1;	//if input three arguments, the second argument should be x
		char *map_fl_name = malloc(sizeof(char) * 12);	// allocate memory for map name
		if(map_fl_name == NULL)
		{
			free(map_fl_name);
			allocate_error(); 	//check allocate memory
		}

		if (argc == 4)	//if input four arguments,
		{
			argv_x_index = 2;	// the third argument should be x
			map_fl_name = realloc(map_fl_name, max(sizeof(char) * (strlen(argv[1]) + 1), 12)); //realloc memory for other name
			if(map_fl_name == NULL)
			{
				free(map_fl_name);
				allocate_error(); 	//check allocate memory
			}
			strcpy(map_fl_name, argv[1]);
		}
		else
		{
			strcpy(map_fl_name, "dungeon.map");	//if not input map path, use default map name
		}

		int input_start_x, input_start_y;
		int is_int_input_x = check_int(&input_start_x, argv[argv_x_index]);
		int is_int_input_y = check_int(&input_start_y, argv[argv_x_index + 1]);
		if (is_int_input_x != 1 || is_int_input_y != 1)
		{
			error_invalid_cl();
		}

		/* read the map */
		FILE *fp = fopen(map_fl_name, "r");	//open the map file
		if (!fp)
		{
			perror("Error reading dungeon file");
			exit(3);
		}

		long read_x, read_y;	//read width and high of the map and do check
		if (fscanf(fp, "%ld", &read_x) == 0 || read_x > INT_MAX || read_x < INT_MIN)
		{
			error_invalid_fl();
		}
		if (fscanf(fp, "%ld", &read_y) == 0 || read_y > INT_MAX || read_y < INT_MIN)
		{
			error_invalid_fl();
		}
		int map_array[read_y][read_x];	//declare an array for map

		char nl_donothing;	//read the first's line new line character but do nothing
		fscanf(fp, "%c", &nl_donothing);
		if (nl_donothing != '\n')
		{
			error_invalid_fl();
		}

		int counter_exit = 0;						//count exit point numbers
		int read_map_p_x = 0, read_map_p_y = 0;		//count x, y position for loop
		int counter_lines = 0;						//count line numbers to check is match the high
		int is_quit = 0;							//represent if is to exit the loop
		int exit_p_x;								//exit point x
		int exit_p_y;								//exit point y
		do
		{
			int is_newline = 0;		//represent if get new line character or not
			do
			{
				is_newline = 0;		//initialize in erery loop
				int counter_scan_char = 0;		//count scaned character to check if valid map
				while(is_newline == 0 && counter_scan_char <= read_x + 1)
				{
					char tmp;					//read by character
					fscanf(fp, "%c", &tmp);
					if (tmp == '\n')		//if get new line character
					{
						is_newline = 1;
					}
					else if (tmp == '#')	//if get wall
					{
						map_array[read_map_p_y][read_map_p_x] = 0;	//0 represent for wall
						counter_scan_char++;
						read_map_p_x++;
					}
					else if (tmp == '.')	//if get road
					{
						map_array[read_map_p_y][read_map_p_x] = 1;	//1 represent for road
						counter_scan_char++;
						read_map_p_x++;
					}
					else if (tmp == 'x')	//if get exit point
					{
						map_array[read_map_p_y][read_map_p_x] = 1;	//1 represent for road, exit is a special road point
						counter_scan_char++;
						counter_exit++;
						exit_p_x = read_map_p_x;	//store exit point x, y
						exit_p_y = read_map_p_y;
						read_map_p_x++;
					}
					else
					{
						error_invalid_fl();	//prompt if read other character
					}
				}

				//the numbers every line must be width read in line one
				if (counter_scan_char != read_x && counter_lines < read_y)
				{
					error_invalid_fl();
				}

				counter_lines++;
				read_map_p_y++;
				read_map_p_x = 0;	//initialize in erery loop

				if (feof(fp))	//if stream go to end of the file, exit the loop
				{
					is_quit = 1;
				}
			}while(is_newline == 1 && is_quit == 0);

			//the line numbers must be high number
			if ((counter_lines - 1) != read_y)
			{
				error_invalid_fl();
			}

			//must be only one exit point
			if (counter_exit != 1)
			{
				error_invalid_fl();
			}
		}while(is_quit == 0);

		//the start location should be empty and in the map
		if (input_start_x >= read_x || input_start_y >= read_y || map_array[input_start_y][input_start_x] == 0)
		{
			printf("Invalid starting location!\n");
			exit(4);
		}

		fclose(fp);	//close the file
		free(map_fl_name);

		/* use A star algorithm to find the path 
		** open_stack to save search boundary point
		** quad_tree is to save search path and father_child relation
		** use map to represent searched point and unsearched point */

		Stack *open_stack = stack_create();	//create a stack
		if(open_stack == NULL)
		{
			allocate_error();
			free(open_stack);
		}

		S_quadtree *root = NULL;	//create a root for tree

		tree_add_node(&root, root, input_start_x, input_start_y, 0, exit_p_x, exit_p_y);	//add start point to tree
		sorted_stack_push(open_stack, root);	//push start point to stack

		S_quadtree *cur_node;
		int counter_index = 0;	//to save father point index in stack
		do
		{
			cur_node = (open_stack->start)->tree_p;		//read the least predict cost point
			counter_index = 0;

			if (map_array[cur_node->p_info.p_y - 1][cur_node->p_info.p_x] == 1)	//if up is unsearched road
			{
				int cur_cost;
				int is_in_open_stack = is_in_stack(open_stack, cur_node->p_info.p_x, cur_node->p_info.p_y - 1, &cur_cost);	//check if in open stack
				if ((is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1) || is_in_open_stack == -1)
				{
					if (is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1)	//in open stack and current cost less than before
					{
						list_remove(&(open_stack->start), is_in_open_stack);	//move the pointer and add new pointer later
					}
					tree_add_node(&root, cur_node, 0, -1, 1, exit_p_x, exit_p_y);	//add point the tree
					sorted_stack_push(open_stack, cur_node->up_p);					//push point to stack
					if (cur_node->up_p->p_info.cost_c_add_h <= cur_node->p_info.cost_c_add_h)
					{
						counter_index++;
					}
				}
			}

			if (map_array[cur_node->p_info.p_y][cur_node->p_info.p_x + 1] == 1)	//if right is unsearched road
			{
				int cur_cost;
				int is_in_open_stack = is_in_stack(open_stack, cur_node->p_info.p_x + 1, cur_node->p_info.p_y, &cur_cost);	//check if in open stack
				if ((is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1) || is_in_open_stack == -1)
				{
					if (is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1)	//in open stack and current cost less than before
					{
						list_remove(&(open_stack->start), is_in_open_stack);	//move the pointer and add new pointer later
					}
					tree_add_node(&root, cur_node, 1, 0, 2, exit_p_x, exit_p_y);	//add point the tree
					sorted_stack_push(open_stack, cur_node->right_p);				//push point to stack
					if (cur_node->right_p->p_info.cost_c_add_h <= cur_node->p_info.cost_c_add_h)
					{
						counter_index++;
					}
				}
			}

			if (map_array[cur_node->p_info.p_y + 1][cur_node->p_info.p_x] == 1)	//if down is unsearched road
			{
				int cur_cost;
				int is_in_open_stack = is_in_stack(open_stack, cur_node->p_info.p_x, cur_node->p_info.p_y + 1, &cur_cost);	//check if in open stack
				if ((is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1) || is_in_open_stack == -1)
				{
					if (is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1)	//in open stack and current cost less than before
					{
						list_remove(&(open_stack->start), is_in_open_stack);	//move the pointer and add new pointer later
					}
					tree_add_node(&root, cur_node, 0, 1, 3, exit_p_x, exit_p_y);	//add point the tree
					sorted_stack_push(open_stack, cur_node->down_p);				//push point to stack
					if (cur_node->down_p->p_info.cost_c_add_h <= cur_node->p_info.cost_c_add_h)
					{
						counter_index++;
					}
				}
			}

			if (map_array[cur_node->p_info.p_y][cur_node->p_info.p_x - 1] == 1)	//if left is unsearched road
			{
				int cur_cost;
				int is_in_open_stack = is_in_stack(open_stack, cur_node->p_info.p_x - 1, cur_node->p_info.p_y, &cur_cost);	//check if in open stack
				if ((is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1) || is_in_open_stack == -1)
				{
					if (is_in_open_stack != -1 && cur_cost > cur_node->p_info.cost_cur + 1)	//in open stack and current cost less than before
					{
						list_remove(&(open_stack->start), is_in_open_stack);	//move the pointer and add new pointer later
					}
					tree_add_node(&root, cur_node, -1, 0, 4, exit_p_x, exit_p_y);	//add point the tree
					sorted_stack_push(open_stack,cur_node->left_p);					//push point to stack
					if (cur_node->left_p->p_info.cost_c_add_h <= cur_node->p_info.cost_c_add_h)
					{
						counter_index++;
					}
				}
			}
			list_remove(&(open_stack->start), counter_index);	//move father point from the stack
			map_array[cur_node->p_info.p_y][cur_node->p_info.p_x] = 2;	//2 to represent searched
		}while(!((cur_node->p_info.p_x == exit_p_x && cur_node->p_info.p_y == exit_p_y) 
			|| list_length(open_stack->start) == 0));	//exit if get exit or stack is empty

		printf("%d, %d\n", input_start_x, input_start_y);
		if (list_length(open_stack->start) == 0)	//if stack is empty, there is no escape
		{
			printf("No escape possible.\n");
		}
		else
		{
			int paths_out[cur_node->p_info.cost_cur][2];
			S_quadtree *search = cur_node;
			int counter_path = 0;
			while(search->father_p != NULL)	//store in an array
			{
				paths_out[counter_path][0] = search->p_info.p_x;
				paths_out[counter_path][1] = search->p_info.p_y;
				search = search->father_p;
				counter_path++;
			}

			int loop_i = cur_node->p_info.cost_cur - 1;	//print path
			while(loop_i >= 0)
			{
				printf("%d, %d\n", paths_out[loop_i][0], paths_out[loop_i][1]);
				loop_i = loop_i - 1; 
			}
		}

		stack_destroy(open_stack);	//free all memory
		tree_destroy(root);
		exit(0);
	}
	else
	{
		error_invalid_cl();
	}
}
