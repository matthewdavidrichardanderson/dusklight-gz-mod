// Exact upstream native setters; used in the standalone load-order regression.
void dStage_nextStage_c::set(const char* i_stage, s8 i_roomId, s16 i_point, s8 i_layer, s8 i_wipe,
                             u8 i_speed) {
    if (!enabled) {
        enabled = true;
        wipe = i_wipe;
        wipe_speed = i_speed;
#if DEBUG
        if (g_kankyoHIO.navy.fade_test_speed) {
            wipe_speed = g_kankyoHIO.navy.fade_test_speed;
        }
#endif
        dStage_startStage_c::set(i_stage, i_roomId, i_point, i_layer);
        if (!strcmp(i_stage, "OPENING")) {
            mDoRst::onReset();
        }
    }
}
void dStage_startStage_c::set(const char* i_Name, s8 i_RoomNo, s16 i_Point, s8 i_Layer) {
#if TARGET_PC
    // UB fix.
    if (mName != i_Name) {
        SafeStringCopy(mName, i_Name);
    }
#else
    strcpy(mName, i_Name);
#endif
    mRoomNo = i_RoomNo;
    mPoint = i_Point;
    mLayer = i_Layer;
}
