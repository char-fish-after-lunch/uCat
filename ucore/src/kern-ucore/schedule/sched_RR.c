#include <types.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <sched_RR.h>

static void RR_init(struct run_queue *rq)
{
	list_init(&(rq->run_list));
	rq->proc_num = 0;
}

static void RR_enqueue(struct run_queue *rq, struct proc_struct *proc)
{
	assert(list_empty(&(proc->run_link)));
	list_add_before(&(rq->run_list), &(proc->run_link));
	if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
		proc->time_slice = rq->max_time_slice;
	}
	proc->rq = rq;
	rq->proc_num++;
}

static void RR_dequeue(struct run_queue *rq, struct proc_struct *proc)
{
	assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
	list_del_init(&(proc->run_link));
	rq->proc_num--;
}

static struct proc_struct *RR_pick_next(struct run_queue *rq)
{
	list_entry_t *le = list_next(&(rq->run_list));
	if (le != &(rq->run_list)) {
		return le2proc(le, run_link);
	}
	return NULL;
}

static void RR_proc_tick(struct run_queue *rq, struct proc_struct *proc)
{
	if (proc->time_slice > 0) {
		proc->time_slice--;
	}
	if (proc->time_slice == 0) {
		proc->need_resched = 1;
	}
}

static double RR_get_load (struct run_queue * rq) {
    return rq->proc_num;
}

static int RR_get_proc(struct run_queue* rq, struct proc_struct* procs_moved[], int needs)
{
    int num = 0;
    list_entry_t *le = list_next(&(rq->run_list));
	
    while (le != &(rq->run_list)) {
		procs_moved[num++] = le2proc(le, run_link);
        le = list_next(le);
        if (num >= needs)
            break;
	}
    return num;
}

struct sched_class RR_sched_class = {
	.name = "RR_scheduler",
	.init = RR_init,
	.enqueue = RR_enqueue,
	.dequeue = RR_dequeue,
	.pick_next = RR_pick_next,
	.proc_tick = RR_proc_tick,
    .get_load = RR_get_load,
    .get_proc = RR_get_proc,
};
