#include "LVGL_Example.h"
#include <lvgl.h>
#include <extra/lv_extra.h>
#include <extra/libs/lv_libs.h>

void Lvgl_BangbooFace(void)
{
  LV_IMG_DECLARE(BangbooEyes);
  lv_obj_t * img;

  img = lv_gif_create(lv_scr_act());
  lv_gif_set_src(img, &BangbooEyes);
  lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
  
}

void Lvgl_close(void)
{
    lv_obj_clean(lv_scr_act());
}


