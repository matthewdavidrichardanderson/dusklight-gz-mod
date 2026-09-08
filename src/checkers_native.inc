// Original TPGZ checker state machines, adapted by tools/adapt-checkers.py.
// GPL-3.0.

namespace coro_td {
    static bool sTimerStarted = false;
    static bool sGoalHit = false;
    static uint32_t sFrameCount = 0;

#ifdef GCN_PLATFORM
#define ITEM_BUTTON_HELD_CHECK (!GZ_getButtonHold(GZPad::Y) || !GZ_getButtonHold(GZPad::X))
#define ITEM_BUTTON_DOWN_CHECK (GZ_getButtonPressed(GZPad::Y) || GZ_getButtonPressed(GZPad::X))
#define PAD Pad
#endif  // GCN_PLATFORM

#ifdef WII_PLATFORM
#define ITEM_BUTTON_HELD_CHECK                                                                     \
    (!GZ_getButtonHold(GZPad::B) || !GZ_getButtonHold(GZPad::DPAD_LEFT) ||                         \
     !GZ_getButtonHold(GZPad::DPAD_RIGHT) || !GZ_getButtonHold(GZPad::DPAD_DOWN))
#define ITEM_BUTTON_DOWN_CHECK                                                                     \
    (GZ_getButtonPressed(GZPad::B) || GZ_getButtonPressed(GZPad::DPAD_LEFT) ||                     \
     GZ_getButtonPressed(GZPad::DPAD_RIGHT) || GZ_getButtonPressed(GZPad::DPAD_DOWN))
#define PAD Mote
#endif  // WII_PLATFORM

void execute() {


    // reset counters on load
    if (sceneLoading()) {
        sFrameCount = 0;
        sGoalHit = false;
        sTimerStarted = false;
    }

    if (!sTimerStarted && GZ_getButtonPressed(GZPad::A) && daAlink_c::checkStageName("F_SP108")) {
        sTimerStarted = true;
    }

    if (sTimerStarted) {
        sFrameCount++;

        if (sFrameCount < 20) {
            if (!sGoalHit && ITEM_BUTTON_HELD_CHECK && ITEM_BUTTON_DOWN_CHECK) {
                char buf[20];
                if (sFrameCount < 10) {
                    snprintf(buf, sizeof(buf), "%df early", 10 - sFrameCount);
                    FIFOQueue::push(buf, Queue, 0x0000FF00);
                } else if (sFrameCount == 10) {
                    FIFOQueue::push("got it", Queue, 0x00CC0000);
                    sGoalHit = true;
                } else if (sFrameCount > 10) {
                    snprintf(buf, sizeof(buf), "%df late", sFrameCount - 10);
                    FIFOQueue::push(buf, Queue, 0x99000000);
                }
            }
        } else {
            sFrameCount = 0;
            sGoalHit = false;
            sTimerStarted = false;
        }
    }
}
void reset(){sTimerStarted=false;sGoalHit=false;sFrameCount=0;}

}

#undef ITEM_BUTTON_DOWN_CHECK
#undef ITEM_BUTTON_HELD_CHECK
#undef PAD
namespace ebmb {

char msg_buffer[40];     // buffer for the message to be printed
u16 previous_action;     // tracks the previous action
s32 eb_frame_delta;      // tracks the active (unpaused) frames since Ending Blow started
bool ib_on;              // tracks whether or not IB are equipped
bool ib_on_last_frame;   // tracks whether or not IB was equipped on the last frame

void checkFrame(daAlink_c* link) {
    switch (link->mProcID) {
    case daAlink_c::PROC_ATN_ACTOR_WAIT:
        previous_action = daAlink_c::PROC_ATN_ACTOR_WAIT;
        break;
    case daAlink_c::PROC_CUT_DOWN:
        if (previous_action == daAlink_c::PROC_ATN_ACTOR_WAIT) {
            eb_frame_delta = 0;
        }

        ib_on = link->checkEquipHeavyBoots();

        if (dComIfGp_isPauseFlag()) {
#if DEBUG

#endif
        } else {
            eb_frame_delta++;
#if DEBUG



#endif
            
            if (!ib_on && ib_on_last_frame) {
                if (eb_frame_delta == 4) {
                    FIFOQueue::push("<3", Queue, 0x00CC0000);
                } else if (eb_frame_delta > 4 && eb_frame_delta <= 10) {
                    snprintf(msg_buffer, sizeof(msg_buffer), "late by %df", (eb_frame_delta - 4));
                    FIFOQueue::push(msg_buffer, Queue, 0xCC000000);
                }
            } else if (!ib_on && eb_frame_delta == 3) {
                FIFOQueue::push("did you forget to equip iron boots?", Queue, 0x00CC0000);
            }

            ib_on_last_frame = ib_on;
            
        }
        previous_action = daAlink_c::PROC_CUT_DOWN;
        break;
    default:
        break;
    }
}

void execute() {
    daAlink_c* link = daAlink_getAlinkActorClass();

    if (!link) {
#if DEBUG

#endif
        return;
    }

    checkFrame(link);
}
void reset(){previous_action=0;eb_frame_delta=0;ib_on=false;ib_on_last_frame=false;}

}

namespace elevator_escape {

u16 previous_action;        // Tracks the previous action
char msg_buffer[20];        // Buffer for the message to be printed
s32 early_roll_frame;       // Frame where the early roll happened
s32 late_roll_frame;        // Frame where the late roll happened
s32 target_frame;           // Frame where the roll *should* happen to get the trick
static std::array<uint32_t,64> earlyPresses{};
static size_t earlyCount=0;
s32 metamorphose_start_frame; // frame that the metamorphose animation starts on

void checkRollFrame(daAlink_c* link) {
    if (sceneLoading()) { earlyCount=0; previous_action=0; return; }
    if (link->mProcID != daAlink_c::PROC_METAMORPHOSE && earlyCount) {
        for(size_t i=0;i<earlyCount;i++){
            snprintf(msg_buffer,sizeof(msg_buffer),"early by %uf",unsigned(g_Counter.mCounter0-earlyPresses[i]));
            FIFOQueue::push(msg_buffer,Queue,0x0000CC00);
        }
        earlyCount=0;
    }
    switch (link->mProcID) {
    case daAlink_c::PROC_TALK:
        previous_action = daAlink_c::PROC_TALK;
        break;
    case daAlink_c::PROC_METAMORPHOSE:
        if (previous_action == daAlink_c::PROC_TALK) {
            metamorphose_start_frame = g_Counter.mCounter0;
        }

#if DEBUG



#endif
            
        if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
            early_roll_frame = g_Counter.mCounter0;
            if(earlyCount<earlyPresses.size())earlyPresses[earlyCount++]=g_Counter.mCounter0;
        }

        previous_action = daAlink_c::PROC_METAMORPHOSE;
        break;

    case daAlink_c::PROC_WAIT:
        late_roll_frame = 0;
        target_frame = g_Counter.mCounter0;
        break;
    case daAlink_c::PROC_MOVE:
    case daAlink_c::PROC_WAIT_TURN:
    case daAlink_c::PROC_MOVE_TURN:
        if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
            late_roll_frame = g_Counter.mCounter0;
        }
        break;
    case daAlink_c::PROC_FRONT_ROLL:
#if DEBUG



#endif
        if (late_roll_frame == 0 && previous_action != daAlink_c::PROC_FRONT_ROLL) {
            FIFOQueue::push("<3", Queue, 0x00CC0000);
        } else if (previous_action != daAlink_c::PROC_FRONT_ROLL) {
            snprintf(msg_buffer, sizeof(msg_buffer), "late by %df", late_roll_frame - target_frame);
            FIFOQueue::push(msg_buffer, Queue, 0xCC000000);
        }

        previous_action = daAlink_c::PROC_FRONT_ROLL;
        break;
    default:
        early_roll_frame = 0;
        late_roll_frame = 0;
    }
}

void execute() {
    daAlink_c* link = daAlink_getAlinkActorClass();

    if (!link) {
#if DEBUG

#endif
        return;
    }

    if (!daAlink_c::checkStageName("R_SP110")) {
#if DEBUG

#endif
        return;
    }

    if (link->current.pos.x > -1400.0f || link->current.pos.x < -1600.0f) {
#if DEBUG

#endif
        return;
    }

    if (link->current.pos.z < 4000.0f || link->current.pos.z > 4400.0f) {
#if DEBUG

#endif
        return;
    }

    checkRollFrame(link);
}
void reset(){earlyCount=0;previous_action=0;early_roll_frame=late_roll_frame=target_frame=metamorphose_start_frame=0;}

}

namespace lfc {

enum Phase {
    LADDER_START,
    LADDER_DURING,
    LADDER_DURING_2,
    LADDER_DROP,
    LADDER_DROP_2,
    LADDER_REGRAB
};

u8 phase = Phase::LADDER_START;
int frameDelta = 0;
s32 drop_1_start_frame;       // The frame the first drop started on.
s32 drop_2_start_frame;       // The frame the second drop started on.
bool game_paused;

static void* searchLadderFreezard(void* i_actor, void* i_data) {
    const float LADDER_FREEZARD_Y_POS = 950.0f;

    if (fopAcM_GetName(static_cast<fopAc_ac_c*>(i_actor)) == fpcNm_E_FB_e && static_cast<fopAc_ac_c*>(i_actor)->current.pos.y == LADDER_FREEZARD_Y_POS) {
        return i_actor;
    }

    return NULL;
}

void checkOnLadder(daAlink_c* link) {
    u16 current_action = link->mProcID;
    
    if (current_action == daAlink_c::PROC_LADDER_UP_START ||
            current_action == daAlink_c::PROC_LADDER_UP_END ||
            current_action == daAlink_c::PROC_LADDER_DOWN_START ||
            current_action == daAlink_c::PROC_LADDER_DOWN_END ||
            current_action == daAlink_c::PROC_LADDER_MOVE) {
                if (phase == Phase::LADDER_START) {
                    phase = Phase::LADDER_DURING;
                } else if (phase == Phase::LADDER_DROP) {
                    phase = Phase::LADDER_DURING_2;
                } else {
                    phase = Phase::LADDER_REGRAB;
                }
    } else if (phase != Phase::LADDER_DROP) {
        phase = Phase::LADDER_START;
    }
}

void checkFirstDrop(daAlink_c* link) {
    if (link->mProcID == daAlink_c::PROC_FALL) {
        if (link->current.pos.y < 247.0f) {
            char msg[80];
            snprintf(msg, sizeof(msg), "off by -%f", 247.0f - link->current.pos.y);
            FIFOQueue::push(msg, Queue, 0xFFFF0000);
            phase = Phase::LADDER_START;
            return;
        }

        if (link->current.pos.y >= 247.0f && link->current.pos.y < 249.8f) {
            FIFOQueue::push("Got it (1/2)", Queue, 0x00FF0000);
            phase = Phase::LADDER_DROP;
            drop_1_start_frame = g_Counter.mCounter0;
            return;
        }

        if (link->current.pos.y >= 249.8f) {
            char msg[80];
            snprintf(msg, sizeof(msg), "off by +%f", link->current.pos.y - 249.8f);
            FIFOQueue::push(msg, Queue, 0xFF000000);
            phase = Phase::LADDER_START;
            return;
        }
    }
}

void checkSecondDrop(daAlink_c* link) {
    if (link->mProcID == daAlink_c::PROC_FALL) {
        drop_2_start_frame = g_Counter.mCounter0;
        s32 delta = drop_2_start_frame - drop_1_start_frame;
#if DEBUG

#endif


        if (delta < 260 && delta >= 255) {
            char msg[80];
            snprintf(msg, sizeof(msg), "early by %df", 260 - delta);
            FIFOQueue::push(msg, Queue, 0x00FF0000);
            phase = Phase::LADDER_DROP_2;
            return;
        }

        if (delta >= 260 && delta < 268) {
            FIFOQueue::push("Got it (2/2)", Queue, 0x00FF0000);
            phase = Phase::LADDER_REGRAB;
            return;
        }

        if (delta >= 268 && delta < 273) {
            char msg[80];
            snprintf(msg, sizeof(msg), "late by %df", delta - 267);
            FIFOQueue::push(msg, Queue, 0x00FF0000);
            phase = Phase::LADDER_DROP_2;
            return;
        }
        
        if (delta >= 273) {
            phase = Phase::LADDER_START;
            return;
        }
        
    }
}

void framePauseUpdate() {
    if (dComIfGp_isPauseFlag()) {
#if DEBUG

#endif

        drop_1_start_frame += 1;
        game_paused = true;
    } else {
        if (game_paused) {
            game_paused = false;
            drop_1_start_frame++;
        }
    }
}

void execute() {
    // Retrieve player pointer
    daAlink_c* link = daAlink_getAlinkActorClass();

    // Early return if player pointer is invalid
    if (!link) {
        return;
    }

    if (dComIfGp_getEvent()->mEventStatus == true) {
#if DEBUG

#endif
        return;
    }

    if (daAlink_getAlinkActorClass()->checkWolf()) {
#if DEBUG

#endif
        return;
    }

    if (!daAlink_c::checkStageName("D_MN11")) {
#if DEBUG

#endif
        return;
    }

    if (fopAcM_GetRoomNo(link) != 4) {
#if DEBUG

#endif
        return;
    }

    fopAc_ac_c* ladder_freezard = (fopAc_ac_c*)fopAcIt_Judge(searchLadderFreezard, nullptr);

    if (!ladder_freezard) {
#if DEBUG

#endif
        return;
    }

    switch(phase) {
        case Phase::LADDER_START:

            checkOnLadder(link);
            break;
        case Phase::LADDER_DURING:

            checkFirstDrop(link);
            break;
        case Phase::LADDER_DROP:

            checkOnLadder(link);
            framePauseUpdate();
            break;
        case Phase::LADDER_DURING_2:

            framePauseUpdate();
            checkSecondDrop(link);
            break;
        case Phase::LADDER_DROP_2:

            checkOnLadder(link);
            break;
        case Phase::LADDER_REGRAB:

        default:
            phase = Phase::LADDER_START;
            break;
    }
}
void reset(){phase=Phase::LADDER_START;frameDelta=0;drop_1_start_frame=drop_2_start_frame=0;game_paused=false;}

}

namespace rolling {

u8 current_frame_delta;     // The current active (unpaused) frame delta since the roll started.
u8 roll_end_frame;          // The frame the roll ends on. Dynamically updated based on the previous action
u8 roll_early_check_frame;  // The earliest frame the early check should be performed on. Dynamically updated based on the previous action.
u8 roll_late_check_frame;   // The latest frame the late check should be performed on. Dynamically updated based on the previous action.
u16 previous_action;        // Tracks the previous action for adjusting the frame timing window, because some actions like land dive cut the beginning of the roll short
char msg_buffer[20];        // Buffer for the message to be printed.

#if DEBUG
void RollIndicator_debug() {


}
#endif

void execute() {
    // Only run the flow if the game is not in an event, 
    // the player is in human form,
    // and the player pointer is valid
    if (dComIfGp_getEvent()->mEventStatus == false && !daAlink_getAlinkActorClass()->checkWolf() && daAlink_getAlinkActorClass()) {
#if DEBUG



#endif

        switch (daAlink_getAlinkActorClass()->mProcID) {
        case daAlink_c::PROC_FRONT_ROLL:
            // this may need to be change to a switch in the future
            // if more actions are found that cut the roll short
            if (previous_action == daAlink_c::PROC_DIVE_JUMP) {
                roll_end_frame = 15;
                roll_early_check_frame = 9;
                roll_late_check_frame = 20;
            } else {
                roll_end_frame = 20;
                roll_early_check_frame = 14;
                roll_late_check_frame = 25;
            }

#if DEBUG


#endif

            if (dComIfGp_isPauseFlag()) {
#if DEBUG

#endif
            } else {
                if (current_frame_delta == 0) {
                    current_frame_delta = 1;
                }

#if DEBUG
                RollIndicator_debug();
#endif

                if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
                    if (current_frame_delta == roll_end_frame) {
#if DEBUG

#endif
                        FIFOQueue::push("<3", Queue, 0x00CC0000);
                        current_frame_delta = 0;
                    } else if (current_frame_delta > roll_early_check_frame && current_frame_delta < roll_end_frame) {
#if DEBUG

#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "early by %d", (roll_end_frame - current_frame_delta));
                        FIFOQueue::push(msg_buffer, Queue, 0x0000FF00);
                    } else if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
#if DEBUG

#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "late by %d", (current_frame_delta - roll_end_frame));
                        FIFOQueue::push(msg_buffer, Queue, 0x99000000);
                        current_frame_delta = 0;
                    }
                }

                if (current_frame_delta != 0) {
                    current_frame_delta++;
                }
            }

            break;
        default:
#if DEBUG

#endif
            if (dComIfGp_isPauseFlag()) {
#if DEBUG

#endif
            } else {
#if DEBUG
                RollIndicator_debug();
#endif

                if (GZ_getButtonPressed(A) && !GZ_getButtonHold(A)) {
                    if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
#if DEBUG

#endif
                        snprintf(msg_buffer, sizeof(msg_buffer), "late by %d", (current_frame_delta - roll_end_frame));
                        FIFOQueue::push(msg_buffer, Queue, 0x99000000);
                        current_frame_delta = 0;
                    }
                }

                if (current_frame_delta > roll_end_frame && current_frame_delta <= roll_late_check_frame) {
                    current_frame_delta++;
                } else {
                    current_frame_delta = 0;
                }
            }

            // Update the previous action to the current action
            previous_action = daAlink_getAlinkActorClass()->mProcID;
            break;
        }
    } else {
        current_frame_delta = 0;
    }
}
void reset(){current_frame_delta=roll_end_frame=roll_early_check_frame=roll_late_check_frame=0;previous_action=0;}

}

namespace umd {

static bool lastFrameADown = false;
static bool lastFrameBDown = false;

static bool start_timer = false;
uint32_t previous_counter = 0;
uint32_t current_counter = 0;
uint32_t counter_difference = 0;
static int firstPressedButton = -1;
static bool exitCheck = false;
static char buf[20];

static const char* getPressedButtonText() {
    if (!GZ_getButtonHold(A, POST_GAME_LOOP) && GZ_getButtonPressed(A))
        return "A";
    else if (!GZ_getButtonHold(B, POST_GAME_LOOP) && GZ_getButtonPressed(B))
        return "B";

    return "";
}

void execute() {
    // Reset everything if map is not active
    if (g_meter2_info.mMapStatus != 2 && g_meter2_info.mMapStatus != 3) {
        counter_difference = 0;
        firstPressedButton = -1;
        exitCheck = false;
        start_timer = false;
    }

    if (g_meter2_info.mMenuWindowClass != NULL) {
        current_counter = g_Counter.mCounter0;

        // Activate frame counter as soon as a map opening was scheduled
        if (start_timer == false &&
            (g_meter2_info.mMapStatus == 2 || g_meter2_info.mMapStatus == 3)) {
            bool eventRunning = g_meter2_info.mMenuWindowClass->isEventCheck();

            if (!eventRunning) {
                start_timer = true;
                previous_counter = current_counter;
                counter_difference = 0;
            }
        }

        if (start_timer == true && !exitCheck) {
            counter_difference += current_counter - previous_counter;
            previous_counter = current_counter;

            // Only care up to 15f after
            if (counter_difference < 15) {
                // Ensure A or B was freshly pressed
                if ((!GZ_getButtonHold(A, POST_GAME_LOOP) && GZ_getButtonPressed(A)) ||
                    (!GZ_getButtonHold(B, POST_GAME_LOOP) && GZ_getButtonPressed(B))) {
                    if (counter_difference == 0) {
                        // A or B were pressed together (first button) = BAD
                        if ((!GZ_getButtonHold(A, POST_GAME_LOOP) && GZ_getButtonPressed(A)) &&
                            (!GZ_getButtonHold(B, POST_GAME_LOOP) && GZ_getButtonPressed(B))) {
                            FIFOQueue::push("hit both A/B as first", Queue, 0x0000FF00);
                            exitCheck = true;
                        } else {
                            // A or B is pressed here (first button) = GOOD
                            if (!GZ_getButtonHold(A, POST_GAME_LOOP) && GZ_getButtonPressed(A))
                                firstPressedButton = 0;
                            else
                                firstPressedButton = 1;

                            snprintf(buf, sizeof(buf), "got first %s", getPressedButtonText());
                            FIFOQueue::push(buf, Queue, 0x00CC0000);
                        }

                    } else if (counter_difference == 1 && firstPressedButton != -1) {
                        // B or A was pressed here as second button, success
                        FIFOQueue::push("got UMD", Queue, 0x00CC0000);
                        exitCheck = true;
                    } else {  // We went late

                        // Missed second button
                        if (firstPressedButton != -1) {
                            // Ensure this is the button that needs to be pressed
                            if ((firstPressedButton == 0 && GZ_getButtonPressed(B)) ||
                                (firstPressedButton == 1 && GZ_getButtonPressed(A))) {
                                snprintf(buf, sizeof(buf), "%df late on second %s",
                                         counter_difference - 1, getPressedButtonText());
                                FIFOQueue::push(buf, Queue, 0x99000000);
                                exitCheck = true;
                            }
                        } else {  // Missed first button
                            snprintf(buf, sizeof(buf), "%df late on first %s", counter_difference,
                                     getPressedButtonText());
                            FIFOQueue::push(buf, Queue, 0x99000000);
                            exitCheck = true;
                        }
                    }
                } else {  // Neither A or B was freshly pressed
                    // A or B wasn't pressed on frame 0, we possibly went early
                    if (counter_difference == 0) {
                        if (lastFrameADown && lastFrameBDown) {
                            FIFOQueue::push("1f early on A/B", Queue, 0x0000FF00);
                            exitCheck = true;
                        } else if (lastFrameADown) {
                            FIFOQueue::push("1f early on A", Queue, 0x0000FF00);
                            exitCheck = true;
                        } else if (lastFrameBDown) {
                            FIFOQueue::push("1f early on B", Queue, 0x0000FF00);
                            exitCheck = true;
                        }
                    }
                }
            } else {
                exitCheck = true;
            }
        }
    }

    lastFrameADown = GZ_getButtonPressed(A) && !GZ_getButtonHold(A, POST_GAME_LOOP);
    lastFrameBDown = GZ_getButtonPressed(B) && !GZ_getButtonHold(B, POST_GAME_LOOP);
}
void reset(){lastFrameADown=lastFrameBDown=start_timer=exitCheck=false;previous_counter=current_counter=counter_difference=0;firstPressedButton=-1;}

}

namespace fast_eel_regrab {
    static bool sTimerStarted = false;
    static bool sGoalHit = false;
    static uint32_t sFrameCount = 0;

#ifdef GCN_PLATFORM
#define X_HELD_CHECK !GZ_getButtonHold(GZPad::X)
#define X_DOWN_CHECK GZ_getButtonPressed(GZPad::X)
#define Y_HELD_CHECK !GZ_getButtonHold(GZPad::Y)
#define Y_DOWN_CHECK GZ_getButtonPressed(GZPad::Y)

#else
#define X_HELD_CHECK !GZ_getButtonHold(GZPad::DPAD_DOWN)
#define X_DOWN_CHECK GZ_getButtonPressed(GZPad::DPAD_DOWN)
#define Y_HELD_CHECK !GZ_getButtonHold(GZPad::B)
#define Y_DOWN_CHECK GZ_getButtonPressed(GZPad::B)
#endif

#define first_frame 9

#define PAD Pad

void execute() {


    // reset counters on load
    if (sceneLoading()) {
        sFrameCount = 0;
        sGoalHit = false;
        sTimerStarted = false;
    }

    bool claw_on_x = dComIfGs_getSelectItemIndex(0) == 9; // claw on x

    if (!sTimerStarted){
        if ((claw_on_x && X_DOWN_CHECK) || (!claw_on_x && Y_DOWN_CHECK)) {
            sTimerStarted = true;
        }
    }

    if (sTimerStarted) {
        sFrameCount++;

        if (sFrameCount < 20) {
            if (!sGoalHit && ((claw_on_x && Y_DOWN_CHECK && Y_HELD_CHECK) || (!claw_on_x && X_DOWN_CHECK && X_HELD_CHECK))) {
                char buf[20];
                if (sFrameCount < first_frame) {
                    snprintf(buf, sizeof(buf), "boots %df early", first_frame - sFrameCount);
                    FIFOQueue::push(buf, Queue, 0x0000FF00);
                } else if (sFrameCount == first_frame) {
                    FIFOQueue::push("boots 1st frame", Queue, 0x00CC0000);
                } else if (sFrameCount == first_frame + 1) {
                    FIFOQueue::push("boots 2nd frame", Queue, 0x00CC0000);
                } else if (sFrameCount == first_frame + 2) {
                    FIFOQueue::push("boots 3rd frame", Queue, 0x00CC0000);
                } else if (sFrameCount > first_frame + 2) {
                    snprintf(buf, sizeof(buf), "boots %df late", sFrameCount - first_frame - 2);
                    FIFOQueue::push(buf, Queue, 0x99000000);
                    sGoalHit = true;
                }
            }
        } else {
            sFrameCount = 0;
            sGoalHit = false;
            sTimerStarted = false;
        }
    }
}
void reset(){sTimerStarted=false;sGoalHit=false;sFrameCount=0;}

}

#undef PAD
#undef X_DOWN_CHECK
#undef X_HELD_CHECK
#undef Y_DOWN_CHECK
#undef Y_HELD_CHECK
#undef first_frame