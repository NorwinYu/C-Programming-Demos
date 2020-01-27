#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

/* Definition of the structure. */
struct single_line_list {
   char *list_name;
   char *list_phone_number;
   char *list_department;
   struct single_line_list *next;
};
typedef struct single_line_list Directory_List;

void print_menu()
{
	printf("1) Print directory\n");
	printf("2) Add number\n");
	printf("3) Modify number\n");
	printf("4) Delete number\n");
	printf("5) Search\n");
	printf("6) Quit\n");
}

void allocate_error()
{
	printf("Unable to allocate memory.\n"); 	//error when allocate memory with exit code -1 
	exit(-1);
}

char *prompt_unlimit(const char *mesg) 	//unlimited prompt function to use malloc and realloc to make enough space to store string and prompt
{
	int size = 10; 	//initial size = 10
	char *buffer = malloc(sizeof(char) * size); 	//allocate the string buffer
	if(buffer == NULL)
	{
		allocate_error(); 	//check allocate memory
		return NULL;
	}

	printf("%s", mesg); 	// print the prompt

	int position = 0, out_number = 0, scan_char_number = 0; 	//position to repersent position in buffer, out_number to check if can exit the loop, scan_char_number to sum the input text length
	do
	{
		if (scan_char_number + 1 > size) 	//if size is not enough, realloc to get a bigger space
		{
			size = size + 10;
			buffer = realloc(buffer, size);
			if(buffer == NULL)
			{
				allocate_error(); 	//check allocate memory
				return NULL;
			} 
		}

		char tmp;
		scanf("%c", &tmp); 	//scan the input text by element
		scan_char_number++;
		if(tmp == '\n') 	// Pressed return
		{
			buffer[position] = '\0';   // Add nul byte at current location
			out_number = 1;
			return buffer;
		}
		else 	// Pressed a different key
		{
			buffer[position] = tmp; 	//Add the char at current location
			position++;
		}
	} while(out_number == 0);
	return NULL;
}

int read_int(const char *mesg, int *value)
{
	char *text = prompt_unlimit(mesg);
	if(text == NULL) 	// error in prompt
	{
		return 0;
	}
	else 	// read in some text
	{
		char *endptr;
		long v = strtol(text, &endptr, 10);
		if(endptr == text) 	// error in converting to a number
		{
			free(text);
			text = NULL;
			return 0; 	//return 0 to say error in converting to a number
		}
		else 	// got a number, return it if within int bounds
		{
			free(text);
			text = NULL;
			if(v > INT_MAX || v < INT_MIN)
			{
				return 0;
			}
			else
			{
				*value = (int)v;
				return 1; 	//return 1 to say sucessfully in converting to a number
			}
		}
	}
}

void print_directory(Directory_List *start)
{
	Directory_List *cur = start;
	while(cur != NULL) 	//stop while go to the null byte
	{
		printf("%s\t", cur->list_phone_number); 	//print the struct by loop
		printf("%s ", cur->list_name);
		printf("(%s)\n", cur->list_department);
		cur = cur->next; 	//go to the next node
	}
}

int read_char(const char *mesg, char **text)
{
	*text = prompt_unlimit(mesg);
	if(*text[0] == '\0')
	{
		return 0;	//return 0 if text is empty
	}
	return 1; 	//return 1 if not empty
}

int find_index_directory(Directory_List *start, char *target, int *index)
{
	if (start == NULL)
	{
		return 0;	//return 0 if not found
	}

	int counter = 0;
	Directory_List *cur = start;
	while(strcmp(cur->list_phone_number, target) != 0)
	{
		if (cur->next == NULL) 	//if cur comes to the end of string and do not found, return 0
		{
			return 0;
		}
		else
		{
			cur = cur->next; 	//go to next node
			counter++;
		}
	}
	*index = counter; 	//store the index of the targrt string
	return 1; 	//return 1 if found
}

void free_pointers(char *point1, char *point2, char *point3)
{
	free(point1); 	//free three points and let they points to null
	free(point2);
	free(point3);
	point1 = NULL;
	point2 = NULL;
	point3 = NULL;
}

void add_directory(Directory_List **start, int *index)
{
	char *input_name, *input_phone_number, *input_department; 	//use read_char to read three strings with three points
	read_char("Name: ", &input_name);
	int is_not_empty = read_char("Number: ", &input_phone_number); 	//int is_not_empty is to store the value to represent the string is empty or not
	read_char("Department: ", &input_department);

	if (is_not_empty == 0) 	//if empty, free three points and prompt error
	{
		free_pointers(input_name, input_phone_number, input_department);
		printf("Phone number cannot be empty, ignoring entry.\n");
	}
	else 	//if not empty
	{
		int is_in_directory_index; 	//store index, but in this function, not use
		int is_in_directory = find_index_directory(*start, input_phone_number, &is_in_directory_index); //int is_in_directory to store the value if found or not
		if (is_in_directory == 1) 	//if phone number is in directory, prompt and free points
		{
			printf("Entry already exists, ignoring duplicate entry.\n");
			free_pointers(input_name, input_phone_number, input_department);
		}
		else 	// if phone number is not in directory
		{
			Directory_List *newel = malloc(sizeof(Directory_List)); 	//malloc for a new node
			if(newel == NULL)
			{
				free_pointers(input_name, input_phone_number, input_department);
				allocate_error();
			}

			newel->list_name = malloc(strlen(input_name) + 1);
			if(newel->list_name == NULL)
			{
				free_pointers(input_name, input_phone_number, input_department);
				allocate_error(); 	//check allocate memory
			}
			strcpy(newel->list_name, input_name);

			newel->list_phone_number = malloc(strlen(input_phone_number) + 1);
			if(newel->list_phone_number == NULL)
			{
				free_pointers(input_name, input_phone_number, input_department);
				allocate_error(); 	//check allocate memory
			}		
			strcpy(newel->list_phone_number, input_phone_number);

			newel->list_department = malloc(strlen(input_department) + 1);
			if(newel->list_department == NULL)
			{
				free_pointers(input_name, input_phone_number, input_department);
				allocate_error(); 	//check allocate memory
			}		
			strcpy(newel->list_department, input_department);

			if (*index == 0) //if the directory is empty
			{
				newel->next = *start; 	//new node to be the start node
				*start = newel;
			}
			else
			{
				int counter = 0;
				Directory_List *cur = *start;
				while((*index - 1) > counter) //cur come to the end of the line list
				{
					cur = cur->next;
					counter++;
				}
				newel->next = cur->next; //add the new node to the struct
				cur->next = newel;
			}
			*index = *index + 1;
			free_pointers(input_name, input_phone_number, input_department);
		}
	}
}

void modify_directory_prompt(Directory_List *cur, char **input, const char *name, int *is_not_empty, char *cur_data)
{
	char prompt[100] = "Enter new "; // prompt 
	strcat(prompt, name);
	strcat(prompt, " (return to keep[");
	strcat(prompt, cur_data);
	strcat(prompt, "]): ");
	*is_not_empty = read_char(prompt, input); // store the value to represent input text if empty or not
}

void modify_directory(Directory_List **start)
{
	char *input_phone_number_mod;
	read_char("Enter phone number to modify: ", &input_phone_number_mod);
	int is_in_directory_index;
	int is_in_directory = find_index_directory(*start, input_phone_number_mod, &is_in_directory_index);

	free(input_phone_number_mod);
	input_phone_number_mod = NULL;

	if (is_in_directory == 0) // if do not found the input phone number
	{
		printf("Entry does not exist.\n");
	}
	else
	{
		int counter = 0;
		Directory_List *cur = *start;
		while(is_in_directory_index > counter) // cur come to the index of the found node
		{
			cur = cur->next;
			counter++;
		}

		char *input_name, *input_phone_number, *input_department;
		int is_not_empty_name, is_not_empty_phone_number, is_not_empty_department;

		modify_directory_prompt(cur, &input_name, "name", &is_not_empty_name, cur->list_name);
		modify_directory_prompt(cur, &input_phone_number, "number", &is_not_empty_phone_number, cur->list_phone_number);
		modify_directory_prompt(cur, &input_department, "department", &is_not_empty_department, cur->list_department);

		if (strlen(cur->list_phone_number) == 0) // if phone number is empty
		{
			printf("Phone number cannot be empty, ignoring entry.\n");
			free_pointers(input_name, input_phone_number, input_department);
		}
		else
		{
			int is_in_directory_index_mod;
			int is_in_directory_mod = find_index_directory(*start, input_phone_number, &is_in_directory_index_mod);
			if (is_in_directory_mod == 1 && is_in_directory_index_mod != is_in_directory_index) //if phone number exists and not the current phone number
			{
				printf("Entry already exists, ignoring duplicate entry.\n");
			}
			else
			{
				if (is_not_empty_name == 1) // if not empty, store in the current node to modify the information
				{
					cur->list_name = realloc(cur->list_name, strlen(input_name) + 1); 
					if(cur->list_name == NULL)
					{
						free_pointers(input_name, input_phone_number, input_department);
						allocate_error(); 	//check allocate memory
					}
					strcpy(cur->list_name, input_name);
				}
				if (is_not_empty_phone_number == 1)
				{
					cur->list_phone_number = realloc(cur->list_phone_number, strlen(input_phone_number) + 1);
					if(cur->list_phone_number == NULL)
					{
						free_pointers(input_name, input_phone_number, input_department);
						allocate_error(); 	//check allocate memory
					}
					strcpy(cur->list_phone_number, input_phone_number);
				}
				if (is_not_empty_department == 1)
				{
					cur->list_department = realloc(cur->list_department, strlen(input_department) + 1);
					if(cur->list_department == NULL)
					{
						free_pointers(input_name, input_phone_number, input_department);
						allocate_error(); 	//check allocate memory
					}
					strcpy(cur->list_department, input_department);
				}
			}
			free_pointers(input_name, input_phone_number, input_department);
		}
	}
}

void delete_directory(Directory_List **start, int *index)
{
	char *input_phone_number_del;
	int is_not_empty = read_char("Enter phone number to delete (return to cancel): ", &input_phone_number_del);

	if (is_not_empty == 1) // if input phone number is not empty
	{
		int is_in_directory_index;
		int is_in_directory = find_index_directory(*start, input_phone_number_del, &is_in_directory_index);
		
		if (is_in_directory == 1) // if input phone number is in directory
		{
			int counter = 0;
			Directory_List *cur = *start;

			if (is_in_directory_index == 0) // if the found node is the first node in the list
			{
				*start = cur->next; //let next node to be the first node
				free(cur->list_name);
				free(cur->list_phone_number);
				free(cur->list_department);
				free(cur);
			}
			else
			{
				while(is_in_directory_index - 1 > counter) //cur come to the left of the found node
				{
					cur = cur->next;
					counter++;
				}
				
				free((cur->next)->list_name);
				free((cur->next)->list_phone_number);
				free((cur->next)->list_department);
				free(cur->next);

				if ((cur->next)->next == NULL) //if found node is not the end of the list 
				{
					cur->next = NULL;  //let cur->next to null byte
				}
				else 	//if found node is the end of the list 
				{
					cur->next = (cur->next)->next; // let cur->next to be found node->next
				}
			}
			*index = *index - 1; //if successfully delete, current number in directory -1
		}
	}
	free(input_phone_number_del);
	input_phone_number_del = NULL;
}

int is_in_text(char *directory_text, char *target_text)
{
	int length_directory_text = strlen(directory_text); //length of string in directory
	int length_target_text = strlen(target_text); 	//length of target string
	if (length_directory_text != 0 && length_target_text != 0) //if both strings are not empty
	{
		int position = 0;
		while(position < length_directory_text) // search in string in directory
		{
			if (directory_text[position] == target_text[0]) //if first character is same as the character in string in that position
			{
				if (length_directory_text - position - 1 >= length_target_text - 1) //if the length of string is enough to compare to target
				{
					int correct_number = 0;
					for (int i = 0; i < length_target_text; i++) //compare in every element in target string
					{
						if (directory_text[position + i] == target_text[i])
						{
							correct_number++;
						}
					}
					if (correct_number == length_target_text) // if is a sub-string, return 1
					{
						return 1;
					}
				}
			}
			position++;
		}
	}
	return 0;	//if not a sub-string, return 0
}

void search_directory(Directory_List *start)
{
	char *input_search_text;
	int is_not_empty = read_char("Search: ", &input_search_text);

	if (is_not_empty == 1) //if input string is not empty
	{
		Directory_List *cur = start;
		while(cur != NULL) // search the whole list until cur to the end of the list
		{
			//if find any sub-string in a node, print the whole node
			if (is_in_text(cur->list_name, input_search_text) == 1 || is_in_text(cur->list_phone_number, input_search_text) == 1 || is_in_text(cur->list_department, input_search_text) == 1)
			{
				printf("%s\t", cur->list_phone_number);
				printf("%s ", cur->list_name);
				printf("(%s)\n", cur->list_department);
			}
			cur = cur->next;
		}
	}
	free(input_search_text);
	input_search_text = NULL;
}

void free_memory(Directory_List *start)
{
	Directory_List *cur = start; //free the whole structure
	while(cur != NULL)
	{
		free(cur->list_name);
		free(cur->list_phone_number);
		free(cur->list_department);
		free(cur);
		cur = cur->next;
	}
}

int main(int argc, char *argv[])
{
	int option_number, current_directory_number = 0;
	Directory_List *start = NULL;
	do
	{
		print_menu();

		if(!read_int("Option: ", &option_number)) 	//if input invalid text, op = -1 to exit the do loop
		{
			option_number = -1;
		}

		if (option_number == 1)
		{
			print_directory(start);
		}
		else if (option_number == 2)
		{
			add_directory(&start, &current_directory_number);
		}
		else if (option_number == 3)
		{
			modify_directory(&start);
		}
		else if (option_number == 4)
		{
			delete_directory(&start, &current_directory_number);
		}
		else if (option_number == 5)
		{
			search_directory(start);
		}
		else if (option_number == 6)
		{
			//do noting
		}
		else
		{
			printf("Unknown option!\n");
		}
	}while(option_number != 6);

	free_memory(start); 	//free all spaces in the structure
	exit(0);
}
