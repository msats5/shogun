#ifndef _KERNEL_H___
#define _KERNEL_H___

#include "lib/common.h"
#include "features/Features.h"

#include <stdio.h>

class CKernel
{
	public:
		CKernel();
		virtual ~CKernel();

		/** get kernel function for lhs feature vector x 
		  and rhs feature vector y
		  */
		REAL kernel(long x, long y);

		/** initialize your kernel
		 * where l are feature vectors to occur on left hand side
		 * and r the feature vectors to occur on right hand side
		 */
		virtual void init(CFeatures* l, CFeatures* r);

		/// clean up your kernel
		virtual void cleanup()=0;

		/// load and save the kernel
		bool load(FILE* src);
		bool save(FILE* dest);

		// check the feature object
		virtual bool check_features(CFeatures* f)=0 ;

		// return the name of a kernel
		virtual const char* get_name()=0 ;

		void get_kernel_row(long docnum, long *active2dnum, REAL *buffer) ;
		void   cache_kernel_row(long x);
		void   cache_multiple_kernel_rows(long* key, long varnum);
		void   kernel_cache_reset_lru();

		/// set the time used for lru	
		inline void CKernel::set_time(long t)
		{
		    kernel_cache.time=t;
		}

		// Update lru time to avoid removal from cache.
		long CKernel::kernel_cache_touch(long cacheidx)
		{
			if(kernel_cache.index[cacheidx] != -1)
			{
				kernel_cache.lru[kernel_cache.index[cacheidx]]=kernel_cache.time; 
				return(1);
			}
			return(0);
		}

	protected:
		/// compute kernel function for features a and b
		/// idx_{a,b} denote the index of the feature vectors
		/// in the corresponding feature object
		virtual REAL compute(long x, long y)=0;

		/**@ cache kernel evalutations to improve speed
		*/
		//@{
		void   kernel_cache_shrink(long, long, long *);

		/// init kernel cache of size megabytes
		void   kernel_cache_init(long size);
		void   kernel_cache_cleanup();
		long   kernel_cache_malloc();
		void   kernel_cache_free(long cacheidx);
		long   kernel_cache_free_lru();
		REAL *kernel_cache_clean_and_malloc(long);


		/// Is that row cached?
		inline long CKernel::kernel_cache_check(long cacheidx)
		{
			return(kernel_cache.index[cacheidx] != -1);
		}
		//@}

		struct KERNEL_CACHE {
			long   *index;  
			REAL *buffer; 
			long   *invindex;
			long   *active2totdoc;
			long   *totdoc2active;
			long   *lru;
			long   *occu;
			long   elems;
			long   max_elems;
			long   time;
			long   activenum;
			long   buffsize;
//			long   r_offs;
		};
	protected:
		/// feature vectors to occur on left hand side
		CFeatures* lhs;
		/// feature vectors to occur on right hand side
		CFeatures* rhs;

		/// kernel cache
		KERNEL_CACHE kernel_cache;
};
#endif
