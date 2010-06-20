/*  http://matthewtemple.wordpress.com/the-evolution-of-culture/             */

#include <stdlib.h>

#define GENE_INDEX_DISPLAY 0
#define GENE_INDEX_MEET 1
#define GENE_INDEX_MOVE 2
#define GENE_INDEX_PERSONALITY 3

#define GENOME_LENGTH 256          /*  2^GENOME_ADDRESS_SIZE==GENOME_LENGTH  */
#define GENOME_ADDRESS_SIZE 8
#define MUTATION_INCIDENCE_PER 100000
#define PANDEMIC_MODULUS 16
#define SLEEP_US 41667 * 1
#define SWAP_POSITIONS_MODULUS 2
#define WORLD_WIDTH 80             /*  make this the width of your terminal  */
#define WORLD_HEIGHT 40           /*  make this the height of your terminal  */

#define ORGANISM_COUNT (WORLD_WIDTH * WORLD_HEIGHT) / 4

char red_face[6] = {'o', 'c', 'b', 'd', 'q', 'g'};
char blue_face[6] = {'\'', '_', '-', '/', '=', '#'};
char green_face[6] = {'X', 'K', 'I', 'Z', 'V', 'M'};

struct display_gene_t {
  unsigned int red;
  unsigned int green;
  unsigned int blue;
};
typedef struct display_gene_t display_gene_t;

struct meet_gene_t {
  unsigned int address;
  unsigned int length;
};
typedef struct meet_gene_t meet_gene_t;

struct move_gene_t {
  int offset_x;
  int offset_y;
};
typedef struct move_gene_t move_gene_t;

struct personality_gene_t {
  unsigned int extrovert;
};
typedef struct personality_gene_t personality_gene_t;

struct position_t {
  unsigned int x;
  unsigned int y;
};
typedef struct position_t position_t;

struct organism_t {
  unsigned int *genome;
  position_t position;
  char face;
};
typedef struct organism_t organism_t;

struct world_t {
  organism_t *cells[WORLD_WIDTH][WORLD_HEIGHT];
};
typedef struct world_t world_t;

void create_organism(world_t *world, organism_t *organism);
void create_world(world_t *world);
void destroy_organism(world_t *world, organism_t *organism);
char display_color(organism_t *organism);
void find_empty_position(world_t *world, position_t *position);
unsigned int gene_at_virtual_index(organism_t *organism,
    unsigned int virtual_index);
unsigned int gene_start_address(organism_t *organism, unsigned int gene_index);
void meet_organism(world_t *world, organism_t *organism_a,
    organism_t *organism_b);
void move_organism(world_t *world, organism_t *organism);
void parse_display_gene(organism_t *organism, unsigned int gene_start_address,
    display_gene_t *display_gene);
void parse_meet_gene(organism_t *organism, unsigned int gene_start_address,
    meet_gene_t *meet_gene);
void parse_move_gene(organism_t *organism, unsigned int gene_start_address,
    move_gene_t *move_gene);
void parse_move_gene_2(organism_t *organism, unsigned int gene_start_address,
    move_gene_t *move_gene);
void parse_personality_gene(organism_t *organism,
    unsigned int gene_start_address, personality_gene_t *personality_gene);
unsigned int random_unsigned_int(unsigned int range);
unsigned int unsigned_int_from_genome(organism_t *organism,
    unsigned int gene_start_address, unsigned int gene_length);
unsigned int wrapped_index(int virtual_index, unsigned int range);

void create_organism(world_t *world, organism_t *organism)
{
  position_t position;
  unsigned int gene;

  organism->genome = malloc(sizeof(unsigned int) * GENOME_LENGTH);
  for (gene = 0; gene < GENOME_LENGTH; gene++) {
    organism->genome[gene] = random_unsigned_int(2);
  }

  find_empty_position(world, &position);
  world->cells[position.x][position.y] = organism;
  organism->position = position;
  organism->face = random() % 6;
}

void create_world(world_t *world)
{
  unsigned int x;
  unsigned int y;

  for (x = 0; x < WORLD_WIDTH; x++) {
    for (y = 0; y < WORLD_HEIGHT; y++) {
      world->cells[x][y] = NULL;
    }
  }
}

void destroy_organism(world_t *world, organism_t *organism)
{
  world->cells[organism->position.x][organism->position.y] = NULL;
  free(organism->genome);
}

char display_color(organism_t *organism)
{
  unsigned int display_gene_start_address;
  display_gene_t display_gene;
  char c;

  display_gene_start_address = gene_start_address(organism,
      GENE_INDEX_DISPLAY);
  parse_display_gene(organism, display_gene_start_address, &display_gene);

  if ((display_gene.red > display_gene.blue)
      && (display_gene.red > display_gene.green)) {
    c = 'r';
  } else if ((display_gene.green > display_gene.red)
      && (display_gene.green > display_gene.blue)) {
    c = 'g';
  } else if ((display_gene.blue > display_gene.green)
      && (display_gene.blue > display_gene.red)) {
    c = 'b';
  } else {
    c = 'w';
  }

  return c;
}

void find_empty_position(world_t *world, position_t *position)
{
  unsigned int x;
  unsigned int y;

  do {
    x = random_unsigned_int(WORLD_WIDTH);
    y = random_unsigned_int(WORLD_HEIGHT);
  } while (NULL != world->cells[x][y]);
  position->x = x;
  position->y = y;
}

unsigned int gene_at_virtual_index(organism_t *organism,
    unsigned int virtual_index)
{
  unsigned int actual_index;

  actual_index = wrapped_index(virtual_index, GENOME_LENGTH);
  return organism->genome[actual_index];
}

unsigned int gene_start_address(organism_t *organism, unsigned int gene_index)
{
  unsigned int address_of_gene_header;
  unsigned int start_address = 0;
  unsigned int each_part_of_address;
  unsigned int each_part_of_address_value = 1;

  address_of_gene_header = GENOME_ADDRESS_SIZE * gene_index;
  start_address = unsigned_int_from_genome(organism, address_of_gene_header,
      GENOME_ADDRESS_SIZE);
  return start_address;
}

void meet_organism(world_t *world, organism_t *organism_a,
    organism_t *organism_b)
{
  meet_gene_t meet_gene;
  unsigned int meet_gene_start_address;
  unsigned int each_gene;
  unsigned int each_gene_virtual;
  unsigned int temp_gene;
  position_t temp_position;

  meet_gene_start_address = gene_start_address(organism_a, GENE_INDEX_MEET);
  parse_meet_gene(organism_a, meet_gene_start_address, &meet_gene);

  for (each_gene = 0; each_gene < meet_gene.length; each_gene++) {
    each_gene_virtual = wrapped_index(each_gene, GENOME_LENGTH);
    organism_b->genome[each_gene_virtual]
      = organism_a->genome[each_gene_virtual];
    if (0 == random_unsigned_int(MUTATION_INCIDENCE_PER)
        || (0 == (time(NULL) % PANDEMIC_MODULUS))) {
      organism_b->genome[each_gene_virtual] = random_unsigned_int(2);
    }
  }

  if (0 == (random() % SWAP_POSITIONS_MODULUS)) {
    world->cells[organism_b->position.x][organism_b->position.y] = organism_a;
    world->cells[organism_a->position.x][organism_a->position.y] = organism_b;
    temp_position.x = organism_a->position.x;
    temp_position.y = organism_a->position.y;
    organism_a->position.x = organism_b->position.x;
    organism_a->position.y = organism_b->position.y;
    organism_b->position.x = temp_position.x;
    organism_b->position.y = temp_position.y;
  }
}

void move_organism(world_t *world, organism_t *organism)
{
  move_gene_t move_gene;
  personality_gene_t personality_gene;
  unsigned int move_gene_start_address;
  unsigned int personality_gene_start_address;
  unsigned int current_x;
  unsigned int current_y;
  unsigned int target_x;
  unsigned int target_y;

  move_gene_start_address = gene_start_address(organism, GENE_INDEX_MOVE);
  parse_move_gene(organism, move_gene_start_address, &move_gene);

  personality_gene_start_address = gene_start_address(organism,
      GENE_INDEX_PERSONALITY);
  parse_personality_gene(organism, personality_gene_start_address,
      &personality_gene);

  current_x = organism->position.x;
  current_y = organism->position.y;
  target_x = wrapped_index(current_x + move_gene.offset_x, WORLD_WIDTH);
  target_y = wrapped_index(current_y + move_gene.offset_y, WORLD_HEIGHT);

  if (NULL == world->cells[target_x][target_y]) {
    world->cells[target_x][target_y] = organism;
    world->cells[current_x][current_y] = NULL;
    organism->position.x = target_x;
    organism->position.y = target_y;
  } else {
    if (personality_gene.extrovert) {
      meet_organism(world, organism, world->cells[target_x][target_y]);
    } else {
      target_x = wrapped_index(current_x + (-1 * move_gene.offset_x),
          WORLD_WIDTH);
      target_y = wrapped_index(current_y + (-1 * move_gene.offset_y),
          WORLD_HEIGHT);
      if (NULL == world->cells[target_x][target_y]) {
        world->cells[target_x][target_y] = organism;
        world->cells[current_x][current_y] = NULL;
        organism->position.x = target_x;
        organism->position.y = target_y;
      }
    }
  }
}

void parse_display_gene(organism_t *organism, unsigned int gene_start_address,
    display_gene_t *display_gene)
{
  display_gene->red = unsigned_int_from_genome
    (organism, gene_start_address + 0, 8);
  display_gene->green = unsigned_int_from_genome
    (organism, gene_start_address + 8, 8);
  display_gene->blue = unsigned_int_from_genome
    (organism, gene_start_address + 16, 8);
}

void parse_meet_gene(organism_t *organism, unsigned int gene_start_address,
    meet_gene_t *meet_gene)
{
  meet_gene->address = unsigned_int_from_genome
    (organism, gene_start_address + 0, 8);
  meet_gene->length = unsigned_int_from_genome
    (organism, gene_start_address + 8, 8);
}

void parse_move_gene(organism_t *organism, unsigned int gene_start_address,
    move_gene_t *move_gene)
{
  int offset_x;
  int offset_y;
  unsigned int is_negative_x;
  unsigned int is_negative_y;

  offset_x = unsigned_int_from_genome(organism, gene_start_address + 0, 1);
  is_negative_x = unsigned_int_from_genome
    (organism, gene_start_address + 1, 1);
  offset_y = unsigned_int_from_genome(organism, gene_start_address + 2, 1);
  is_negative_y = unsigned_int_from_genome
    (organism, gene_start_address + 3, 1);

  if (is_negative_x) {
    offset_x *= -1;
  }
  if (is_negative_y) {
    offset_y *= -1;
  }

  move_gene->offset_x = offset_x;
  move_gene->offset_y = offset_y;
}

void parse_personality_gene(organism_t *organism,
    unsigned int gene_start_address, personality_gene_t *personality_gene)
{
  personality_gene->extrovert = unsigned_int_from_genome
    (organism, gene_start_address + 0, 1);
}

unsigned int random_unsigned_int(unsigned int range)
{
  return random() % range;
}

unsigned int unsigned_int_from_genome(organism_t *organism,
    unsigned int gene_start_address, unsigned int gene_length)
{
  unsigned int each_part_of_address;
  unsigned int each_part_of_address_value = 1;
  unsigned int r = 0;
  unsigned int gene_end_address;

  gene_end_address = gene_start_address + gene_length;

  for (each_part_of_address = gene_start_address;
     each_part_of_address < gene_end_address;
     each_part_of_address++) {
    r += each_part_of_address_value
      * gene_at_virtual_index(organism, each_part_of_address);
    each_part_of_address_value *= 2;
  }

  return r;
}

unsigned int wrapped_index(int virtual_index, unsigned int range)
{
  unsigned int wrapped_index;

  if (virtual_index >= (int) range) {
    wrapped_index = virtual_index - range;
  } else if (virtual_index < 0) {
    wrapped_index = range + virtual_index;
  } else {
    wrapped_index = virtual_index;
  }
  return wrapped_index;
}

int main(int argc, char *argv[])
{
  world_t world;
  unsigned int each_organism;
  unsigned int x;
  unsigned int y;
  char c;
  char color;
  char face;
  organism_t organisms[ORGANISM_COUNT];

  srandom(time(NULL));

  create_world(&world);
  for (each_organism = 0; each_organism < ORGANISM_COUNT; each_organism++) {
    create_organism(&world, &organisms[each_organism]);
  }
  while (1) {
    for (each_organism = 0; each_organism < ORGANISM_COUNT;
       each_organism++) {
      move_organism(&world, &organisms[each_organism]);
    }
    putchar('\n');
    for (x = 0; x < WORLD_WIDTH; x++) {
      for (y = 0; y < WORLD_HEIGHT; y++) {
        if (NULL == world.cells[x][y]) {
          face = ' ';
        } else {
          color = display_color(world.cells[x][y]);
          c = world.cells[x][y]->face;
          switch (color) {
            case 'r':
              face = red_face[c];
              break;
            case 'g':
              face = green_face[c];
              break;
            case 'b':
              face = blue_face[c];
              break;
            default:
              break;
          }
        }
        putchar(face);
      }
    }
    usleep(SLEEP_US);
  }

  for (each_organism = 0; each_organism < ORGANISM_COUNT; each_organism++) {
    destroy_organism(&world, &organisms[each_organism]);
  }

  return 0;
}
