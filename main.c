#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include "hash.h"
#include "bitmap.h"
#include "list.h"
#include "limits.h"
#include "round.h"
#include "debug.h"

typedef struct list_info{
	char list_name[100];
	struct list make_list;
	int list_used;
}list_info;

typedef struct hash_info{
	char hash_name[100];
	struct hash make_hash;
	int hash_used;
}hash_info;

typedef struct bitmap_info{
	char bitmap_name[100];
	struct bitmap *make_bitmap;
	int bitmap_used;
}bitmap_info;

typedef struct all_info{
	list_info *l_info;
	hash_info *h_info;
	bitmap_info *b_info;
}all_info;

typedef struct list_data{
	struct list_elem link;
	int data;
}list_data;

typedef struct hash_data{
	struct hash_elem link;
	int data;
}hash_data;

void list_swap(struct list_elem *a, struct list_elem *b);
void list_shuffle(struct list *list);
unsigned hash_int_2(int i);
int chk_str(char str[],char str_tok[][100],all_info *);
struct list *find_list_name(list_info*,char*);
struct hash *find_hash_name(hash_info*,char*);
struct bitmap *find_bitmap_name(bitmap_info*,char*);
bool list_less(const struct list_elem *a, const struct list_elem *b, void *aux);
bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux);
unsigned hash_hash(const struct hash_elem *a, void *aux);
void hash_action_triple(const struct hash_elem *a, void *aux);
void hash_action_square(const struct hash_elem *a, void *aux);

int main()
{
	int i;
	char str[100],str_tok[10][100];	
	all_info a_info;
	list_info l_info[10];
	hash_info h_info[10];
    bitmap_info b_info[10];

	for(i=0; i<10; i++)
	{
		l_info[i].list_used = 0;
		h_info[i].hash_used = 0;
		b_info[i].bitmap_used = 0;
	}
	a_info.l_info = l_info;
	a_info.h_info = h_info;
	a_info.b_info = b_info;
	while(1)
	{
		fgets(str,sizeof(str),stdin);
		if(chk_str(str,str_tok,&a_info)) break;
		for(i=0; i<10; i++) memset(str_tok[i],0,strlen(str_tok[i])+1);
	}
	return 0;
}

int chk_str(char str[],char str_tok[][100],all_info *a_info)
{
	int i;
	char *temp;
	if(strlen(str) == '1') return 0;
	
	for(i=0; i<strlen(str); i++)
	{
		if(str[i] == '\n') str[i] = '\0';
	}
	
	i=0;
	while(1)
	{
		if(i==0)
			temp = strtok(str," \t\n");
		else
			temp = strtok(NULL," \t\n");
		if(temp == NULL) break;
		else
		{
			strcpy(str_tok[i],temp);
			i++;
		}
	}	

	if(!strcmp(str,"quit")) return 1;
	else if(!strcmp(str_tok[0],"create"))
	{
		if(!strcmp(str_tok[1],"list"))
		{
			for(i=0; i<10; i++)
			{
				if((a_info->l_info[i]).list_used == 0)
				{
					strcpy((a_info->l_info[i]).list_name,str_tok[2]);
					(a_info->l_info[i]).list_used=1;
					list_init(&(a_info->l_info[i]).make_list);
					break;
				}
			}
		}
		else if(!strcmp(str_tok[1],"hashtable"))
		{
			for(i=0; i<10; i++)
			{
				if((a_info->h_info[i]).hash_used == 0)
				{
					strcpy((a_info->h_info[i]).hash_name,str_tok[2]);
					(a_info->h_info[i]).hash_used=1;
					hash_init(&(a_info->h_info[i]).make_hash,hash_hash,hash_less,NULL);
					break;
				}						
			}			
		}
		else if(!strcmp(str_tok[1],"bitmap"))
		{
			for(i=0; i<10; i++)
			{
				if((a_info->b_info[i]).bitmap_used == 0)
				{
					strcpy((a_info->b_info[i]).bitmap_name,str_tok[2]);
					(a_info->b_info[i]).bitmap_used=1;
					(a_info->b_info[i]).make_bitmap=bitmap_create(atoi(str_tok[3]));
					break;		
				}
			}
		}
	}
	else if(!strcmp(str_tok[0],"dumpdata"))
	{
		int i,j;
		for(i=0; i<10; i++)
		{
			if(!strcmp(str_tok[1],(a_info->l_info[i]).list_name))
			{
				struct list_elem *l_temp = list_begin(&((a_info->l_info[i]).make_list));
				while(1)
				{
					if(l_temp == list_end(&((a_info->l_info[i]).make_list)))	break;

					printf("%d ", list_entry(l_temp,list_data,link)->data);
					l_temp=l_temp->next;
				}
			}
			else if(!strcmp(str_tok[1],(a_info->h_info[i]).hash_name))
			{
				struct hash_iterator h;
				hash_first(&h,&(a_info->h_info[i].make_hash));
				while(hash_next(&h))
				{
					printf("%d ", hash_entry(hash_cur(&h),hash_data,link)->data);
				}
				break;
			}
			else if(!strcmp(str_tok[1],(a_info->b_info[i]).bitmap_name))
			{
				const struct bitmap *b_temp = find_bitmap_name(a_info->b_info,str_tok[1]);
				for(j=0; j<bitmap_size(b_temp); j++)
				{
					printf("%d",bitmap_test(b_temp,j));
				}
			}
		}
		printf("\n");
	}
	else if(!strcmp(str_tok[0],"list_insert"))
	{

		int i;
		struct list *temp = find_list_name(a_info->l_info,str_tok[1]);
		struct list_elem *temp_elem = &(temp->head);

		list_data *new = (list_data*)malloc(sizeof(list_data));
		new->data=atoi(str_tok[3]);

		temp_elem = temp_elem->next;
		for(i=0; i<atoi(str_tok[2]);i++)
		{
			temp_elem = temp_elem->next;
		}
		list_insert(temp_elem,&(new->link));
	}	
	else if(!strcmp(str_tok[0],"list_splice"))
	{
		
		int i;
		struct list *temp = find_list_name(a_info->l_info,str_tok[1]);
		struct list *temp2 = find_list_name(a_info->l_info,str_tok[3]);
		struct list_elem *temp_elem = &(temp->head);
		struct list_elem *temp_elem2 = &(temp2->head);
		struct list_elem *temp_elem3 = &(temp2->head);

		temp_elem = temp_elem->next;
		temp_elem2 = temp_elem2->next;
		temp_elem3 = temp_elem3->next;
		for(i=0; i<atoi(str_tok[2]);i++)
		{
			temp_elem = temp_elem->next;
		}	
		for(i=0; i<atoi(str_tok[4]);i++)
		{
			temp_elem2 = temp_elem2->next;
		}
		for(i=0; i<atoi(str_tok[5]);i++)
		{
			temp_elem3 = temp_elem3->next;
		}
		list_splice(temp_elem,temp_elem2,temp_elem3);	
	}
	else if(!strcmp(str_tok[0],"list_push_front"))
	{
		list_data *new = (list_data*)malloc(sizeof(list_data));
		new->data=atoi(str_tok[2]);
		list_push_front(find_list_name(a_info->l_info,str_tok[1]),&(new->link));
	}
	else if(!strcmp(str_tok[0],"list_push_back"))
	{
		list_data *new = (list_data*)malloc(sizeof(list_data));
		new->data=atoi(str_tok[2]);
		list_push_back(find_list_name(a_info->l_info,str_tok[1]),&(new->link));
	}
	else if(!strcmp(str_tok[0],"list_remove"))
	{
		int i;
		struct list *temp = find_list_name(a_info->l_info,str_tok[1]);
		struct list_elem *temp_elem = &(temp->head);
	    
		temp_elem = temp_elem->next;
		for(i=0; i<atoi(str_tok[2]);i++)
		{
			temp_elem = temp_elem->next;
		}
		list_remove(temp_elem);
	}	
	else if(!strcmp(str_tok[0],"list_pop_front"))
	{
		list_pop_front(find_list_name(a_info->l_info,str_tok[1]));
	}	
	else if(!strcmp(str_tok[0],"list_pop_back"))
	{
		list_pop_back(find_list_name(a_info->l_info,str_tok[1]));
	}	
	else if(!strcmp(str_tok[0],"list_front"))
	{
		struct list_elem *temp = list_front(find_list_name(a_info->l_info,str_tok[1]));
		printf("%d\n", list_entry(temp,list_data,link)->data);
	}	
	else if(!strcmp(str_tok[0],"list_back"))
	{
		struct list_elem *temp = list_back(find_list_name(a_info->l_info,str_tok[1]));
		printf("%d\n", list_entry(temp,list_data,link)->data);
	}	
	else if(!strcmp(str_tok[0],"list_size"))
	{	
		printf("%d\n", list_size(find_list_name(a_info->l_info,str_tok[1])));
	}	
	else if(!strcmp(str_tok[0],"list_empty"))
	{
		if(list_empty(find_list_name(a_info->l_info,str_tok[1])))
			printf("true\n");
		else
			printf("false\n");
	}	
	else if(!strcmp(str_tok[0],"list_reverse"))
	{
		list_reverse(find_list_name(a_info->l_info,str_tok[1]));
	}	
	else if(!strcmp(str_tok[0],"list_sort"))
	{
		list_sort(find_list_name(a_info->l_info,str_tok[1]),list_less,NULL);
	}	
	else if(!strcmp(str_tok[0],"list_insert_ordered"))
	{
		list_data *new = (list_data*)malloc(sizeof(list_data));
		new->data=atoi(str_tok[2]);
		list_insert_ordered(find_list_name(a_info->l_info,str_tok[1]),&(new->link),list_less,NULL);
	}	
	else if(!strcmp(str_tok[0],"list_unique"))
	{
		if(str_tok[2][0] != 0)
			list_unique(find_list_name(a_info->l_info,str_tok[1]),find_list_name(a_info->l_info,str_tok[2]),list_less,NULL);
		else
			list_unique(find_list_name(a_info->l_info,str_tok[1]),NULL,list_less,NULL);
	}	
	else if(!strcmp(str_tok[0],"list_max"))
	{
		struct list_elem *temp = list_max(find_list_name(a_info->l_info,str_tok[1]),list_less,NULL);
		printf("%d\n", list_entry(temp,list_data,link)->data);
	}	
	else if(!strcmp(str_tok[0],"list_min"))
	{
		struct list_elem *temp = list_min(find_list_name(a_info->l_info,str_tok[1]),list_less,NULL);
		printf("%d\n", list_entry(temp,list_data,link)->data);
	}
	else if(!strcmp(str_tok[0],"list_swap"))
	{
		struct list *temp = find_list_name(a_info->l_info,str_tok[1]);
		struct list_elem *temp_elem1 = &(temp->head);
		struct list_elem *temp_elem2 = &(temp->head);
	    
		temp_elem1 = temp_elem1->next;
		temp_elem2 = temp_elem2->next;
		
		for(i=0; i<atoi(str_tok[2]);i++)
		{
			temp_elem1 = temp_elem1->next;
		}

		for(i=0; i<atoi(str_tok[3]);i++)
		{
			temp_elem2 = temp_elem2->next;
		}

		list_swap(temp_elem1,temp_elem2);
	}
	else if(!strcmp(str_tok[0],"list_shuffle"))
	{
		list_shuffle(find_list_name(a_info->l_info,str_tok[1]));
	}
	else if(!strcmp(str_tok[0],"hash_insert"))
	{
		hash_data *new = (hash_data*)malloc(sizeof(hash_data));
		new->data=atoi(str_tok[2]);

		hash_insert(find_hash_name(a_info->h_info,str_tok[1]),&(new)->link);
	}
	else if(!strcmp(str_tok[0],"hash_replace"))
	{
		hash_data *new = (hash_data*)malloc(sizeof(hash_data));
		new->data=atoi(str_tok[2]);

		hash_replace(find_hash_name(a_info->h_info,str_tok[1]),&(new->link));
	}
	else if(!strcmp(str_tok[0],"hash_find"))
	{
		hash_data *new = (hash_data*)malloc(sizeof(hash_data));
		new->data = atoi(str_tok[2]);
		
		struct hash_elem *temp = hash_find(find_hash_name(a_info->h_info,str_tok[1]),&(new->link));
		if(temp != NULL)
			printf("%d\n", hash_entry(temp,hash_data,link)->data);
	}
	else if(!strcmp(str_tok[0],"hash_delete"))
	{
		hash_data *new = (hash_data*)malloc(sizeof(hash_data));
		new->data = atoi(str_tok[2]);
		
		hash_delete(find_hash_name(a_info->h_info,str_tok[1]),&(new->link));
	}
	else if(!strcmp(str_tok[0],"hash_clear"))
	{
		hash_clear(find_hash_name(a_info->h_info,str_tok[1]),NULL);
	}
	else if(!strcmp(str_tok[0],"hash_size"))
	{
		printf("%d\n", hash_size(find_hash_name(a_info->h_info,str_tok[1])));
	}
	else if(!strcmp(str_tok[0],"hash_empty"))
	{
		if(hash_empty(find_hash_name(a_info->h_info,str_tok[1])))
			printf("true\n");
		else
			printf("false\n");
	}
	else if(!strcmp(str_tok[0],"hash_apply"))
	{
		if(!strcmp(str_tok[2],"triple"))
			hash_apply(find_hash_name(a_info->h_info,str_tok[1]),hash_action_triple);
		else
			hash_apply(find_hash_name(a_info->h_info,str_tok[1]),hash_action_square);
	}
	else if(!strcmp(str_tok[0],"bitmap_size"))
	{
		printf("%d\n",bitmap_size(find_bitmap_name(a_info->b_info,str_tok[1])));
	}
	else if(!strcmp(str_tok[0],"bitmap_set"))
	{
		int value=0;
		if(!strcmp(str_tok[3],"true"))
			value=1;
		bitmap_set(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),value);
	}
	else if(!strcmp(str_tok[0],"bitmap_mark"))
	{
		bitmap_mark(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]));
	}
	else if(!strcmp(str_tok[0],"bitmap_reset"))
	{
		bitmap_reset(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]));
	}
	else if(!strcmp(str_tok[0],"bitmap_flip"))
	{
		bitmap_flip(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]));
	}
	else if(!strcmp(str_tok[0],"bitmap_test"))
	{
		int result=bitmap_test(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]));
		if(result)
			printf("true\n");
		else
			printf("false\n");
	}
	else if(!strcmp(str_tok[0],"bitmap_set_all"))
	{
		if(!strcmp(str_tok[2],"true"))
			bitmap_set_all(find_bitmap_name(a_info->b_info,str_tok[1]),1);
		else	
			bitmap_set_all(find_bitmap_name(a_info->b_info,str_tok[1]),0);
	}
	else if(!strcmp(str_tok[0],"bitmap_set_multiple"))
	{
		if(!strcmp(str_tok[4],"true"))
			bitmap_set_multiple(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),1);
		else
			bitmap_set_multiple(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),0);
	}
	else if(!strcmp(str_tok[0],"bitmap_count"))
	{
		if(!strcmp(str_tok[4],"true"))
			printf("%d\n", bitmap_count(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),1));
		else
			printf("%d\n", bitmap_count(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),0));
	}
	else if(!strcmp(str_tok[0],"bitmap_contains"))
	{
		int result=0;
		if(!strcmp(str_tok[4],"true"))
		{
			result = bitmap_count(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),1);
			printf("%s\n", result? "true":"false");
		}
		else
		{
			result = bitmap_count(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),0);
			printf("%s\n", result? "true":"false");
		}
	}
	else if(!strcmp(str_tok[0],"bitmap_any"))
	{	
		int result = bitmap_any(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]));
		printf("%s\n", result? "true":"false");
	}
	else if(!strcmp(str_tok[0],"bitmap_none"))
	{
		int result = bitmap_none(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]));
		printf("%s\n", result? "true":"false");
	}
	else if(!strcmp(str_tok[0],"bitmap_all"))
	{
		int result = bitmap_all(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]));
		printf("%s\n", result? "true":"false");
	}
	else if(!strcmp(str_tok[0],"bitmap_scan"))
	{	
		if(!strcmp(str_tok[4],"true"))
			printf("%u\n", bitmap_scan(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),1));
		else
			printf("%u\n", bitmap_scan(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),0));
	}
	else if(!strcmp(str_tok[0],"bitmap_scan_and_flip"))
	{
		if(!strcmp(str_tok[4],"true"))
			printf("%u\n", bitmap_scan_and_flip(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),1));
		else
			printf("%u\n", bitmap_scan_and_flip(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]),atoi(str_tok[3]),0));
	}
	else if(!strcmp(str_tok[0],"bitmap_dump"))
	{
		bitmap_dump(find_bitmap_name(a_info->b_info,str_tok[1]));	
	}
	else if(!strcmp(str_tok[0],"bitmap_expand"))
	{
		bitmap_expand(find_bitmap_name(a_info->b_info,str_tok[1]),atoi(str_tok[2]));
	}
	return 0;
}

struct list *find_list_name(list_info *list, char *name)
{
	int i;
	for(i=0; i<10; i++)
	{
		if(!strcmp(list[i].list_name,name))
			break;
	}
	return &(list[i].make_list);
}

struct hash *find_hash_name(hash_info *hash, char*name)
{
	int i;
	for(i=0; i<10; i++)
	{
		if(!strcmp(hash[i].hash_name,name))
			break;
	}
	return &(hash[i].make_hash);
}

struct bitmap *find_bitmap_name(bitmap_info *bitmap, char *name)
{
	int i;
	for(i=0; i<10; i++)
	{
		if(!strcmp(bitmap[i].bitmap_name,name))
			break;
	}
	return bitmap[i].make_bitmap;
}

bool list_less(const struct list_elem *a, const struct list_elem *b, void *aux)
{
	return list_entry(a,list_data,link)->data < list_entry(b,list_data,link)->data;
}

bool hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
	return hash_entry(a, hash_data,link)->data < hash_entry(b,hash_data,link)->data; 
}

unsigned hash_hash(const struct hash_elem *a, void *aux)
{
	return hash_int(hash_entry(a,hash_data,link)->data);
}

void hash_action_triple(const struct hash_elem *a, void *aux)
{
	int i = hash_entry(a,hash_data,link)->data;
	hash_entry(a,hash_data,link)->data=i*i*i;
}

void hash_action_square(const struct hash_elem *a, void *aux)
{
	int i = hash_entry(a,hash_data,link)->data;
	hash_entry(a,hash_data,link)->data=i*i;
}

void list_swap(struct list_elem *a, struct list_elem *b)
{
	struct list_elem *temp;

		a->next->prev=b;
		a->prev->next=b;
		b->next->prev=a;
		b->prev->next=a;

		temp=a->prev;
		a->prev=b->prev;
		b->prev=temp;

		temp=a->next;
		a->next=b->next;
		b->next=temp;

}

void list_shuffle(struct list *list)
{
	int size=list_size(list);
	int i,rand1,rand2;
	struct list_elem *temp1;
	struct list_elem *temp2;
	
	srand(time(NULL));
	if(size > 1)
	{
		for(i=0; i<size; i++)
		{
			srand(time(NULL));
			rand1=rand()%(size-1);
			srand(time(NULL));
			rand2=rand()%(size-1);
			
			temp1=list_begin(list);
			temp2=list_begin(list);

			for(i=1; i<=rand1; i++)
				temp1=list_next(temp1);
			for(i=1; i<=rand2; i++)
				temp2=list_next(temp2);

			list_swap(temp1,temp2);

		}
	}	
}

unsigned hash_int_2(int i)
{
	i=i*7%4;
	return hash_bytes(&i, sizeof i);
}
