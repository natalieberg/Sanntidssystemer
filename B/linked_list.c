#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
	list_t newList = malloc(sizeof(list_t));
	newList->head = NULL;
	newList->tail = NULL;
	newList->length = 0;

	return newList;
}

void list_delete(list_t list)
{
	list->head = NULL;
	list->tail = NULL;
}

void list_insert(list_t list, int index, int data)
{
	struct node *newNode = malloc(sizeof(struct node));
	newNode->data = data;

	struct node *current = list->head;

	int i;

	for (i = 0; i < index-1; i++)
	{
		current = current->next;
	}

	newNode->next = current->next;
	current->next = newNode;
	newNode->prev = current;
	(newNode->next)->prev = newNode;

	list->length += 1;

}

void list_append(list_t list, int data)
{
	struct node *newNode = malloc(sizeof(struct node));
	newNode->data = data;		

	if (list->length == 0)
	{
	newNode->next = NULL;
	newNode->prev = NULL;
	list->head = newNode;
	list->tail = newNode;

	} else
	{
	newNode->next = NULL;
	newNode->prev = list->tail;

	(list->tail)->next = newNode;

	list->tail = newNode;

	}

	list->length += 1;

}

void list_print(list_t list)
{	
	struct node *current = list->head;

	if (list->length == 0){
		printf(" ");
	} else
	{ 
		while (current->next != NULL)
		{
			printf("%d ", current->data);
			current = current->next;
		}
		printf("%d ", current->data);
	}
	printf("\n");

}

long list_sum(list_t list)
{
	long sum = 0;
	struct node *current = list->head;

	while (current->next != NULL)
	{
		sum += current->data;
		current = current->next;
	}
	sum += current->data;

	return sum;
}

int list_get(list_t list, int index)
{
	struct node *current = list->head;

	int i;

	for (i = 0; i <= index; i++)
	{
		current = current->next;
	}

	return current->data;
}

int list_extract(list_t list, int index)
{
	struct node *current = list->head;

	int data;
	int i;

	if(list->length == 1)
	{
		data = (list->head)->data;
		list->head = NULL;
		list->tail = NULL;
	}else if (index == list->length)
	{
		data = (list->tail)->data;
		(list->tail)->prev->next = NULL;
		list->tail = (list->tail)->prev;

	} else if (index == 0)
	{
		data = (list->head)->data;
		(list->head)->next->prev = NULL;
		list->head = (list->head)->next;
	} else 
	{

		for (i = 0; i < index; i++)
		{
			current = current->next;
		}

		data = (current->next)->data;
		current->next = (current->next)->next;
		(current->next)->prev = current;
	}
	list->length -= 1;
	return data;
}
