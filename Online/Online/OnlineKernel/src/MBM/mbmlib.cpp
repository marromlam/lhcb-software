/*-----------------------------------------------------------------------*/
/*
*   OS-9 BUFFER MANAGER
*   TRAP MODULE  (C ROUTINES)
*
* Edition History
*
*  #   Date  Comments   By
* -- -------- ----------------------------------------------- ---
*  0  28/09/88  Initial version				PM
*  1  11/11/88  Released version 1.0				PM
*  2  29/11/88  Multibuffer and spy introduced			PM
*  3  15/12/88  Released version 2.0				PM
*  4  14/03/89  Minor corrections				PM
*  5  24/09/91  Allow waiting space and event at the same time PM
*  6  06/10/92  Add update request for reformatting tasks	PM/BJ
* ---
*  7  ??/12/92  Multi-Buffer Manager calls			BJ
*  8  25/03/93  Basic clean-up					AMi8
*/
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <memory>
#include "bm_struct.h"

#define _mbm_return_err(a)	{ errno = a; return (a); }

inline int _mbm_printf(const char* , ...)  {
//inline int _mbm_printf(const char* fmt, ...)  {
//  va_list args;
//  va_start(args, fmt);
//  return vprintf(fmt, args);
  return 1;
}


template <class T> void print_queue(const char* format,const T* ptr, int type)  {
  switch (type)  {
  case 0:
    printf(format,*ptr,*ptr,add_ptr(ptr,*ptr));
    break;
  case 1:
    printf(format,*ptr,*ptr,add_ptr(ptr,(*ptr-4)));
    break;
  }
}



inline void Check_EVENT(const EVENT* ev) {
  if ( ev->block_id != MBM::BID_EVENT )    {
    lib_rtl_signal_message (0, "bad EVENT pointer");
  }
}
inline void Check_USER(const USER* us) {
  if ( us->block_id != MBM::BID_USER )   {
    lib_rtl_signal_message(0, "bad USER pointer");
  }
}


typedef qentry_t* PQ_t;
class Lock  {
  BMDESCRIPT *m_bm;
  int m_status;
public:
  Lock(BMDESCRIPT* bm) : m_bm(bm) {
    m_status = _mbm_lock_tables(m_bm);
    if ( !(m_status&1) ) {
      errno = m_status;
      ::printf("LOCK: System Lock error on BM tables\n");
    }
    if ( m_bm->_control()->spare1 != 0 )  {
      //::printf("LOCK: Lock error on BM tables\n");
    }
    //m_bm->_control()->spare1 = 1;
  }
  virtual ~Lock() {
    if ( m_bm->_control()->spare1 != 1 )  {
      //::printf("UNLOCK: Lock error on BM tables\n");
    }
    //m_bm->_control()->spare1 = 0;
    //  _mbm_flush_sections(m_bm);
    m_status = _mbm_unlock_tables(m_bm);
    if ( !m_status )  {
      // throw exception ?
    }
  }
  operator int ()   {
    return lib_rtl_is_success(m_status);
  }
  int status()      const   {
    return m_status;
  }
};

class UserLock  {
  Lock  m_lock;
  USER* m_user;
  int m_status;
public:
  UserLock(BMDESCRIPT* bm, int def=MBM_ILL_CONS) : m_lock(bm), m_user(0) {
    if ( m_lock ) {
      m_user = bm->_user();
      m_status = m_user ? MBM_NORMAL : (errno = def);
    }
    m_status = m_lock.status();
  }
  virtual ~UserLock() {
  }
  USER* user()      const   {
    return m_user;
  }
  int status()  const  {
    return m_status;
  }
};

static qentry_t *desc_head;
static int reference_count = 0;

static int USER_next_off;
static int USER_ws_off;
static int USER_we_off;
static int USER_wes_off;
static int EVENT_next_off;
static int CONTROL_cons_off;
static int CONTROL_cwe_off;
static int CONTROL_pws_off;
static int CONTROL_wes_off;
static int CONTROL_ev_off;
static MBM_ast_t _mbm_wes_ast_add = 0;
static int disable_rundown=0;

#define QR_success(a) ((a) > 0)

int mbm_error()  {
  ::printf("Bad\n");
  return MBM_ERROR;
}
#undef MBM_ERROR
#define  MBM_ERROR mbm_error();

static EXHDEF exh_block;
static int exit_status;

using namespace MBM;

BMDESCRIPT *mbm_include (const char* bm_name, const char* name, int partid) {
  char text[32]="";
  int status;

  _mbm_fill_offsets();
  _mbm_wes_ast_add  = _mbm_wes_ast;
  if ( reference_count == 0 )  {
    desc_head	= new qentry_t;
    memset(desc_head,0,sizeof(qentry_t));
  }
  std::auto_ptr<BMDESCRIPT> bm(new BMDESCRIPT);
  memset(bm.get(),0,sizeof(BMDESCRIPT));

  strcpy(bm->bm_name,bm_name);
  sprintf(text, "bm_ctrl_%s",   bm_name);
  status  = _mbm_map_section(text, sizeof(CONTROL), bm->ctrl_add);
  if (!lib_rtl_is_success(status))    {
    ::printf("failure to map control section for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->ctrl = (CONTROL*)bm->ctrl_add[0];
  sprintf(text, "bm_event_%s",  bm_name);
  status  = _mbm_map_section(text, bm->ctrl->p_emax*sizeof(EVENT), bm->event_add);
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->ctrl_add);
    ::printf("failure to map event section for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->event = (EVENT*)bm->event_add[0];
  sprintf(text, "bm_user_%s",   bm_name);
  status  = _mbm_map_section(text, bm->ctrl->p_umax*sizeof(USER), bm->user_add);
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    ::printf("failure to map user section for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->user = (USER*)bm->user_add[0];
  sprintf(text, "bm_bitmap_%s", bm_name);
  status  = _mbm_map_section(text, bm->ctrl->bm_size, bm->bitm_add);
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    ::printf("failure to map bit-map section for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->bitmap = (char*)bm->bitm_add[0];
  sprintf(text, "bm_buffer_%s", bm_name);
  status  = _mbm_map_section(text, bm->ctrl->buff_size, bm->buff_add);
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->bitm_add);
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    ::printf("failure to map buffer section for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->buffer_add = (char*)bm->buff_add[0];

  status = _mbm_create_lock(bm.get());
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->buff_add);
    _mbm_unmap_section(bm->bitm_add);
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    ::printf("failure to create lock for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  status = _mbm_lock_tables(bm.get());
  if (!lib_rtl_is_success(status))  {
    _mbm_unmap_section(bm->buff_add);
    _mbm_unmap_section(bm->bitm_add);
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    _mbm_delete_lock(bm.get());
    ::printf("failure to lock tables for %s. Status = %d\n",bm_name,status);
    return (BMDESCRIPT*)-1;
  }
  bm->bitmap_size = bm->ctrl->bm_size;
  bm->buffer_size = bm->ctrl->buff_size;

  int owner = _mbm_ualloc (bm.get());	/* find free slot */
  if (owner == -1)  {
    _mbm_unlock_tables(bm.get());
    errno = MBM_NO_FREE_US;
    return (BMDESCRIPT*)-1;
  }
  bm->owner = owner;

  USER* us = bm->user + bm->owner;
  us->c_state = S_active;
  us->p_state = S_active;
  us->partid = partid;
  ::strncpy (us->name, name, NAME_LENGTH);
  us->pid = lib_rtl_pid ();
  us->space_add     = 0;
  us->space_size    = 0;
  us->ev_produced   = 0;
  us->ev_actual     = 0;
  us->ev_seen       = 0;
  us->held_eid      = -1;
  us->n_req         = 0;
  us->get_ev_calls  = 0;
  us->get_wakeups   = 0;	
  us->get_asts_run  = 0;	

  bm->ctrl->i_users++;
  reference_count++;

  // Activate this user
  sprintf(us->wes_flag, "bm_%s_WES_%d", bm_name, lib_rtl_pid());
  lib_rtl_create_named_event(us->wes_flag, &bm->WES_event_flag);
  sprintf(us->wev_flag, "bm_%s_WEV_%d", bm_name, lib_rtl_pid());
  lib_rtl_create_named_event(us->wev_flag, &bm->WEV_event_flag);
  sprintf(us->wsp_flag, "bm_%s_WSP_%d", bm_name, lib_rtl_pid());
  lib_rtl_create_named_event(us->wsp_flag, &bm->WSP_event_flag);
  sprintf(us->wspa_flag, "bm_%s_WSPA_%d", bm_name, lib_rtl_pid());
  lib_rtl_create_named_event(us->wspa_flag, &bm->WSPA_event_flag);
  sprintf(us->weva_flag, "bm_%s_WEVA_%d", bm_name, lib_rtl_pid());
  lib_rtl_create_named_event(us->weva_flag, &bm->WEVA_event_flag);

  insqhi (bm.get(), desc_head);
  if (exh_block.exit_param == 0)  {
    exh_block.exit_handler  = _mbm_exit;
    exh_block.exit_param    = (void*)0;
    exh_block.exit_status   = &exit_status;
    lib_rtl_declare_exit (&exh_block);
  }
  lib_rtl_declare_rundown(_mbm_shutdown,0);
  errno = 0;
  _mbm_unlock_tables(bm.get());
  return bm.release();
}

/// Exclude from buffer manager
int mbm_exclude (BMDESCRIPT *bm)  {
  Lock lock(bm);
  if ( lock )  {
    int owner = bm->owner;
    if (owner == -1)    {
      _mbm_return_err (MBM_ILL_CONS);
    }
    USER* us = bm->user + bm->owner;
    lib_rtl_delete_named_event(us->wes_flag,  &bm->WES_event_flag);
    lib_rtl_delete_named_event(us->wev_flag,  &bm->WEV_event_flag);
    lib_rtl_delete_named_event(us->wsp_flag,  &bm->WSP_event_flag);
    lib_rtl_delete_named_event(us->wspa_flag, &bm->WSPA_event_flag);
    lib_rtl_delete_named_event(us->weva_flag, &bm->WEVA_event_flag);
    lib_rtl_remove_rundown(_mbm_shutdown,0);
    _mbm_uclean(bm);
    _mbm_unmap_section(bm->buff_add);
    _mbm_unmap_section(bm->bitm_add);
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
  }
  else {
    return lock.status();
  }
  _mbm_delete_lock(bm);
  qentry_t *dummy, *hd	= add_ptr(bm->next,&bm->prev);
  remqhi (hd , &dummy);

  free(dummy);
  reference_count--;
  if (reference_count == 0)  {
    free(desc_head);
    desc_head = 0;
  }
  return MBM_NORMAL;
}

/// Consumer routines
int mbm_add_req (BMDESCRIPT *bm, int evtype, int trg_mask[4], int veto_mask[4], int masktype, 
                 int usertype, int freqmode, float freq)
{
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    TriggerMask* trmask = (TriggerMask*)trg_mask;
    VetoMask*    veto   = (VetoMask*)veto_mask;
    if (us->n_req == 8)  {
      _mbm_return_err (MBM_TOO_MANY);
    }
    REQ* rq = us->req + us->n_req;
    ::memset(rq,0,sizeof(REQ));
    rq->ev_type   = evtype;
    rq->tr_mask   = *trmask;
    if (evtype > TOPTYP && int(trmask->word(0)) <= evtype )        {
      rq->tr_mask.setWord(0, evtype);
    }
    rq->vt_mask   = *veto;
    rq->masktype  = masktype;
    rq->freqmode  = freqmode;
    rq->user_type = usertype;
    rq->freq	    = freq;
    us->n_req    += 1;
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_del_req (BMDESCRIPT *bm, int evtype, int trmask[4], int veto[4], int masktype, int usertype)  {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    REQ *rq, *rqn;
    int i, j;
    for (i = 0, rq = us->req; i < us->n_req; i++, rq++)  {
      if ( evtype != rq->ev_type || rq->tr_mask != trmask || rq->vt_mask != veto )
        continue;
      if ( masktype != rq->masktype || usertype != rq->user_type )
        continue;
      for (j = i + 1, rqn = rq + 1; j < us->n_req; j++, rq++, rqn++)      {
        ::memcpy(rq,rqn,sizeof(REQ));
      }
      us->n_req -= 1;
      return MBM_NORMAL;
    }
  }
  return user.status();
}

int mbm_get_event_a (BMDESCRIPT *bm, int** ptr, int* size, int* evtype, int trmask[4], 
		     int part_id, MBM_ast_t astadd, void* astpar) {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    TriggerMask* mask = (TriggerMask*)trmask;
    if (us->c_state == S_wevent)    {
      ::printf("Too many calls to mbm_get_event_a\n");
      return MBM_NORMAL;
    }
    if (us->held_eid != -1)    {
      _mbm_rel_event(bm, bm->owner);
    }
    if (us->c_state == S_pause)    {
      us->c_state = S_active;
    }
    us->get_ev_calls++;
    int status = _mbm_get_ev (bm, us);
    if (status == MBM_NORMAL)    {
      us->c_partid      = part_id;
      us->we_ptr_add    = ptr;
      us->we_size_add   = size;
      us->we_evtype_add = evtype;
      us->we_trmask_add = mask;
      us->c_state       = S_wevent_ast_queued;
      us->c_astadd      = astadd;
      us->c_astpar      = astpar;
      us->reason        = BM_K_INT_EVENT;
      us->get_wakeups++;
      return MBM_NORMAL;
    }
    /// add wait event queue
    _mbm_add_wev (bm, us, ptr, size, evtype, mask, part_id, astadd, astpar);
    return MBM_NO_EVENT;
  }
  return user.status();
}

int mbm_get_event(BMDESCRIPT *bm, int** ptr, int* size, int* evtype, int trmask[4], int part_id) {
  int sc = mbm_get_event_a(bm, ptr,size,evtype,trmask,part_id,mbm_get_event_ast,bm);
  if ( sc == MBM_NORMAL || sc == MBM_NO_EVENT ) {
    return mbm_wait_event(bm);
  }
  return sc;
}

int mbm_free_event (BMDESCRIPT *bm) {
  UserLock user(bm, MBM_INTERNAL);
  USER* us = user.user();
  if ( us )  {
    if (us->held_eid == -1)    {
      _mbm_return_err (MBM_NO_EVENT);
    }
    _mbm_rel_event (bm, bm->owner);	/* release event held by him  */
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_pause (BMDESCRIPT *bm)  {
  qentry_t park(0,0);
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    CONTROL* ctrl = bm->_control();
    if ( ctrl )  {
      EVENT *ev1;
      if (us->held_eid != -1)  {
        _mbm_rel_event (bm, bm->owner);
      }
      us->c_state = S_pause;
      for(int sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1);
          QR_success(sc);
          sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1) )
      {
        insqhi (ev1, &park);
        EVENT* ev = add_ptr(ev1,-EVENT_next_off);
        Check_EVENT(ev);
        if (ev->umask0.bit_test(us->uid) || ev->umask1.bit_test(us->uid) )    {
          ev->umask0.bit_clear(us->uid);
          ev->umask1.bit_clear(us->uid);
          if ( !ev->umask0.mask_or_ro(ev->umask1) )	  {  /* no more consumers */
            /* de-allocate event space */
            _mbm_sfree (bm, ev->ev_add, ev->ev_size);
            _mbm_efree (bm, ev->eid);	/* de-allocate event slot */
          }
        }
      }
      return _mbm_restore_park_queue(&park,&ctrl->e_head);
    }
  }
  return user.status();
}

/*
Producer Routines
*/
int mbm_get_space_a (BMDESCRIPT *bm, int size, int** ptr, MBM_ast_t astadd, void* astpar)  {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    CONTROL* ctrl = bm->_control();
    if (size <= 0 || size > ctrl->buff_size)    {
      _mbm_return_err (MBM_ILL_LEN);
    }
    if (us->p_state == S_wspace)    {
      _mbm_del_wsp (bm, us);
    }
    if (us->space_size)    {
      _mbm_printf("mbm_get_space_a> Free event space:%08X \n",us->space_add);
      _mbm_sfree (bm, us->space_add, us->space_size);
      us->space_add = 0;
      us->space_size = 0;
    }
    int status = _mbm_get_sp (bm, us, size, ptr);
    if (status == MBM_NO_ROOM)  {
      if (_mbm_check_freqmode (bm) == 0)  {
        status = _mbm_get_sp (bm, us, size, ptr);
      }
      if (status == MBM_NO_ROOM)  {
        _mbm_add_wsp (bm, us, size, ptr, astadd, astpar);
      }
      else  {
        us->reason      = BM_K_INT_SPACE;
        us->p_state	    = S_wspace_ast_queued;
        us->ws_size     = size;
        us->ws_ptr_add  = ptr;
        us->p_astadd    = astadd;
        us->p_astpar    = astpar;
        lib_rtl_set_event (bm->WSP_event_flag);
      }
    }
    else    {
      us->reason      = BM_K_INT_SPACE;
      us->ws_size     = size;
      us->ws_ptr_add  = ptr;
      us->p_state     = S_wspace_ast_queued;
      us->p_astadd    = astadd;
      us->p_astpar    = astpar;
      lib_rtl_set_event (bm->WSP_event_flag);
    }
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_declare_event (BMDESCRIPT *bm, int len, int evtype, int* trmask, const char* dest,
                       void** free_add, int* free_size, int part_id)
{
  Lock lock(bm);
  if ( lock )  {
    return _mbm_declare_event (bm, len, evtype, *(TriggerMask*)trmask, dest, free_add, free_size, part_id);
  }
  return lock.status();
}

int mbm_declare_event_and_send (BMDESCRIPT *bm, int len, int evtype, int* trmask,
                                const char* dest, void** free_add, int* free_size, int part_id)
{
  Lock lock(bm);
  if ( lock )  {
    int sc = _mbm_declare_event (bm, len, evtype, *(TriggerMask*)trmask, dest, free_add, free_size, part_id);
    if (lib_rtl_is_success(sc))    {
      sc = _mbm_send_space(bm);
      if (lib_rtl_is_success(sc))      {
        *free_add = 0;
        *free_size	= 0;
      }
    }
    return sc;
  }
  return lock.status();
}

int mbm_free_space (BMDESCRIPT *bm) {
  UserLock user(bm, MBM_ILL_CONS);
  USER* us = user.user();
  if ( us )  {
    if (us->space_size)    {
      _mbm_sfree (bm, us->space_add, us->space_size);
      us->space_add = 0;
      us->space_size = 0;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_NO_EVENT);
  }
  return user.status();
}

int mbm_send_space (BMDESCRIPT* bm) {
  Lock lock(bm);
  if ( lock )  {
    return _mbm_send_space(bm);
  }
  return lock.status();
}

#if 0
int mbm_send_event (BMDESCRIPT *bm, int* array, int len, int evtype, int* trmask, const char* dest, int partid)  {
  int *buff, dummy;
  int status = mbm_get_space (bm, len, &buff);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  for (int *bfs = array, *bfd = buff, i = (len + 3) >> 2; i > 0; bfs++, bfd++, i--)  {
    *bfd = *bfs;
  }
  status = mbm_declare_event (bm, len, evtype, trmask, dest, &dummy, &dummy, partid);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  status = mbm_send_space (bm);
  if (status != MBM_NORMAL)  {
    return (status);
  }
  return MBM_NORMAL;
}
#endif

int mbm_cancel_request (BMDESCRIPT *bm)   {
  UserLock user(bm, MBM_ILL_CONS);
  USER* us = user.user();
  if ( us )  {
    if (us->c_state == S_wevent)    {
      _mbm_del_wev (bm, us);
    }
    if (us->p_state == S_wspace)    {
      _mbm_del_wsp (bm, us);
    }
    if (us->p_state == S_weslot)    {
      _mbm_del_wes (bm, us);
    }
    return MBM_NORMAL;
  }
  return user.status();
}

/// Utility routines

/// clear events with freqmode = notall
int _mbm_check_freqmode (BMDESCRIPT *bm)  {
  EVENT *ev, *ev1;
  qentry_t park(0,0);
  CONTROL *ctrl = bm->ctrl;

  int ret = -1;
  for(int sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1);
      QR_success(sc);
      sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1) )
  {
    insqhi (ev1, &park);
    ev  = add_ptr(ev1,-EVENT_next_off);
    Check_EVENT(ev);
    if (!ev->umask0.mask_summ() && ev->umask1.mask_summ() && !ev->held_mask.mask_summ() )    {
      _mbm_sfree (bm, ev->ev_add, ev->ev_size);      /* de-allocate event space */
      _mbm_efree (bm, ev->eid);                      /* de-allocate event slot  */
      ret = 0;
    }
  }
  _mbm_restore_park_queue(&park,&ctrl->e_head);
  return (ret);
}

/// try to get space ...
int _mbm_get_sp (BMDESCRIPT *bmid, USER* us, int size, int** ptr)  {
  int bit, nbit = (size + Bytes_p_Bit) >> Shift_p_Bit;	/* round size to block */
  CONTROL *ctrl = bmid->ctrl;
  char *bitmap = bmid->bitmap;
  ctrl->last_alloc = 0;
  int status = BF_alloc(bitmap+ctrl->last_alloc,ctrl->bm_size-(ctrl->last_alloc<<3),nbit,&bit);
  if (!lib_rtl_is_success(status))  {
    ctrl->last_alloc = 0;
    status = BF_alloc(bitmap,ctrl->bm_size,nbit,&bit);
  }
  if (lib_rtl_is_success(status))  {
    bit += ctrl->last_alloc<<3 ;
    ctrl->last_alloc  = (bit+nbit)>>3;
    ctrl->last_bit = bit;
    ctrl->i_space -= nbit;
    us->ws_ptr = (bit << Shift_p_Bit);
    *ptr = (int*)(bmid->buffer_add + (bit << Shift_p_Bit));
    us->space_add = (bit<<Shift_p_Bit);	/* keep space inf... */
    us->space_size = nbit << Shift_p_Bit;
    _mbm_printf("Got space: %08X %d Bytes\n",us->ws_ptr, size);
    return MBM_NORMAL;
  }
  return MBM_NO_ROOM;
}

/* try to get event ... */  
int _mbm_get_ev(BMDESCRIPT *bm, USER* us)  {
  EVENT *ev1;
  qentry_t park(0,0);
  CONTROL *ctrl = bm->ctrl;
  for(int sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1);
    QR_success(sc);
    sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1)    )
  {
    insqhi (ev1, &park);
    EVENT* e  = add_ptr(ev1,-EVENT_next_off);
    Check_EVENT(e);
    if ( (e->busy !=2) && (e->busy !=0) )  {
      if ( e->umask0.bit_test(us->uid) || e->umask1.bit_test(us->uid) )  {
        us->we_ptr    = e->ev_add;
        us->we_size   = e->ev_size;
        us->we_evtype = e->ev_type;
        us->we_trmask = e->tr_mask;
        us->held_eid  = e->eid;
        e->held_mask.bit_set(us->uid);
        us->ev_seen++;
        ctrl->tot_seen++;
        return _mbm_restore_park_queue(&park,&ctrl->e_head);
      }
    }
  }
  _mbm_restore_park_queue(&park,&ctrl->e_head);
  return MBM_NO_EVENT;
}

/// add user in wait_event queue
int _mbm_add_wev(BMDESCRIPT *bm, USER *us, int** ptr, int* size, int* evtype, TriggerMask* trmask, 
		 int part_id, MBM_ast_t astadd, void* astpar)  {
  static int calls = 0;
  CONTROL *ctrl     = bm->ctrl;
  us->c_state       = S_wevent;
  us->c_partid      = part_id;
  us->we_ptr_add    = ptr;
  us->we_size_add   = size;
  us->we_evtype_add = evtype;
  us->we_trmask_add = trmask;
  us->c_astadd      = astadd;
  us->c_astpar      = astpar;
  us->held_eid      = -1;
  _mbm_printf("WEV ADD> %d State:%d\n",calls++, us->c_state);
  insqti(&us->wenext, (qentry_t *)&ctrl->wev_head.next);
  return MBM_NORMAL;
}

/// del user from the wait_event queue
int _mbm_del_wev (BMDESCRIPT* /* bm */, USER* us) {
  static int calls = 0;
  if ( us->c_state != S_wevent )  {
    ::printf("INCONSISTENCY: Delete user from WEV queue without state S_wevent");
  }
  _mbm_printf("WEV DEL> %d\n",calls++);
  us->c_state = S_wevent_ast_queued;
  qentry_t *dummy, *hd	= add_ptr(&us->wenext,us->wenext.prev);
  remqhi (hd, &dummy);
  return MBM_NORMAL;
}

/// check wait event queue
int _mbm_check_wev (BMDESCRIPT *bm, EVENT* ev)  {
  USER *us1;
  CONTROL *ctrl = bm->ctrl;
  qentry_t park(0,0);
  for (int sc=remqhi(&ctrl->wev_head,(qentry_t**)&us1); QR_success(sc);
           sc = remqhi(&ctrl->wev_head,(qentry_t**)&us1) )
  {
    insqhi (us1, &park);
    USER* us  = add_ptr(us1,-USER_we_off);
    Check_USER(us);
    ev->held_mask.bit_set(us->uid);
    if (ev->umask0.bit_test(us->uid) || ev->umask1.bit_test(us->uid))    {
      us->we_ptr    = ev->ev_add;
      us->we_size   = ev->ev_size;
      us->we_evtype = ev->ev_type;
      us->we_trmask = ev->tr_mask;
      us->held_eid  = ev->eid;
      ev->held_mask.bit_set(us->uid);
      us->ev_seen++;
      ctrl->tot_seen++;
      _mbm_del_wev (bm, us);
      us->get_wakeups++;
      _mbm_printf("EVENT: id=%d  %d %08X -> %s\n",ev->eid, ev->count, ev->ev_add, us->name);
      _mbm_wake_process(BM_K_INT_EVENT, us);
    }
  }
  _mbm_restore_park_queue(&park,&ctrl->wev_head);
  return MBM_NORMAL;
}

/// add user in the wait_space queue
int _mbm_add_wsp (BMDESCRIPT *bm, USER* us, int size, int** ptr, MBM_ast_t astadd, void* astpar) {
  CONTROL *ctrl   = bm->ctrl;
  us->p_state     = S_wspace;
  us->p_astadd    = astadd;
  us->p_astpar    = astpar;
  us->ws_size     = size;
  us->ws_ptr_add  = ptr;
  insqti (&us->wsnext, &ctrl->wsp_head);
  return MBM_NORMAL;
}

/// del user from the wait_space queue
int _mbm_del_wsp (BMDESCRIPT* /* bm */, USER* us) {
  if (us->p_state != S_wspace)  {
    _mbm_printf("INCONSISTENCY: Delete user from WSP queue without state S_wspace");
  }
  qentry_t *dummy, *hd = add_ptr(&us->wsnext,us->wsnext.prev);
  remqhi (hd , &dummy);
  us->p_state = S_wspace_ast_queued;
  return MBM_NORMAL;
}

/// check wait space queue
int _mbm_check_wsp (BMDESCRIPT *bmid, int bit, int nbit)  {
  USER *us1;
  qentry_t park(0,0);
  CONTROL *ctrl = bmid->ctrl;
  char *bitmap = bmid->bitmap;
  int size = nbit << Shift_p_Bit;
  for(int sc = remqhi(&ctrl->wsp_head,(qentry_t**)&us1); QR_success(sc);
    sc = remqhi(&ctrl->wsp_head,(qentry_t**)&us1) )
  {
    insqhi(us1, &park);
    USER* us  = add_ptr(us1,-USER_ws_off);
    Check_USER(us);
    _mbm_printf("WSP: User %s\n",us->name);
    if ( us->p_state == S_wspace && us->ws_size <= size)      {
      int ubit = (us->ws_size + Bytes_p_Bit) >> Shift_p_Bit;
      ctrl->last_alloc = 0;
      int status = BF_alloc(bitmap+ctrl->last_alloc,ctrl->bm_size-(ctrl->last_alloc<<3),ubit,&bit);
      if (!lib_rtl_is_success(status))      {
        ctrl->last_alloc = 0;
        status = BF_alloc(bitmap,(ctrl->bm_size),ubit,&bit);
      }
      if (lib_rtl_is_success(status))       {
        bit += ctrl->last_alloc<<3 ;
        ctrl->last_alloc  = (bit+ubit)>>3;
        ctrl->last_bit    = bit;
        ctrl->i_space    -= ubit;
        us->ws_ptr        = (bit << Shift_p_Bit);
        us->space_add     = us->ws_ptr;
        us->space_size    = ubit << Shift_p_Bit;
        _mbm_del_wsp (bmid, us);
        _mbm_wake_process(BM_K_INT_SPACE, us);
        break;
      }
    }
  }
  return _mbm_restore_park_queue(&park,&ctrl->wsp_head);
}

/// find matching req
int _mbm_match_req (BMDESCRIPT *bm, int partid, int evtype, TriggerMask& trmask, 
                    UserMask& mask0, UserMask& mask1)  
{
  USER *us1;
  UserMask dummy;
  qentry_t park(0,0);
  CONTROL *ctrl = bm->ctrl;
  for(int sc = remqhi(&ctrl->u_head,(qentry_t**)&us1);
    QR_success(sc);
    sc = remqhi(&ctrl->u_head,(qentry_t**)&us1)
    )
  {
    int i;
    REQ *rq;
    insqhi(us1, &park);
    USER* us  = add_ptr(us1,-USER_next_off);
    Check_USER(us);

    if (((us->partid != 0) && (partid != 0) && (us->partid != partid)) || (us->c_state == S_pause))  {
      continue;
    }
    for (i = 0, rq = us->req; i < us->n_req; i++, rq++)  {
      if (evtype <= TOPTYP)  {
        if (evtype != rq->ev_type)
          continue;
        if (rq->masktype == BM_MASK_ALL)  {
          if ( trmask != rq->tr_mask )
            continue;
        }
        else if ( !dummy.mask_and(trmask, rq->tr_mask) )
          continue;
        else if ( dummy.mask_and(trmask, rq->vt_mask) )  
          continue;
      }
      else if (rq->ev_type <=TOPTYP)
        continue;
      else if ( evtype < rq->ev_type || evtype > (int)rq->tr_mask.word(0) )
        continue;
      else if ( (float(::rand())/float(RAND_MAX)*100.0) > rq->freq)
        continue;

      (rq->user_type == BM_REQ_VIP) ? mask0.bit_set(us->uid) : mask1.bit_set(us->uid);
      us->ev_actual++;
      ctrl->tot_actual++;
      break;
    }
  }
  return _mbm_restore_park_queue(&park,&ctrl->u_head);
}

/// check existance of name
int _mbm_findnam (BMDESCRIPT *bm, const char* name) {
  USER *us1;
  qentry_t park(0,0);
  CONTROL *ctrl = bm->ctrl;
  for(int sc = remqhi(&ctrl->u_head,(qentry_t**)&us1); QR_success(sc); )  {
    insqhi(us1, &park);
    USER* us  = add_ptr(us1,-USER_next_off);
    Check_USER(us);
    if (strncmp (us->name, name, NAME_LENGTH) == 0)  {
      _mbm_restore_park_queue(&park,&ctrl->u_head);
      return us->uid;
    }
    sc = remqhi(&ctrl->u_head,(qentry_t**)&us1);
  }
  _mbm_restore_park_queue(&park,&ctrl->u_head);
  return (-1);
}

/// alloc user slot
int _mbm_ualloc (BMDESCRIPT *bm)  {
  USER *us = bm->user;
  int i = 0;
  for (CONTROL *ctrl = bm->ctrl; i < ctrl->p_umax; i++, us++)  {
    if (us->busy == 0)    {
      us->busy = 1;
      us->uid = i;
      insqti(us, &ctrl->u_head);
      return (i);
    }
  }
  return (-1);
}

/// alloc event slot
int _mbm_ealloc (BMDESCRIPT *bm)  {
  int i = 0;
  static int cnt = 0;
  EVENT *ev = bm->event;
  for (CONTROL *ctrl = bm->ctrl; i < ctrl->p_emax; i++, ev++)  {
    Check_EVENT(ev);
    if (ev->busy == 0)    {
      int c = ev->count;
      ev->busy = 2;
      ev->eid  = i;
      ev->count = cnt++;
      ctrl->i_events++;
      insqti(ev, &ctrl->e_head);
      _mbm_printf("Got EVENT Slot: %d %d [%d]\n",ev->eid,ev->count,c);
      return i;
    }
  }
  return (-1);
}

/// free user slot
int _mbm_ufree (BMDESCRIPT *bm, int i)  {
  USER* us = bm->user + i;
  if (us->busy != 1 || us->uid != i)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  us->busy = 0;
  us->uid  = 0;
  qentry_t *dummy, *hd	= add_ptr(us->prev,&us->next);
  remqhi (hd, &dummy);
  return MBM_NORMAL;
}

/// free event slot
int _mbm_efree (BMDESCRIPT *bm, int i)  {
  if (i == -1)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  EVENT* ev = bm->event + i;
  if (ev->busy == 0 || ev->eid != i)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  ev->busy = 0;
  _mbm_printf("Free slot: %d %d\n",ev->eid, ev->count);
  qentry_t* dummy;
  qentry_t *hd = add_ptr(ev->prev,&ev->next);
  remqhi(hd, &dummy);
  bm->ctrl->i_events--;
  _mbm_check_wes (bm);
  return MBM_NORMAL;
}

/// release event held by this user
int _mbm_rel_event (BMDESCRIPT *bm, int uid)  {
  USER  *us = bm->user + uid;
  EVENT *ev = bm->event + us->held_eid;
  us->held_eid = -1;
  ev->umask0.bit_clear(uid);
  ev->umask1.bit_clear(uid);
  ev->held_mask.bit_clear(uid);
  if ( !ev->umask0.mask_or_ro(ev->umask1) )	{  // no more consumers
    _mbm_sfree (bm, ev->ev_add, ev->ev_size);  // de-allocate event space
    _mbm_efree (bm, ev->eid);                  // de-allocate event slot
  }
  return MBM_NORMAL;
}

/// clean-up this user
int _mbm_uclean (BMDESCRIPT *bm)  {
  EVENT *ev1;
  qentry_t park(0,0);
  int uid = bm->owner;
  CONTROL* ctrl = bm->ctrl;
  USER*    us   = bm->user + uid;

  if (us->c_state == S_wevent)  {
    _mbm_del_wev (bm, us);
  }
  if (us->p_state == S_wspace)  {
    _mbm_del_wsp (bm, us);
  }
  if (us->p_state == S_weslot)  {
    _mbm_del_wes (bm, us);
  }

  if (us->space_size)	 {	/* free the held space */
    _mbm_sfree (bm, us->space_add, us->space_size);
    us->space_add = 0;
    us->space_size = 0;
  }
  if (us->held_eid != -1)	  { /* free the held event */
    _mbm_rel_event (bm, uid);	/* release event */
  }
  for(int sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1); QR_success(sc);
    sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1)
    )
  {
    insqhi (ev1, &park);
    EVENT* ev  = add_ptr(ev1,-EVENT_next_off);
    Check_EVENT(ev);
    ev->umask0.bit_clear(uid);
    ev->umask1.bit_clear(uid);
    ev->held_mask.bit_clear(uid);
    if ( !ev->umask0.mask_or_ro(ev->umask1) || ((ev->busy == 2) && ev->held_mask.mask_summ()))  {
      _mbm_sfree (bm, ev->ev_add, ev->ev_size);
      _mbm_efree (bm, ev->eid);
    }
  }
  _mbm_restore_park_queue(&park,&ctrl->e_head);
  _mbm_ufree (bm, uid);	/* de-allocate user slot */
  ctrl->i_users--;
  return MBM_NORMAL;
}

/// deallocate space
int _mbm_sfree (BMDESCRIPT *bm, int add, int size)  {
  CONTROL *ctrl = bm->ctrl;
  int bit  =  add >> Shift_p_Bit;
  int nbit = (size + Bytes_p_Bit) >> Shift_p_Bit;
  BF_free(bm->bitmap,bit,nbit);
  ctrl->last_alloc = 0;
  ctrl->i_space += nbit;
  _mbm_printf("Free space  add=%08X  size=%d\n",add,size);
  if ( ctrl->wsp_head.next )  {
    int s, l;
    BF_count(bm->bitmap, ctrl->bm_size, &s, &l);    // find largest block 
    _mbm_check_wsp (bm, s, l);                      // check the space wait queue 
  }
  return MBM_NORMAL;
}

int mbm_grant_update (BMDESCRIPT *bm)   {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    if (us->held_eid == -1)  {
      _mbm_return_err (MBM_NO_EVENT);
    }
    return _mbm_req_upd (bm, us);	/* convert event to space */
  }
  return user.status();
}

int _mbm_req_upd (BMDESCRIPT *bm, USER *us)   {
  EVENT* ev = bm->event + us->held_eid;
  int held_eid = us->held_eid;
  int add = (int)ev->ev_add ;
  int siz = ev->ev_size;
  us->held_eid = -1;
  ev->held_mask.bit_clear(us->uid);
  ev->umask0.bit_clear(us->uid);
  ev->umask1.bit_clear(us->uid);
  if ( !ev->umask0.mask_or_ro(ev->umask1) )  {
    //  no more consumers - convert event into space de-allocate event slot
    _mbm_efree (bm, ev->eid);
    us->space_add = add;
    us->space_size = siz;
    return 0;
  }
  // Still other consumers for this event. Restore old situation and return error
  ev->held_mask.bit_set(us->uid);
  ev->umask0.bit_set(us->uid);
  ev->umask1.bit_set(us->uid);
  us->held_eid = held_eid;
  _mbm_return_err (MBM_ILL_REQ);
}

/// clean-up this user in all buffers
int _mbm_exit(void* )    {
  _mbm_shutdown(0);
  return MBM_NORMAL;
}

int _mbm_shutdown (void* /* param */) {
  qentry_t *q, *bmq = desc_head;
  if (bmq == 0)  {
    return MBM_NORMAL;
  }
  for(int sc=remqhi(bmq,&q); QR_success(sc); sc=remqhi(bmq,&q))  {
    BMDESCRIPT *bm = (BMDESCRIPT *)q;
    USER       *us = bm->user + bm->owner;
    _mbm_cancel_lock(bm);
    _mbm_delete_lock(bm);
    if (disable_rundown == 1)    {
      continue;
    }
    //_mbm_create_lock(bm);
    _mbm_lock_tables(bm);
    lib_rtl_delete_named_event(us->wes_flag,  &bm->WES_event_flag);
    lib_rtl_delete_named_event(us->wev_flag,  &bm->WEV_event_flag);
    lib_rtl_delete_named_event(us->wsp_flag,  &bm->WSP_event_flag);
    lib_rtl_delete_named_event(us->wspa_flag, &bm->WSPA_event_flag);
    lib_rtl_delete_named_event(us->weva_flag, &bm->WEVA_event_flag);
    _mbm_uclean (bm);
    _mbm_unmap_section(bm->buff_add);
    _mbm_unmap_section(bm->bitm_add);
    _mbm_unmap_section(bm->user_add);
    _mbm_unmap_section(bm->event_add);
    _mbm_unmap_section(bm->ctrl_add);
    _mbm_unlock_tables(bm);
    //_mbm_delete_lock(bm);
  }
  /*  bm_exh_unlink (); */
  return MBM_NORMAL;
}

int _mbm_find_buffer (const char* bm_name)  {
  qentry_t *next, *bmq = desc_head;
  int l = strlen (bm_name);
  for(BMDESCRIPT *bm=(BMDESCRIPT*)add_ptr(bmq,bmq->next); bmq != bm; bm=add_ptr(bm,next))  {
    int l1 = strlen (bm->bm_name);
    next	= bm->next ;
    if (l1 == l)    {
      if (strncmp (bm->bm_name, bm_name, l) == 0)      {
        return 0;
      }
    }
  }
  return -1;
}

/// add user in the wait_event_slot queue
int _mbm_add_wes (BMDESCRIPT *bm, USER *us, MBM_ast_t astadd)  {
  CONTROL *ctrl = bm->ctrl;
  us->p_state   = S_weslot;
  us->p_astadd  = astadd;
  us->p_astpar  = bm;
  insqti (&us->wesnext, &ctrl->wes_head);
  return MBM_NORMAL;
}

/// del user from the wait_event_slot queue
int _mbm_del_wes (BMDESCRIPT* /* bm */, USER* us)   {
  qentry_t* dummy, *hd	= add_ptr(&us->wesnext,us->wesnext.prev);
  us->p_state = S_weslot_ast_queued;
  remqhi (hd , &dummy);
  return MBM_NORMAL;
}

/// check wait event slot
int _mbm_check_wes (BMDESCRIPT *bm)   {
  USER *us1;
  qentry_t park(0,0);
  CONTROL *ctrl = bm->ctrl;
  for(int sc = remqhi(&ctrl->wes_head,(qentry_t**)&us1);
      QR_success(sc);
      sc = remqhi(&ctrl->wes_head,(qentry_t**)&us1))  {
    insqhi (us1, &park);
    USER* us  = add_ptr(us1,-USER_wes_off);
    Check_USER(us);
    if (us->p_state == S_weslot)    {
      _mbm_del_wes (bm, us);
      _mbm_wake_process (BM_K_INT_ESLOT, us);
      break;
    }
  }
  return _mbm_restore_park_queue(&park,&ctrl->wes_head);
}


/* Statistics routines */
int mbm_events_actual (BMDESCRIPT *bm, int *events)   {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    *events = us->ev_actual;
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_events_produced (BMDESCRIPT *bm, int *events)   {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    *events = us->ev_produced;
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_events_seen (BMDESCRIPT *bm, int *events)   {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    *events = us->ev_seen;
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_reset_statistics (BMDESCRIPT *bm) {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    us->ev_actual   = 0;
    us->ev_produced = 0;
    us->ev_seen     = 0;
    return MBM_NORMAL;
  }
  return user.status();
}

int mbm_buffer_size (BMDESCRIPT *bm, int* size) {
  Lock lock(bm);
  if ( lock )  {
    CONTROL *ctrl = bm->_control();
    if ( ctrl )  {
      *size = ctrl->buff_size;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_ILL_CONS);
  }
  return lock.status();
}

int mbm_events_in_buffer (BMDESCRIPT *bm, int* events)  {
  Lock lock(bm);
  if ( lock )  {
    CONTROL *ctrl = bm->_control();
    if ( ctrl )  {
      *events = ctrl->i_events;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_ILL_CONS);
  }
  return lock.status();
}

int mbm_space_in_buffer (BMDESCRIPT *bm, int* total, int* large)  {
  Lock lock(bm);
  if ( lock )  {
    CONTROL *ctrl = bm->_control();
    if ( ctrl )  {
      int s,l;
      *total = ctrl->i_space << Shift_p_Bit;
      /* find largest block */
      BF_count(bm->bitmap, ctrl->bm_size, &s, &l);
      *large = l << Shift_p_Bit;
      return MBM_NORMAL;
    }
    _mbm_return_err (MBM_ILL_CONS);
  }
  return lock.status();
}

int mbm_process_exists (BMDESCRIPT *bm, const char* name, int* exists)  {
  int owner = bm->owner;
  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  int status = _mbm_findnam (bm, name);
  *exists =  (status == -1) ? 0 : 1;
  return MBM_NORMAL;
}

int  mbm_wait_event(BMDESCRIPT *bm)    {
  int sc, owner = bm->owner;
  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  USER* us = bm->_user();
  if ( us->held_eid != -1 )  {
    Lock lock(bm);
    _mbm_printf("WEV: State=%d  %s \n", us->c_state, us->c_state == S_wevent_ast_queued ? "OK" : "BAAAAD");
    if ( us->c_state == S_wevent_ast_queued )  {
      lib_rtl_clear_event (	bm->WEV_event_flag);
      us->reason        = BM_K_INT_EVENT;
      us->get_wakeups++;
      lib_rtl_run_ast(us->c_astadd, us->c_astpar, 3);
      us->c_state = S_active;
      return MBM_NORMAL;
    }
  }
Again:
  sc = 1;
  if ( us->c_state != S_wevent_ast_queued )  {
    _mbm_printf("Wait...lib_rtl_wait_for_event\n");
    sc = lib_rtl_wait_for_event(bm->WEV_event_flag);
  }
  if ( lib_rtl_is_success(sc) )  {
    Lock lock(bm);
    lib_rtl_clear_event (	bm->WEV_event_flag);
    if (us->held_eid == -1)    {
      goto Again;
    }
    us->reason        = BM_K_INT_EVENT;
    us->get_wakeups++;
    lib_rtl_run_ast(us->c_astadd, us->c_astpar, 3);
    us->c_state = S_active;
    return MBM_NORMAL;
  }
  MBM_ERROR;
  _mbm_return_err (MBM_ILL_CONS);
}

int mbm_wait_event_a(BMDESCRIPT* bm)    {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    int sc = lib_rtl_wait_for_event_a(bm->WEVA_event_flag,(lib_rtl_thread_routine_t)mbm_wait_event,bm);
    if ( lib_rtl_is_success(sc) )  {
      return MBM_NORMAL;
    }
    return MBM_INTERNAL;
  }
  return user.status();
}

int  mbm_wait_space(BMID bm)    {
  int sc = 1;
  int owner = bm->owner;
  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_REQ);
  }
  USER* us = bm->_user();
wait:
  if ( us->p_state == S_weslot_ast_queued )  {
    sc = lib_rtl_wait_for_event(bm->WES_event_flag);
    lib_rtl_clear_event(bm->WES_event_flag);
    lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
  }
  else if ( us->p_state == S_wspace_ast_queued )    {
    _mbm_printf("Wait...lib_rtl_wait_for_event\n");
    sc = lib_rtl_wait_for_event(bm->WSP_event_flag);
    if ( lib_rtl_is_success(sc) )  {
      lib_rtl_clear_event(bm->WSP_event_flag);
      if ( us->p_state == S_wspace_ast_queued )  {
        Lock lock(bm);
        _mbm_printf("Got space\n");
        lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
        return MBM_NORMAL;
      }
    }
  }
  goto wait;
  _mbm_return_err (MBM_ILL_CONS);
}

long _mbm_wait_space_a(void* param)  {
  BMDESCRIPT* bm = (BMDESCRIPT*)param;
  while(1)  {
    lib_rtl_clear_event(bm->WSPA_event_flag);
    if ( mbm_wait_space(bm) != MBM_NORMAL )  {   
    }
    lib_rtl_wait_for_event(bm->WSPA_event_flag);
  }
  return 1;
}

int  mbm_wait_space_a(BMDESCRIPT* bm)    {
  UserLock user(bm);
  USER* us = user.user();
  if ( us )  {
    int sc = 0;
    if ( 0 == bm->pThread )  {
      sc = lib_rtl_start_thread(_mbm_wait_space_a, bm, &bm->pThread);
      if ( !(sc & 1) )  {
        lib_rtl_signal_message(0,"Failed to manipulate producer BM thread");
        return MBM_INTERNAL;
      }
    }
    lib_rtl_set_event(bm->WSPA_event_flag);
    return MBM_NORMAL;
  }
  return user.status();
}

/// Wakeup process to continue processing
int _mbm_wake_process (int reason, USER* us) {
  us->reason = reason;
  int status = 0;
  switch(reason)  {
    case BM_K_INT_EVENT:
      status = lib_rtl_wakeup_process(us->pid, us->wev_flag, us->c_astadd, us->c_astpar, 3);
      break;
    case BM_K_INT_ESLOT:
      status = lib_rtl_wakeup_process(us->pid, us->wes_flag, us->p_astadd, us->p_astpar, 3);
      break;
    case BM_K_INT_SPACE:
      status = lib_rtl_wakeup_process(us->pid, us->wsp_flag, us->p_astadd, us->p_astpar, 3);
      break;
  }
  if (!lib_rtl_is_success(status))  {
    lib_rtl_start_debugger();
  }
  return MBM_NORMAL;
}

int _mbm_fill_offsets() {
  byte_offset(USER,next,USER_next_off);
  byte_offset(USER,wsnext,USER_ws_off);
  byte_offset(USER,wenext,USER_we_off);
  byte_offset(USER,wesnext,USER_wes_off);
  byte_offset(EVENT,next,EVENT_next_off);
  byte_offset(CONTROL,u_head,CONTROL_cons_off);
  byte_offset(CONTROL,wev_head,CONTROL_cwe_off);
  byte_offset(CONTROL,wsp_head,CONTROL_pws_off);
  byte_offset(CONTROL,wes_head,CONTROL_wes_off);
  byte_offset(CONTROL,e_head,CONTROL_ev_off);
  return MBM_NORMAL;
}

int mbm_get_event_ast(void* par) {
  BMDESCRIPT *bm = (BMDESCRIPT*)par;
  USER       *us = bm->user+bm->owner;
  if (us == 0)  {
    return 1;
  }
  us->get_asts_run++;
  if (us->c_state != S_wevent_ast_queued)  {
    ::printf("us->c_state Not S_wevent_ast_queued, but is %d\n",us->c_state);
    return 1;
  }
  us->c_state = S_active;
  EVENT* ev	= bm->event+us->held_eid;
  if (us->reason != BM_K_INT_EVENT)  {
    ::printf("event_ast spurious wakeup reason = %d\n",us->reason);
    us->reason = S_wevent_ast_handled;
    return 1;
  }
  us->reason = S_wevent_ast_handled;
  int buff_add = (int)bm->buffer_add;
  *us->we_ptr_add    = (int*)(ev->ev_add+buff_add);
  *us->we_ptr_add   += sizeof(int);
  *us->we_size_add   = ev->ev_size;
  *us->we_evtype_add = ev->ev_type;
  *us->we_trmask_add = ev->tr_mask;
  return MBM_NORMAL;
}

int  mbm_get_space_ast(void* par) {
  BMDESCRIPT *bm = (BMDESCRIPT*)par;
  USER *us = bm->_user();
  if ( us == 0 )  {
    return MBM_NORMAL;
  }
  if (us->p_state != S_wspace_ast_queued)   {
    printf("us->p_state Not S_wspace_ast_queued, but is %d\n",us->p_state);
    us->p_state = S_wspace_ast_handled;
    us->reason = -1;
    return MBM_NORMAL;
  }
  us->p_state = S_active;
  if ( !(us->reason&BM_K_INT_SPACE) )   {
    printf("space_ast spurious wakeup reason = %d\n",us->reason);
    us->p_state = S_wspace_ast_handled;
    us->reason = -1;
    return MBM_NORMAL;
  }
  *us->ws_ptr_add = (int*)(us->space_add+bm->buffer_add);
  *us->ws_ptr_add += sizeof(int);
  us->p_state = S_wspace_ast_handled;
  us->reason = -1;
  return MBM_NORMAL;
}

/// Wait event space AST function
int _mbm_wes_ast(void* par)   {
  BMDESCRIPT *bm = (BMDESCRIPT*)par;
  USER *us	= bm->user+bm->owner;
  if (us->p_state == S_weslot_ast_queued)  {
    us->p_state	= S_active;
    // lib_rtl_set_event (bm->WES_event_flag);
    return 1;
  }
  ::printf("_mbm_wes_ast state is not S_weslot_ast_queued is %d. IGNORED\n",us->p_state);
  return 0;
}


int _mbm_restore_park_queue(qentry_t *p, qentry_t *d) {
  qentry_t *e;
  for (int sc=remqhi(p,&e);QR_success(sc);sc=remqhi(p,&e))  {
    insqhi (e, d);
  }
  return MBM_NORMAL;
}

int _mbm_declare_event (BMDESCRIPT *bm, int len, int evtype, TriggerMask& trmask,
                        const char* dest, void** free_add, int* free_size, int part_id)
{
  char*  add;
  int eid, uid, rlen;
  UserMask mask0(0);
  UserMask mask1(0);
  int      owner = bm->owner;
  CONTROL* ctrl  = bm->ctrl;

  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  USER* us = bm->user + owner;
  if (us->uid != owner)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  if (!us->space_size)  {
    _mbm_return_err (MBM_NO_EVENT);
  }
  if (len <= 0)   {
    _mbm_return_err (MBM_ZERO_LEN);
  }
  if (len > us->space_size)  {
    _mbm_return_err (MBM_EV_TOO_BIG);
  }
  rlen = ((len + Bytes_p_Bit) >> Shift_p_Bit) << Shift_p_Bit;
  add = us->space_add+bm->buffer_add;
  /* find all destinations */
  *(int*)add = len/4;
  if (dest)  {
    uid = _mbm_findnam (bm, dest);
    if (uid != -1)    {
      mask0.bit_set(uid);
    }
  }
  do  {
    eid = _mbm_ealloc (bm);
    if (eid == -1)    {
      if (_mbm_check_freqmode (bm) == 0)  {
        eid = _mbm_ealloc (bm);
        if (eid == -1)   {
          /* add on the wait event slot queue */
          _mbm_add_wes (bm, us, _mbm_wes_ast_add);
          lib_rtl_clear_event (	bm->WES_event_flag);
          _mbm_unlock_tables(bm);
          lib_rtl_wait_for_event (bm->WES_event_flag);
          if (us->p_state == S_weslot_ast_queued)  {
            lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
          }
          _mbm_lock_tables(bm);
          if (us->p_state != S_active)   {
            _mbm_return_err (MBM_REQ_CANCEL); /* other reasons */
          }
        }
      }
      else   {
        /* add on the wait event slot queue */
        _mbm_add_wes (bm, us, _mbm_wes_ast_add);
        lib_rtl_clear_event (	bm->WES_event_flag);
        _mbm_unlock_tables(bm);
        lib_rtl_wait_for_event (bm->WES_event_flag);
        if (us->p_state == S_weslot_ast_queued)  {
          lib_rtl_run_ast(us->p_astadd, us->p_astpar, 3);
        }
        _mbm_lock_tables(bm);
        if (us->p_state != S_active)        {
          _mbm_return_err (MBM_REQ_CANCEL);	/* other reasons */
        }
      }
    }
  } while (eid == -1);
  _mbm_match_req (bm, part_id, evtype, trmask, mask0, mask1);
  if ( mask0.mask_or_ro(mask1) )  {
    EVENT *ev = bm->event + eid;
    ev->busy    = 2;		/* exclusive use of event */
    ev->partid  = part_id;
    ev->ev_type = evtype;
    ev->tr_mask = trmask;
    ev->ev_add  = us->space_add;
    ev->ev_size = len;
    ev->umask0  = mask0;
    ev->umask1  = mask1;
    ev->held_mask.clear();
    ev->held_mask.bit_set(owner);
    _mbm_printf("Got slot:%d %d  %08X\n",ev->eid, ev->count, ev->ev_add);
  }
  else  {
    _mbm_efree (bm, eid);
    _mbm_sfree (bm, us->space_add, rlen);	/* free event space */
  }
  *free_add = add + rlen;
  us->space_add = (int)*free_add - (int)bm->buffer_add;
  us->space_size = (*free_size = us->space_size - rlen);
  if (us->space_size == 0)  {
    us->space_add = 0;	/* if size zero, address zero */
  }
  ctrl->tot_produced++;
  us->ev_produced++;
  return MBM_NORMAL;
}

int _mbm_check_cons (BMDESCRIPT *bm)  {
  EVENT *ev, *ev1;
  qentry_t park(0,0);
  int      owner = bm->owner;
  CONTROL* ctrl  = bm->ctrl;

  if (owner == -1)  {
    _mbm_return_err (MBM_ILL_CONS);
  }
  USER* us = bm->user + owner;
  if (us->uid != owner)  {
    _mbm_return_err (MBM_INTERNAL);
  }
  for (int sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1);
       QR_success(sc);
       sc = remqhi(&ctrl->e_head,(qentry_t**)&ev1) )
  {
    insqhi (ev1, &park);
    ev  = add_ptr(ev1,-EVENT_next_off);
    Check_EVENT(ev);
    if (ev->busy != 2)     {
      continue;
    }
    if ( ev->held_mask.bit_test(owner) )      {
      ev->held_mask.bit_clear(owner);
      ev->busy = 1;
      _mbm_printf("_mbm_send_space EVENT: %d %d\n",ev->eid, ev->count);
      _mbm_check_wev (bm, ev);	/* check wev queue */
    }
  }
  _mbm_restore_park_queue(&park,&ctrl->e_head);
  return MBM_NORMAL;
}

int _mbm_send_space (BMDESCRIPT *bm)  {
  int sc = _mbm_check_cons(bm);
  if ( sc == MBM_NORMAL )   {
    USER* us = bm->user + bm->owner;
    if (us->space_size)  {
      _mbm_printf("_mbm_send_space> Free event space\n");
      _mbm_sfree (bm, us->space_add, us->space_size);	/* free space */
    }
    us->space_add = 0;
    us->space_size = 0;
  }
  return sc;
}

int _mbm_create_lock(BMDESCRIPT *bm)    {
  if ( !bm->lockid )  {
    ::strcpy(bm->mutexName,"BM_");
    ::strcat(bm->mutexName,bm->bm_name);
    int status = lib_rtl_create_lock(bm->mutexName, &bm->lockid);
    if (!lib_rtl_is_success(status))    {
      ::printf("error in creating lock %s. Status %d\n",bm->mutexName,status);
    }
    return status;
  }
  return 0;
}

int _mbm_lock_tables(BMDESCRIPT *bm)  {
  if ( bm->lockid )  {
    int status = lib_rtl_lock(bm->mutexName,bm->lockid);
    if (!lib_rtl_is_success(status))    { 
      ::printf("error in unlocking tables. Status %d\n",status);
    }
    return status;
  }
  ::printf("error in unlocking tables [Invalid Mutex] %s.\n",bm->mutexName);
  return 0;
}

int _mbm_unlock_tables(BMDESCRIPT *bm)    {
  if ( bm->lockid )  {
    int status = lib_rtl_unlock(bm->lockid);
    if (!lib_rtl_is_success(status))    { 
      ::printf("error in unlocking tables %s. Status %d\n",bm->mutexName,status);
    }
    return status;
  }
  ::printf("error in unlocking tables %s [Invalid Mutex].\n",bm->mutexName);
  return 0;
}

int _mbm_delete_lock(BMDESCRIPT *bm)    {
  if ( bm->lockid )  {
    int status = lib_rtl_delete_lock(bm->mutexName,bm->lockid);
    if (!lib_rtl_is_success(status))    { 
      ::printf("error in deleting lock %s. Status %d\n",bm->mutexName,status);
    }
    else  {
      bm->lockid = 0;
    }
    return status;
  }
  ::printf("error in deleting lock %s [Invalid Mutex].\n", bm->mutexName);
  return 0;
}

int _mbm_cancel_lock(BMDESCRIPT *bm)   {
  if ( bm->lockid )  {
    int status = lib_rtl_cancel_lock(bm->mutexName,bm->lockid);
    if (!lib_rtl_is_success(status))    { 
      ::printf("error in cancelling lock %s. Status %d\n",bm->mutexName,status);
    }
    else  {
      bm->lockid = 0;
    }
    return status;
  }
  ::printf("error in cancelling lock %s [Invalid Mutex].\n", bm->mutexName);
  return 0;
}


int _mbm_create_section(const char* sec_name, int size, void* address) {
  return lib_rtl_create_section(sec_name, size, address);
}

int _mbm_delete_section(const char *sec_name)  {
  return lib_rtl_delete_section(sec_name);
}

int _mbm_map_section(const char* sec_name, int size, void* address)   {
  return lib_rtl_map_section(sec_name, size, address);
}

/// Unmap global section: address is quadword: void*[2]
int _mbm_unmap_section(void* address)   {
  return lib_rtl_unmap_section(address);
}

int _mbm_flush_sections(BMDESCRIPT* bm)   {
  lib_rtl_flush_section(bm->ctrl_add, sizeof(CONTROL));
  lib_rtl_flush_section(bm->event_add,bm->ctrl->p_emax*sizeof(EVENT));
  lib_rtl_flush_section(bm->user_add, bm->ctrl->p_umax*sizeof(USER));
  lib_rtl_flush_section(bm->buff_add, bm->ctrl->buff_size);
  lib_rtl_flush_section(bm->bitm_add, bm->ctrl->bm_size);
  return 1;
}
