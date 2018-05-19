#ifndef __KERN_MM_PMM_H__
#define __KERN_MM_PMM_H__

#include <assert.h>
#include <atomic.h>
#include <types.h>
#include <memlayout.h>
#include <mmu.h>
#include <proc.h>
#include <arch.h>
#include <spinlock.h>

#define TEST_PAGE 0X1000

// pmm_manager is a physical memory management class. A special pmm manager -
// XXX_pmm_manager
// only needs to implement the methods in pmm_manager class, then
// XXX_pmm_manager can be used
// by ucore to manage the total physical memory space.
struct pmm_manager {
    const char *name;  // XXX_pmm_manager's name
    void (*init)(
        void);  // initialize internal description&management data structure
                // (free block list, number of free block) of XXX_pmm_manager
    void (*init_memmap)(
        struct Page *base,
        size_t n);  // setup description&management data structcure according to
                    // the initial free physical memory space
    struct Page *(*alloc_pages)(
        size_t n);  // allocate >=n pages, depend on the allocation algorithm
    void (*free_pages)(struct Page *base, size_t n);  // free >=n pages with
                                                      // "base" addr of Page
                                                      // descriptor
                                                      // structures(memlayout.h)
    size_t (*nr_free_pages)(void);  // return the number of free pages
    void (*check)(void);            // check the correctness of XXX_pmm_manager
};
// struct proc_struct;

extern const struct pmm_manager *pmm_manager;
extern spinlock_s pmm_lock;
extern pgd_t *boot_pgdir;
extern uintptr_t boot_cr3;


void check_pgdir(void);
void check_boot_pgdir(void);
void check_alloc_page(void);
void check_boot_pgdir(void);

void pmm_init(void);
void pmm_init_ap(void);
void boot_map_segment(pgd_t *pgdir, uintptr_t la, size_t size,
                             uintptr_t pa, uint32_t perm);

struct Page *alloc_pages(size_t n);
void *boot_alloc_page(void);
void free_pages(struct Page *base, size_t n);
size_t nr_used_pages(void);
size_t nr_free_pages(void);

#define alloc_page() alloc_pages(1)
#define free_page(page) free_pages(page, 1)

pgd_t *get_pgd(pgd_t * pgdir, uintptr_t la, bool create);
pud_t *get_pud(pgd_t * pgdir, uintptr_t la, bool create);
pmd_t *get_pmd(pgd_t * pgdir, uintptr_t la, bool create);
pte_t *get_pte(pgd_t * pgdir, uintptr_t la, bool create);
struct Page *get_page(pgd_t *pgdir, uintptr_t la, pte_t **ptep_store);
void page_remove(pgd_t *pgdir, uintptr_t la);
int page_insert(pgd_t *pgdir, struct Page *page, uintptr_t la, pte_perm_t perm);
struct Page *pgdir_alloc_page(pgd_t *pgdir, uintptr_t la, uint32_t perm);

void load_rsp0(uintptr_t rsp0);
void set_pgdir(struct proc_struct *proc, pgd_t * pgdir);
void load_pgdir(struct proc_struct *proc);
void map_pgdir(pgd_t * pgdir);


void tlb_update(pgd_t * pgdir, uintptr_t la);
void tlb_invalidate(pgd_t * pgdir, uintptr_t la);
void tlb_invalidate_user(void);

struct Page *pgdir_alloc_page(pgd_t * pgdir, uintptr_t la, uint32_t perm);
void unmap_range(pgd_t * pgdir, uintptr_t start, uintptr_t end);
void exit_range(pgd_t * pgdir, uintptr_t start, uintptr_t end);
int copy_range(pgd_t * to, pgd_t * from, uintptr_t start, uintptr_t end,
	       bool share);

void print_pgdir(void);

/* *
 * PADDR - takes a kernel virtual address (an address that points above
 * KERNBASE),
 * where the machine's maximum 256MB of physical memory is mapped and returns
 * the
 * corresponding physical address.  It panics if you pass it a non-kernel
 * virtual address.
 * */
#define PADDR(kva)                                                 \
    ({                                                             \
        uintptr_t __m_kva = (uintptr_t)(kva);                      \
        if (__m_kva < KERNBASE) {                                  \
            panic("PADDR called with invalid kva %08lx", __m_kva); \
        }                                                          \
        __m_kva - va_pa_offset;                                    \
    })

/* *
 * KADDR - takes a physical address and returns the corresponding kernel virtual
 * address. It panics if you pass an invalid physical address.
 * */
#define KADDR(pa)                                                \
    ({                                                           \
        uintptr_t __m_pa = (pa);                                 \
        size_t __m_ppn = PPN(__m_pa);                            \
        if (__m_ppn >= npage) {                                  \
            panic("KADDR called with invalid pa %08lx", __m_pa); \
        }                                                        \
        (void *)(__m_pa + va_pa_offset);                         \
    })

/* Simply translate between VA and PA without checking */
#define VADDR_DIRECT(addr) ((void*)((uintptr_t)(addr) + va_pa_offset))
#define PADDR_DIRECT(addr) ((uintptr_t)(addr) - va_pa_offset)

#define NEXT_PAGE(pg) (pg + 1)

extern struct Page *pages;
extern size_t npage;
extern const size_t nbase;
extern uint64_t va_pa_offset;

static inline ppn_t page2ppn(struct Page *page) { return page - pages + nbase; }

static inline uintptr_t page2pa(struct Page *page) {
    return page2ppn(page) << PGSHIFT;
}

static inline struct Page *pa2page(uintptr_t pa) {
    if (PPN(pa) >= npage) {
        panic("pa2page called with invalid pa");
    }
    return &pages[PPN(pa) - nbase];
}

static inline void *page2kva(struct Page *page) { return KADDR(page2pa(page)); }

static inline struct Page *kva2page(void *kva) { return pa2page(PADDR(kva)); }

static inline struct Page *pte2page(pte_t pte) {
    if (!(pte & PTE_V)) {
        panic("pte2page called with invalid pte");
    }
    return pa2page(PTE_ADDR(pte));
}

static inline struct Page *pde2page(pgd_t pde) {
    return pa2page(PDE_ADDR(pde));
}

static inline struct Page *pmd2page(pmd_t pmd)
{
	return pa2page(PMD_ADDR(pmd));
}

static inline struct Page *pud2page(pud_t pud)
{
	return pa2page(PUD_ADDR(pud));
}

static inline struct Page *pgd2page(pgd_t pgd)
{
	return pa2page(PGD_ADDR(pgd));
}

static inline int page_ref(struct Page *page)
{
    return atomic_read(&(page->ref));
}

static inline void set_page_ref(struct Page *page, int val)
{
	atomic_set(&(page->ref), val);
}

static inline int page_ref_inc(struct Page *page) {
	return atomic_add_return(&(page->ref), 1);
}

static inline int page_ref_dec(struct Page *page) {
	return atomic_sub_return(&(page->ref), 1);
}

static inline pgd_t *init_pgdir_get(void)
{
	return boot_pgdir;
}

static inline void flush_tlb() { asm volatile("sfence.vma"); }

// construct PTE from a page and permission bits
static inline pte_t pte_create(uintptr_t ppn, int type) {
    return (ppn << PTE_PPN_SHIFT) | PTE_V | type;
}

static inline pte_t ptd_create(uintptr_t ppn) { return pte_create(ppn, PTE_V); }

// extern char bootstack[], bootstacktop[];

#endif /* !__KERN_MM_PMM_H__ */

