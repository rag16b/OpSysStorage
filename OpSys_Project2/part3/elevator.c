#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/delay.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

#define ENTRY_NAME "elevator"
#define ENTRY_SIZE 1000
#define PERMS 0644
#define PARENT NULL
static struct file_operations fops;

struct task_struct* elevator_thread;
static char *message;
static int read_p;
int passengersUnloaded = 0;

/* Structure that holds Elevator Information below */
typedef enum { OFFLINE, IDLE, LOADING, UP, DOWN } ElevatorState;
struct Elevator
{
	int currentFloor;
	int numPassengers;
	int passWeight;
	int isFull;
	ElevatorState state;
};
struct Elevator elevator;

/* Linked List for loaded people below */
struct loadedPass
{
	int dFloor;
	int sFloor;
	int unit;
	int weight;
	int pType; // Adult = 1, Child = 2, RS = 3, BH = 4
	struct loadedPass * next;
};
typedef struct loadedPass *t_load;
t_load lhead; // initialize head node for elevator list

/* Linked List for waiting people below */
struct Passenger
{
	int dFloor;
	int sFloor;
	int unit;
	int weight;
	int pType; // Adult = 1, Child = 2, RS = 3, BH = 4
	struct Passenger * next;
};
typedef struct Passenger *t_pass;
t_pass phead; // initilize head node

// create blank node for elevator
t_load createElevPass(void)
{
	t_load temp; // create new node temp
	temp = (t_load) kmalloc(sizeof(struct loadedPass), __GFP_RECLAIM); // allocate memory
	temp->next = NULL; // make next point to NULL
	return temp; // return new node
}

// Move the elevator from floor to floor
void moveElevator(void)
{
	if(elevator.currentFloor < lhead->dFloor)
	{
		elevator.state = UP;
		ssleep(2);
		elevator.currentFloor++;
	}
	if(elevator.currentFloor > lhead->dFloor)
	{
		elevator.state = DOWN;
		ssleep(2);
		elevator.currentFloor--;
	}
}

// Delete node from Elevator Queue
void deleteLNode(t_load DeleteIt)
{
    // When node to be deleted is head node
    if(lhead == DeleteIt)
    {
        if(lhead->next == NULL)
        {
            return;
        }
        /* Copy the data of next node to head */
        lhead->sFloor = lhead->next->sFloor;
        lhead->dFloor = lhead->next->dFloor;
        lhead->unit = lhead->next->unit;
        lhead->weight = lhead->next->weight;
        // store address of next node
        DeleteIt = lhead->next;
        // Remove the link of next node
        lhead->next = lhead->next->next;
        // free memory
        kfree(DeleteIt);
        return;
    }
}

// Used to delete a node from Pass Queue when transfering to Elevator LL
void deletePNode(t_pass DeleteIt)
{
	t_pass prev = phead;
    // When node to be deleted is head node
    if(phead == DeleteIt)
    {
        if(phead->next == NULL)
        {
            return;
        }
        /* Copy the data of next node to head */
        phead->sFloor = phead->next->sFloor;
        phead->dFloor = phead->next->dFloor;
        phead->unit = phead->next->unit;
        phead->weight = phead->next->weight;
        // store address of next node
        DeleteIt = phead->next;
        // Remove the link of next node
        phead->next = phead->next->next;
        // free memory
        kfree(DeleteIt);
        return;
    }
    while(prev->next != NULL && prev->next != DeleteIt)
        prev = prev->next;
    // Check if node really exists in Linked List
    if(prev->next == NULL)
    {
        return;
    }
    // Remove node from Linked List
    prev->next = prev->next->next;
    // Free memory
    kfree(DeleteIt);
    return;
}

// Unlod people from elevator
void unloadElevator(void)
{
	while(lhead != NULL)
	{
		if(lhead->dFloor == elevator.currentFloor)
		{
			elevator.isFull = 0;
			elevator.passWeight = elevator.passWeight - lhead->weight;
			elevator.numPassengers = elevator.numPassengers - lhead->unit;
			deleteLNode(lhead);
			passengersUnloaded++;
		}
		else
			return;
	}
}

// function to take the node from the waiting line and append to those in elevator
void loadElev(void)
{
		t_pass q; // place holder for phead
		t_load temp, p;
		int nodeFound = 0; // flag to check if node was found
		q = phead;

		while(q != NULL && nodeFound == 0)
		{
			if(q->sFloor == elevator.currentFloor) // if current node's floor = elevator's floor
			{
				if((elevator.passWeight + q->weight > 30) || elevator.numPassengers + q->unit > 10)
				{
					elevator.isFull = 1;
					ssleep(1);
					return;
				}

				temp = createElevPass(); 	// creates new node for elevator
				temp->pType  	= q->pType; // transfer node data
				temp->sFloor 	= q->sFloor;
				temp->dFloor 	= q->dFloor;
				temp->weight 	= q->weight;
				temp->unit 		= q->unit;

				// adds weight and units to elevator
				elevator.passWeight = elevator.passWeight + temp->weight;
				elevator.numPassengers = elevator.numPassengers + temp->unit;

				if(lhead == NULL)  // checks to see if head exists, if not it creates head
				{
					lhead = temp;
				}
				else  // if head exists, new node appended to end
				{
					p = lhead;
					while(p->next != NULL)
					{
						p = p->next;
					}
					p->next = temp;
				}

				deletePNode(q);
				nodeFound = 1;
			}
			q = q->next;
		}
}

// create blank passenger node
t_pass createPass(void)
{
	t_pass temp; // create new node temp
	temp = (t_pass) kmalloc(sizeof(struct Passenger), __GFP_RECLAIM); // allocate memory
	temp->next = NULL; // make next point to NULL
	return temp; // return new node
}

// appends the passenger to the end of the list, returns the head node.
t_pass addPass(t_pass head, int dest, int start, int pers)
{
	t_pass temp, p; // declare two nodes
	temp = createPass(); 	// Creates blank pass node
	temp->pType = pers; 	// assigns person type
	temp->dFloor = dest; 	// assigns destination floor
	temp->sFloor = start; 	// assigns start floor

	if (pers == 1) {temp->weight = 2, temp->unit = 1;} // if Adult
	if (pers == 2) {temp->weight = 1, temp->unit = 1;} // if Child
	if (pers == 3) {temp->weight = 4, temp->unit = 2;} // if RS
	if (pers == 4) {temp->weight = 8, temp->unit = 2;} // if BH

	if(head == NULL)
	{
		head = temp;
	}
	else
	{
		p = head; //assign head to
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = temp;
	}
	return head;
}


// traverses passenger list and prints the nodes
void travPlist(void)
{
	int c0unt = 0;
	t_pass q;
	q = phead;
	while(q != NULL)
	{
		printk(KERN_NOTICE "Person %d: Type- %d, Start- %d, Dest- %d, Units- %d, Weight- %d\n",
		c0unt, q->pType, q->sFloor, q->dFloor, q->unit, q->weight);
		q = q->next;
		c0unt++;
	}
}
// traverses elevator list and prints nodes
void travLlist(void)
{
	int c0unt = 0;
	t_load q;
	q = lhead;
	while(q != NULL)
	{
		printk(KERN_NOTICE "ELEVATOR Person %d: Type- %d, Start- %d, Dest- %d, Units- %d, Weight- %d\n",
		c0unt, q->pType, q->sFloor, q->dFloor, q->unit, q->weight);
		q = q->next;
		c0unt++;
	}
}

int elevThread(void *data)
{
	while (!kthread_should_stop())
	{
		int unloaded = 0;
		if (elevator.state != OFFLINE)
		{
			if(elevator.isFull == 0)
			{
				elevator.state = LOADING;
				loadElev();
			}
			else
			{
				travLlist();
				while(elevator.currentFloor != lhead->dFloor && unloaded == 0)
				{
					moveElevator();
					if(elevator.currentFloor == lhead->dFloor)
					{
						unloadElevator();
						//loadElev();
						unloaded = 1;
					}
				}
				//travLlist();
				//travPlist();
			}
		}
	}
	return 0;
}

extern long (*STUB_start_elevator)(void);
long start_elevator(void)
{
	if (elevator.state == OFFLINE) {
		elevator.currentFloor = 1;
		elevator.numPassengers = 0;
		elevator.passWeight = 0;
		elevator.state = IDLE;
		elevator.isFull = 0;
		return 0;
	}
	else if (elevator.state != OFFLINE)
		return 1;
	else
		return -ENOMEM;
}

extern long (*STUB_issue_request)(int, int, int);
long issue_request(int p, int s, int d)
{
	// appends new passenger
	phead = addPass(phead, d, s, p);
	return 0;
}

extern long (*STUB_stop_elevator)(void);
long stop_elevator(void)
{
	// lock so that it cannot issue anymore requests or pick up more people
	// unload current passengers
	elevator.state = OFFLINE;
	return 0;
}

int print_elevator(void)
{
    // this is where we print our entire elevator. follow format below
	int nextFloor = 0;
	char stateString[10];
	char *buf = kmalloc(sizeof(char) * 300, __GFP_RECLAIM);
	if(lhead != NULL)
		nextFloor = lhead->dFloor;
	if (buf == NULL)
	{
		printk(KERN_WARNING "print_elevator");
		return -ENOMEM;
	}

	strcpy(stateString, "");

	if(elevator.state == OFFLINE)
		strcpy(stateString, "OFFLINE");
	if(elevator.state == IDLE)
		strcpy(stateString, "IDLE");
	if(elevator.state == LOADING)
		strcpy(stateString, "LOADING");
	if(elevator.state == UP)
		strcpy(stateString, "UP");
	if(elevator.state == DOWN)
		strcpy(stateString, "DOWN");

	strcpy(message, ""); // initialize message buffer

  sprintf(buf, "Elevator State: %s\nCurrent Floor: %d\nElevator Next Floor: %d\nCurrent Weight: %d\nCurrent Load: %d\nPassengers Serviced: %d\n",
	stateString, elevator.currentFloor, nextFloor, elevator.passWeight, elevator.numPassengers, passengersUnloaded);
	strcat(message, buf);
	kfree(buf);
	return 0;
}


int elevator_proc_open(struct inode *sp_inode, struct file *sp_file)
{
	read_p = 1;
	message = kmalloc(sizeof(char) * ENTRY_SIZE, __GFP_RECLAIM | __GFP_IO | __GFP_FS);
	if (message == NULL)
	{
		printk(KERN_WARNING "elevator_proc_open");
		return -ENOMEM;
	}
	return print_elevator();    // calls the print elevator function
}

ssize_t elevator_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset)
{
	int len = strlen(message);

	read_p = !read_p;
	if (read_p)
		return 0;

	copy_to_user(buf, message, len);
	return len;
}

int elevator_proc_release(struct inode *sp_inode, struct file *sp_file)
{
	// cleans up memory once releasing proc
	kfree(message);
	return 0;
}

static int elevator_init(void)
{
	printk(KERN_NOTICE "/proc/%s create\n",ENTRY_NAME);
	fops.open = elevator_proc_open;
	fops.read = elevator_proc_read;
	fops.release = elevator_proc_release;

	if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops))
	{
		printk(KERN_WARNING "proc create\n");
		remove_proc_entry(ENTRY_NAME, NULL);
		return -ENOMEM;
	}

	STUB_start_elevator = start_elevator; // calls the SystemCalls
	STUB_issue_request  = issue_request;
	STUB_stop_elevator  = stop_elevator;

	// starts the elevator thread
	elevator_thread = kthread_run(elevThread, NULL, "Elevator Thread");

	return 0;
}
module_init(elevator_init);

static void elevator_exit(void)
{
	STUB_start_elevator = NULL; // sets everything to null
	STUB_issue_request  = NULL;
	STUB_stop_elevator  = NULL;

	remove_proc_entry(ENTRY_NAME, NULL);

	// stopping the thread
	kthread_stop(elevator_thread);
}
module_exit(elevator_exit);
