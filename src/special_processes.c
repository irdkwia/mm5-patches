#include <pmdsky.h>
#include <cot.h>
#include "extern.h"

// This function isn't in pmdsky-debug yet, so we have to declare it
// here and add its offset in "symbols/custom_[region].ld".
extern void ChangeGlobalBorderColor(int color_type);

// Special process 100: Change border color
// Based on https://github.com/SkyTemple/eos-move-effects/blob/master/example/process/set_frame_color.asm
static int SpChangeBorderColor(short arg1) {
  ChangeGlobalBorderColor(arg1);
  return 0;
}

static int SpSetTextboxTransparency(short arg1) {
  if(arg1 == 0)
    TextboxSolid();
  else
    TextboxTransparent();
  return 0;
}

static bool SpCheckInputStatus(short arg1, short arg2) {
  struct button_struct buttons;
  int controller = 0;
  if(arg2 == 0)
    MyGetPressedButtons(controller, &buttons);
  else
    MyGetHeldButtons(controller, &buttons);
  return buttons.bitfield & arg1 ? true : false;
}

static bool SpCreateSpecialWindow(short idx)
{
  struct window_params window_params[] = { 
    {.x_offset = 0x1, .y_offset = 0x1, .width = 0x1E, .height = 0x4, .screen = 0x1, .box_type = 0xFF } 
  };
  struct preprocessor_flags dbox_flags[] = {
    {.flags_1 = 0b0010, .flags_11 = 0b00}
  };
  uint16_t message_ids[] = {11621};
  SPECIAL_DBOX_ID = CreateDialogueBox(&(window_params[idx]));
  SPECIAL_DBOX_TYPE = idx;
  SPECIAL_MESSAGE_ID = message_ids[idx];
  SPECIAL_PREPROCRESSOR_FLAGS = dbox_flags[idx];
  ShowStringIdInDialogueBox(SPECIAL_DBOX_ID, SPECIAL_PREPROCRESSOR_FLAGS, SPECIAL_MESSAGE_ID, NULL);
  SaveScriptVariableValueAtIndex(NULL, VAR_PERFORMANCE_PROGRESS_LIST, 62, 1);
  return true;
}

static bool SpCloseSpecialWindow()
{
  SaveScriptVariableValueAtIndex(NULL, VAR_PERFORMANCE_PROGRESS_LIST, 62, 0);
  CloseDialogueBox(SPECIAL_DBOX_ID);
  return true;
}


// Called for some special custom processes!

// Set return_val to the return value that should be passed back to the game's script engine. Return true,
// if the special process was handled.
bool CustomScriptSpecialProcessCall(undefined4* unknown, uint32_t special_process_id, short arg1, short arg2, int* return_val) {
  switch (special_process_id) {
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
      CreateNewSaveMenu();
      *return_val = 0;
      return true;
    case 100:
      *return_val = SpChangeBorderColor(arg1);
      return true;
    case 101:
      *return_val = SpSetTextboxTransparency(arg1);
      return true;
    case 102:
      *return_val = SpCheckInputStatus(arg1, arg2);
      return true;
    case 254:
      *return_val = SpCreateSpecialWindow(arg1);
      return true;
    case 255:
      *return_val = SpCloseSpecialWindow();
      return true;
    default:
      return false;
  }
}
