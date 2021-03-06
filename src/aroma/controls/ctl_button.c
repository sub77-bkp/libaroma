/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : ctl_button.c
 * Description : button control
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 06/02/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_ctl_button_c__
#define __libaroma_ctl_button_c__
#include <aroma_internal.h>
#include "../ui/ui_internal.h"

#define _LIBAROMA_CTL_BUTTON_HOLD_TIMING 300
#define _LIBAROMA_CTL_BUTTON_ANI_TIMING 500

/* HANDLER */
dword _libaroma_ctl_button_msg(LIBAROMA_CONTROLP, LIBAROMA_MSGP);
void _libaroma_ctl_button_draw (LIBAROMA_CONTROLP, LIBAROMA_CANVASP);
void _libaroma_ctl_button_destroy(LIBAROMA_CONTROLP);
byte _libaroma_ctl_button_thread(LIBAROMA_CONTROLP);
static LIBAROMA_CONTROL_HANDLER _libaroma_ctl_button_handler={
  message:_libaroma_ctl_button_msg,
  draw:_libaroma_ctl_button_draw,
  focus:NULL,
  destroy:_libaroma_ctl_button_destroy,
  thread:_libaroma_ctl_button_thread
};

/*
 * Structure   : __LIBAROMA_CTL_BUTTON
 * Typedef     : _LIBAROMA_CTL_BUTTON, * _LIBAROMA_CTL_BUTTONP
 * Descriptions: button control internal structure
 */
typedef struct __LIBAROMA_CTL_BUTTON _LIBAROMA_CTL_BUTTON;
typedef struct __LIBAROMA_CTL_BUTTON * _LIBAROMA_CTL_BUTTONP;
struct __LIBAROMA_CTL_BUTTON{
  char * text;
  byte style;
  byte isdark;
  word color;
  byte forcedraw;
  
  LIBAROMA_RIPPLE ripple;
  LIBAROMA_MUTEX mutex;
  LIBAROMA_CANVASP rest_canvas;
  LIBAROMA_CANVASP push_canvas;
};

/*
 * Function    : _libaroma_ctl_button_internal_draw
 * Return Value: void
 * Descriptions: internal button draw
 */
void _libaroma_ctl_button_internal_draw(LIBAROMA_CONTROLP ctl){
  _LIBAROMA_CTL_BUTTONP me = (_LIBAROMA_CTL_BUTTONP) ctl->internal;
  if (ctl->window==NULL){
    return;
  }
  
  
  LIBAROMA_CANVASP rest_canvas=NULL;
  LIBAROMA_CANVASP push_canvas=NULL;
  
  byte is_disabled = (me->style&LIBAROMA_CTL_BUTTON_DISABLED)?1:0;
  rest_canvas = libaroma_canvas(ctl->w,ctl->h);
  if (!is_disabled){
    push_canvas = libaroma_canvas(ctl->w,ctl->h);
  }
  LIBAROMA_CANVASP bg = libaroma_canvas(ctl->w,ctl->h);
  libaroma_control_erasebg(ctl,rest_canvas);
  if (!is_disabled){
    libaroma_control_erasebg(ctl,push_canvas);
  }
  libaroma_control_erasebg(ctl,bg);
  
  int ix = libaroma_dp(4);
  int iy = libaroma_dp(6);
  int iw = ctl->w-ix*2;
  int ih = ctl->h-iy*2;
  
  if (me->style&LIBAROMA_CTL_BUTTON_COLORED) {
    me->isdark = libaroma_color_isdark(me->color);
  }
  else{
    me->isdark = libaroma_color_isdark(libaroma_colorget(ctl,NULL)->window_bg);
  }
  word push_color = me->isdark?RGB(ffffff):RGB(000000);
  word text_color = me->isdark?0xffff:0;
  word rest_text_color = text_color;
  byte push_opa   = me->isdark?128:98;
  
  /* buttons */
  LIBAROMA_CANVASP btnmask = libaroma_canvas_ex(iw,ih,1);
  libaroma_canvas_setcolor(btnmask,0,0);
  libaroma_gradient(btnmask,
    0,0,
    ctl->w-libaroma_dp(8), ctl->h-libaroma_dp(12),
    0,0,
    libaroma_dp(2),
    0x1111
  );
  
  /* raised shadow */
  if (me->style&LIBAROMA_CTL_BUTTON_RAISED){
    int raised_sz;
    LIBAROMA_CANVASP raised_canvas;
    
    raised_sz = libaroma_dp(2);
    raised_canvas = libaroma_blur_ex(btnmask,
      raised_sz,1,RGB(000000)
    );
    libaroma_draw_opacity(
      rest_canvas,raised_canvas,
      ix-raised_sz,
      iy-raised_sz+libaroma_dp(1),
      1,40
    );
    libaroma_canvas_free(raised_canvas);
    
    if (!is_disabled){
      raised_sz = libaroma_dp(4);
      raised_canvas = libaroma_blur_ex(btnmask,
        raised_sz,1,RGB(000000)
      );
      libaroma_draw_opacity(
        push_canvas,raised_canvas,
        ix-raised_sz,
        iy-raised_sz+libaroma_dp(3),
        1,50
      );
      libaroma_canvas_free(raised_canvas);
    }
  }
  
  if (me->style&LIBAROMA_CTL_BUTTON_COLORED) {
    if (is_disabled){
      libaroma_draw_ex(btnmask,bg,0,0,ix,iy,btnmask->w,btnmask->h,0,0xff);
      libaroma_draw_rect(btnmask,0,0,btnmask->w,btnmask->h,
        me->isdark?0xffff:0,127);
    }
    else{
      libaroma_draw_rect(btnmask,0,0,btnmask->w,btnmask->h,me->color,0xff);
    }
    if (me->style&LIBAROMA_CTL_BUTTON_RAISED){
      libaroma_draw(rest_canvas, btnmask, ix, iy, 1);
    }
    else{
      rest_text_color = me->color;
    }
    if (!is_disabled){
      libaroma_draw_rect(btnmask,0,0,btnmask->w,btnmask->h,push_color,push_opa);
      libaroma_draw(push_canvas, btnmask, ix, iy, 1);
    }
  }
  else{
    if (me->style&LIBAROMA_CTL_BUTTON_RAISED){
      libaroma_draw_ex(btnmask,bg,0,0,ix,iy,btnmask->w,btnmask->h,0,0xff);
      libaroma_draw(rest_canvas, btnmask, ix, iy, 1);
    }
    if (!is_disabled){
      libaroma_draw_ex(btnmask,bg,0,0,ix,iy,btnmask->w,btnmask->h,0,0xff);
      libaroma_draw_rect(btnmask,0,0,btnmask->w,btnmask->h,push_color,push_opa);
      libaroma_draw(push_canvas, btnmask, ix, iy, 1);
    }
  }
  
  /* draw text */
  libaroma_mutex_lock(me->mutex);
  LIBAROMA_TEXT textp = libaroma_text(
    me->text,
    text_color,
    iw - libaroma_dp(16),
    LIBAROMA_FONT(0,4)|
    LIBAROMA_TEXT_SINGLELINE|
    LIBAROMA_TEXT_CENTER|
    LIBAROMA_TEXT_FIXED_INDENT|
    LIBAROMA_TEXT_FIXED_COLOR|
    LIBAROMA_TEXT_NOHR,
    100
  );
  libaroma_mutex_unlock(me->mutex);
  int y = (ctl->h>>1) - ((libaroma_text_height(textp)>>1)+libaroma_dp(1));
  
  if (is_disabled){
    rest_text_color=me->isdark?0xffff:0;
  }
  libaroma_text_draw_color(rest_canvas,textp,libaroma_dp(12)+ix,y,
    rest_text_color
  );
  
  if (!is_disabled){
    libaroma_text_draw(push_canvas,textp,libaroma_dp(12)+ix,y);
  }
  else{
    libaroma_draw_ex(rest_canvas,bg,0,0,0,0,ctl->w,ctl->h,0,
      me->isdark?171:189);
  }
  libaroma_text_free(textp);
  
  /* cleanup */
  libaroma_canvas_free(bg);
  libaroma_canvas_free(btnmask);
  
  /* save it */
  libaroma_mutex_lock(me->mutex);
  LIBAROMA_CANVASP rc=me->rest_canvas;
  LIBAROMA_CANVASP pc=me->push_canvas;
  me->rest_canvas=rest_canvas;
  me->push_canvas=push_canvas;
  if (rc!=NULL){
    libaroma_canvas_free(rc);
  }
  if (pc!=NULL){
    libaroma_canvas_free(pc);
  }
  me->forcedraw=1;
  libaroma_mutex_unlock(me->mutex);
} /* End of _libaroma_ctl_button_internal_draw */

static void * _libaroma_ctl_button_req_internal_draw_thread(void * ctl){
  _libaroma_ctl_button_internal_draw((LIBAROMA_CONTROLP) ctl);
  return NULL;
}
void _libaroma_ctl_button_req_internal_draw(LIBAROMA_CONTROLP ctl){
  LIBAROMA_THREAD pp;
  libaroma_thread_create(
    &pp,_libaroma_ctl_button_req_internal_draw_thread,(void *) ctl);
  libaroma_thread_detach(pp);
}

/*
 * Function    : _libaroma_ctl_button_draw
 * Return Value: void
 * Descriptions: draw callback
 */
void _libaroma_ctl_button_draw(
    LIBAROMA_CONTROLP ctl,
    LIBAROMA_CANVASP c){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 
  );
  libaroma_mutex_lock(me->mutex);
  if ((me->rest_canvas==NULL)||(me->forcedraw==2)){
    libaroma_mutex_unlock(me->mutex);
    _libaroma_ctl_button_internal_draw(ctl);
    libaroma_mutex_lock(me->mutex);
    if (me->rest_canvas==NULL){
      libaroma_mutex_unlock(me->mutex);
      return;
    }
  }
  me->forcedraw = 0;
  
  
  
  libaroma_control_erasebg(ctl,c);
  if (me->style&LIBAROMA_CTL_BUTTON_DISABLED){
    libaroma_draw(c, me->rest_canvas, 0, 0, 0);
  }
  else{
    int ripple_i = 0;
    int ripple_p = 0;
    libaroma_draw(c, me->rest_canvas, 0, 0, 0);
    while(libaroma_ripple_loop(&me->ripple,&ripple_i,&ripple_p)){
      int x=0;
      int y=0;
      int size=0;
      byte push_opacity=0;
      byte ripple_opacity=0;
      if (libaroma_ripple_calculation(
        &me->ripple, ctl->w, ctl->h, &push_opacity, &ripple_opacity,
        &x, &y, &size,ripple_p
        )){
          libaroma_draw_mask_circle(
            c, me->push_canvas, x, y, x, y, size, ripple_opacity
          );
          libaroma_draw_opacity(c,me->push_canvas,0,0,0,push_opacity);
      }
    }
  }
  libaroma_mutex_unlock(me->mutex);
} /* End of _libaroma_ctl_button_draw */

/*
 * Function    : _libaroma_ctl_button_thread
 * Return Value: byte
 * Descriptions: control thread callback
 */
byte _libaroma_ctl_button_thread(LIBAROMA_CONTROLP ctl) {
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  byte is_draw = me->forcedraw;
  if (!(me->style&LIBAROMA_CTL_BUTTON_DISABLED)){
    byte res = libaroma_ripple_thread(&me->ripple, 0);
    if (res&LIBAROMA_RIPPLE_REDRAW){
      is_draw = 1;
    }
    if (res&LIBAROMA_RIPPLE_HOLDED){
      libaroma_window_post_command(
        LIBAROMA_CMD_SET(LIBAROMA_CMD_HOLD, 0, ctl->id)
      );
    }
  }
  if (is_draw){
    return 1;
  }
  
  return 0;
} /* End of _libaroma_ctl_button_thread */

/*
 * Function    : _libaroma_ctl_button_destroy
 * Return Value: void
 * Descriptions: destroy callback
 */
void _libaroma_ctl_button_destroy(
    LIBAROMA_CONTROLP ctl){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 
  );
  libaroma_mutex_lock(me->mutex);
  if (me->rest_canvas!=NULL){
    libaroma_canvas_free(me->rest_canvas);
    me->rest_canvas=NULL;
  }
  if (me->push_canvas!=NULL){
    libaroma_canvas_free(me->push_canvas);
    me->push_canvas=NULL;
  }
  if (me->text!=NULL){
    free(me->text);
  }
  libaroma_mutex_unlock(me->mutex);
  libaroma_mutex_free(me->mutex);
  free(me);
} /* End of _libaroma_ctl_button_destroy */

/*
 * Function    : _libaroma_ctl_button_msg
 * Return Value: byte
 * Descriptions: message callback
 */
dword _libaroma_ctl_button_msg(
    LIBAROMA_CONTROLP ctl,
    LIBAROMA_MSGP msg){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  
  switch(msg->msg){
    case LIBAROMA_MSG_TOUCH:
      {
        if (me->style&LIBAROMA_CTL_BUTTON_DISABLED){
          /* no thing for disabled button */
          return 0;
        }
        int x = msg->x;
        int y = msg->y;
        libaroma_window_calculate_pos(NULL,ctl,&x,&y);
        
        /* touch handler */
        if (msg->state==LIBAROMA_HID_EV_STATE_DOWN){
          libaroma_ripple_down(&me->ripple, x, y);
        }
        else if (msg->state==LIBAROMA_HID_EV_STATE_UP){
          byte res = libaroma_ripple_up(&me->ripple,0);
          if ((res&LIBAROMA_RIPPLE_TOUCHED)&&
              (!(res&LIBAROMA_RIPPLE_HOLDED))){
            libaroma_window_post_command(
              LIBAROMA_CMD_SET(LIBAROMA_CMD_CLICK, 0, ctl->id)
            );
          }
        }
        else if (msg->state==LIBAROMA_HID_EV_STATE_MOVE){
          if (!((x>=0)&&(y>=0)&&(x<ctl->w)&&(y<ctl->h))) {
            libaroma_ripple_cancel(&me->ripple);
          }
          else{
            libaroma_ripple_move(&me->ripple, x, y);
          }
        }
      }
      break;
  }
  return 0;
} /* End of _libaroma_ctl_button_msg */

/*
 * Function    : libaroma_ctl_button
 * Return Value: LIBAROMA_CONTROLP
 * Descriptions: create button control
 */
LIBAROMA_CONTROLP libaroma_ctl_button(
    LIBAROMA_WINDOWP win,
    word id,
    int x, int y, int w, int h,
    char * text,
    byte button_style,
    word button_color
){
  /* init internal data */
  _LIBAROMA_CTL_BUTTONP me = (_LIBAROMA_CTL_BUTTONP)
      calloc(sizeof(_LIBAROMA_CTL_BUTTON),1);
  if (!me){
    ALOGW("libaroma_ctl_button alloc button memory failed");
    return NULL;
  }
  
  /* set internal data */
  libaroma_mutex_init(me->mutex);
  me->text = strdup(text);
  me->style = button_style;
  me->color = button_color;
  
  /* init control */
  LIBAROMA_CONTROLP ctl =
    libaroma_control_new(
      id,
      x, y, w, h,
      libaroma_dp(48),libaroma_dp(48), /* min size */
      (voidp) me,
      &_libaroma_ctl_button_handler,
      win
    );
  if (!ctl){
    if (me->text!=NULL){
      free(me->text);
    }
    free(me);
    return NULL;
  }
  return ctl;
} /* End of libaroma_ctl_button */

/*
 * Function    : libaroma_ctl_button_style
 * Return Value: byte
 * Descriptions: set button style
 */
byte libaroma_ctl_button_style(
    LIBAROMA_CONTROLP ctl,
    byte button_style,
    word button_color
){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  me->style = button_style;
  me->color = button_color;
  _libaroma_ctl_button_req_internal_draw(ctl);
  return 1;
} /* End of libaroma_ctl_button_style */

/*
 * Function    : libaroma_ctl_button_text
 * Return Value: byte
 * Descriptions: set button text
 */
byte libaroma_ctl_button_text(
    LIBAROMA_CONTROLP ctl,
    char * text
){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  libaroma_mutex_lock(me->mutex);
  if (me->text!=NULL){
    free(me->text);
  }
  me->text = strdup(text);
  libaroma_mutex_unlock(me->mutex);
  _libaroma_ctl_button_req_internal_draw(ctl);
  return 1;
} /* End of libaroma_ctl_button_text */

/*
 * Function    : libaroma_ctl_button_disable
 * Return Value: byte
 * Descriptions: set button disable state
 */
byte libaroma_ctl_button_disable(
    LIBAROMA_CONTROLP ctl,
    byte is_disable
){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  if (is_disable){
    me->style |= LIBAROMA_CTL_BUTTON_DISABLED;
  }
  else{
    me->style &= ~LIBAROMA_CTL_BUTTON_DISABLED;
  }
  _libaroma_ctl_button_req_internal_draw(ctl);
  return 1;
} /* End of libaroma_ctl_button_disable */

/*
 * Function    : libaroma_ctl_button_is_disabled
 * Return Value: byte
 * Descriptions: check if button is disabled
 */
byte libaroma_ctl_button_is_disabled(LIBAROMA_CONTROLP ctl){
  /* internal check */
  _LIBAROMA_CTL_CHECK(
    _libaroma_ctl_button_handler, _LIBAROMA_CTL_BUTTONP, 0
  );
  if (me->style&LIBAROMA_CTL_BUTTON_DISABLED){
    return 1;
  }
  return 0;
} /* End of libaroma_ctl_button_is_disabled */


#endif /* __libaroma_ctl_button_c__ */
