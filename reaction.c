#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	int h_atoms;
	struct condition *new_h_arrival;
	struct condition *reaction_occurred_h;
	struct lock *lock;
};

/**/
void reaction_init(struct reaction *reaction)
{
	reaction->new_h_arrival = malloc(sizeof(struct condition));
	reaction->reaction_occurred_h = malloc(sizeof(struct condition));
	reaction->lock = malloc(sizeof(struct lock));
	cond_init(reaction->new_h_arrival);
	cond_init(reaction->reaction_occurred_h);
	lock_init(reaction->lock);

	reaction->h_atoms = 0;
}

/* Invoked when H atom is ready to react. The function must delay until there 
 * are at least two H atoms and one O atom present, and then exactly one of 
 * the functions must call the procedure make_water 
 */
void reaction_h(struct reaction *reaction)
{
	lock_acquire(reaction->lock);
	reaction->h_atoms++;
	cond_signal(reaction->new_h_arrival,reaction->lock);
	cond_wait(reaction->reaction_occurred_h,reaction->lock);
	lock_release(reaction->lock);
}

void reaction_o(struct reaction *reaction)
{
	lock_acquire(reaction->lock);
	while(reaction->h_atoms < 2) {
		cond_wait(reaction->new_h_arrival,reaction->lock);
	}

	//enough atoms present to make a water molecule
	make_water();
	reaction->h_atoms-=2;
	cond_signal(reaction->reaction_occurred_h,reaction->lock);
	cond_signal(reaction->reaction_occurred_h,reaction->lock);
	lock_release(reaction->lock);
}