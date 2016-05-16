#ifndef __REFCOUNT_H_
#define __REFCOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct isc_refcount_s {
    int refs;
} isc_refcount_t; 

#define isc_refcount_destroy(rp) ((rp)->refs == 0)
#define isc_refcount_current(rp) ((unsigned int)((rp)->refs))

#define isc_refcount_increment0(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int prev;				\
		prev = (rp)->refs + 1;		\
		if (_tmp != NULL)				\
			*_tmp = prev + 1;			\
	} while (0)

#define isc_refcount_increment(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int prev;				\
		prev = (rp)->refs + 1;		\
		if (_tmp != NULL)				\
			*_tmp = prev + 1;			\
	} while (0)

#define isc_refcount_decrement(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int prev;				\
		prev = (rp)->refs  -1;	\
		if (_tmp != NULL)				\
			*_tmp = prev - 1;			\
	} while (0)

void isc_refcount_init(isc_refcount_t *ref, unsigned int n);

#ifdef __cplusplus
}
#endif

#endif

