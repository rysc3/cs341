/*
  Ryan Scherbarth 
  November 2024 
  NETID: 101968169
*/

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


typedef struct
{
  int valid;
  long tag;
  int last;
} line;

typedef struct
{
  line *lines;
} set;

typedef struct
{
  set *sets;
  int s, E, b;
  int hits, misses, evicts;
  int time;
} cache;

cache init(int s, int E, int b);
void run(cache *c, const char *trace_file);
void free_cache(cache *c);

int main(int argc, char **argv)
{
  int s = 0, E = 0, b = 0;
  char *trace_file = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1)
  {
    switch (opt)
    {
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      trace_file = optarg;
      break;
    default:
      exit(EXIT_FAILURE);
    }
  }

  cache c = init(s, E, b);
  run(&c, trace_file);
  printSummary(c.hits, c.misses, c.evicts);
  free_cache(&c);

  return 0;
}

cache init(int s, int E, int b)
{
  cache c;
  c.s = s;
  c.E = E;
  c.b = b;
  c.hits = 0;
  c.misses = 0;
  c.evicts = 0;
  c.time = 0;

  int num_sets = 1 << s;
  c.sets = (set *)malloc(num_sets * sizeof(set));

  for (int i = 0; i < num_sets; i++)
  {
    c.sets[i].lines = (line *)malloc(E * sizeof(line));
    for (int j = 0; j < E; j++)
    {
      c.sets[i].lines[j].valid = 0;
      c.sets[i].lines[j].tag = 0;
      c.sets[i].lines[j].last = 0;
    }
  }

  return c;
}

/*
  Frees all the mallocs in the init function.
*/
void free_cache(cache *c)
{
  for (int i = 0; i < 1 << c->s ; i++)
  {
    free(c->sets[i].lines);
  }
  free(c->sets);
}

void run(cache *c, const char *trace_file)
{
  char operation;
  long address;
  int size;
  FILE *file = fopen(trace_file, "r");

  while (fscanf(file, " %c %lx,%d", &operation, &address, &size) == 3)
  {
    // Ignore load instructions
    if (operation == 'I'){ continue; }
    // M is always a hit
    if (operation == 'M'){ c->hits++; }
    c->time++;

    /*
      set_index, we shift off the size of b for the block offset bits. 
      We then need to do a bitwise and so that we can ignore all bits that
      aren't the set index bits. 

      we get the tag by just shifting off both the set index and the 
      block offset bits.
    */
    long set_index = (address >> c->b) & ((1 << c->s) - 1);
    long tag = address >> (c->s + c->b);

    /*
      once we've found the set index and tag, we can just select the correct 
      set usign the set index. 
    */
    set *current_set = &c->sets[set_index];
    int hit = 0, empty_line = -1, lru = 0;
    int replaced = current_set->lines[0].last;

    for (int i = 0; i < c->E; i++)
    {
      if (current_set->lines[i].valid && current_set->lines[i].tag == tag)
      {
        current_set->lines[i].last = c->time;
        hit = 1;
        break;
      }
      if (!current_set->lines[i].valid && empty_line == -1)
        empty_line = i;
      if (current_set->lines[i].last < replaced)
      {
        replaced = current_set->lines[i].last;
        lru = i;
      }
    }

    if (hit)
    {
      c->hits++;
    }
    else
    {
      if (empty_line != -1)
      {
        current_set->lines[empty_line].last = c->time;
        current_set->lines[empty_line].valid = 1;
        current_set->lines[empty_line].tag = tag;
      }
      else
      {
        current_set->lines[lru].tag = tag;
        c->evicts++;
        current_set->lines[lru].last = c->time;
      }
      c->misses++;
    }
  }
  fclose(file);
}