/* 
 * Advent of Code: Day 20
 * See README for details
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdbool.h>

typedef long long int scalar;

typedef struct {
	scalar x, y, z;
} coord;

typedef struct {
	coord pos;
	coord vel;
	coord acc;
	bool  dead;
} particle;

static particle particles[1024];
size_t num_particles;

static particle* collision_table[2048];

// Turns out that hash is a pretty useful
// function
unsigned long hash(particle* p) {
	unsigned long hash = 5381;

	hash = (hash * 33) + p->pos.x;
	hash = (hash * 33) + p->pos.y;
	hash = (hash * 33) + p->pos.z;
	
	return hash;
}	

int main(int argc, const char** argv) {
	FILE* in;
	if (argc >= 2) {
		in = fopen(argv[1], "r");
	}
	else {
		in = stdin;
	}


	char* line;
	size_t line_len = 0;
	particle* cur = particles;

	while(NULL != (line = fgetln(in, &line_len))) {
		line[line_len - 1] = '\0';

		if(sscanf(line, "p=<%lld,%lld,%lld>, v=<%lld,%lld,%lld>, a=<%lld,%lld,%lld>",
							&cur->pos.x, &cur->pos.y, &cur->pos.z,
							&cur->vel.x, &cur->vel.y, &cur->vel.z,
							&cur->acc.x, &cur->acc.y, &cur->acc.z) != 9) {
			break;
		}
		
		cur++;
	}
	if (in != stdin) {
		fclose(in);
	}

	num_particles = cur - particles;

	printf("Read %zu particles\n", num_particles);

	particle* end = particles + num_particles;

	size_t num_iterations = 0;

	scalar min_accel = -1;
	particle* slowest_accel = NULL;
	cur = particles;
	while(cur != end) {
		scalar cur_accel = cur->acc.x * cur->acc.x + cur->acc.y * cur->acc.y +
			cur->acc.z * cur->acc.z;
		if (cur_accel <= min_accel || !slowest_accel) {
			slowest_accel = cur;
			min_accel = cur_accel;
			printf("%zu: %lld\n", slowest_accel - particles, min_accel);
		}
		cur++;
	}

	sleep(1);
	
	for (;;) {
		bzero(collision_table, sizeof collision_table);
		
		cur = particles;
		while(cur != end) {
			if (!cur->dead) {
				cur->vel.x += cur->acc.x;
				cur->vel.y += cur->acc.y;
				cur->vel.z += cur->acc.z;

				cur->pos.x += cur->vel.x;
				cur->pos.y += cur->vel.y;
				cur->pos.z += cur->vel.z;

				long unsigned h = hash(cur);
				int table_size = (sizeof collision_table / sizeof collision_table[0]);
				int idx = h % table_size;
				while (collision_table[idx % table_size]) {
					// check for collision
					particle* other = collision_table[idx % table_size];
					if (other->pos.x == cur->pos.x &&
							other->pos.y == cur->pos.y &&
							other->pos.z == cur->pos.z) {
						/* Collision! */
						printf("collision! (%zu,%zu)\n",
									 cur - particles, other - particles);
						other->dead = true;
						cur->dead = true;
						break;
					}
					idx++;
				}
				if (!cur->dead) {
					collision_table[idx % table_size] = cur;
				}
			}
			cur++;
		}
		
		if (!(num_iterations % 1000000)) {
			double min_distance_sq = -1;
			particle* closest = NULL;
			particle* dist_cur = particles;
			while (dist_cur != end) {
				double distance_sq = (double)dist_cur->pos.x * (double)dist_cur->pos.x
					+ (double)dist_cur->pos.y * (double)dist_cur->pos.y
					+ (double)dist_cur->pos.z * (double)dist_cur->pos.z;
				if (distance_sq < min_distance_sq || !closest) {
					min_distance_sq = distance_sq;
					closest = dist_cur;
				}
				dist_cur++;
			}
			if (min_distance_sq < 0) {
				printf ("OVERFLOW\n");
				goto done;
			}
			printf("Particle %zu (%lf)\n", closest - particles, min_distance_sq);
			{
				int num_alive = 0;
				for(int i = 0; i < num_particles; i++) {
					if (!particles[i].dead) num_alive++;
				}
				printf("%d Particles left\n", num_alive);
			}
		}
	}

 done:
	
	return 0;
}

