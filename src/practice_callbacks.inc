// Generated from pinned TPGZ save_specials.cpp (GPL-3.0). See tools/adapt-practice-callbacks.py.
static void BeastGanonSpecial_setLayer();
static void SaveMngSpecial_AGEarlyBk();
static void SaveMngSpecial_Aeralfos();
static void SaveMngSpecial_AnyPlummOoB();
static void SaveMngSpecial_Argorok();
static void SaveMngSpecial_Argorok2_after();
static void SaveMngSpecial_ArgorokCSSkip();
static void SaveMngSpecial_BossFlags();
static void SaveMngSpecial_CaveOfOrdeals();
static void SaveMngSpecial_CenterCamera();
static void SaveMngSpecial_CityPoeCycle();
static void SaveMngSpecial_Dangoro();
static void SaveMngSpecial_Darkhammer();
static void SaveMngSpecial_DeathSword();
static void SaveMngSpecial_EarlyEle();
static void SaveMngSpecial_EarlyEleSpawn();
static void SaveMngSpecial_EarlyPlatform();
static void SaveMngSpecial_EldinCollection();
static void SaveMngSpecial_Escort();
static void SaveMngSpecial_EscortKeys();
static void SaveMngSpecial_FanTower();
static void SaveMngSpecial_ForestBit();
static void SaveMngSpecial_Ganondorf();
static void SaveMngSpecial_Goats1();
static void SaveMngSpecial_GorgeVoid();
static void SaveMngSpecial_HoldSol();
static void SaveMngSpecial_HorseSpawn();
static void SaveMngSpecial_Hugo();
static void SaveMngSpecial_Iza1Skip();
static void SaveMngSpecial_KB2Skip();
static void SaveMngSpecial_KB4();
static void SaveMngSpecial_KargOoB();
static void SaveMngSpecial_LakebedBKSkip();
static void SaveMngSpecial_MDHBridge();
static void SaveMngSpecial_Morpheel();
static void SaveMngSpecial_NoSQAeralfos();
static void SaveMngSpecial_OrdonRock();
static void SaveMngSpecial_Palace1();
static void SaveMngSpecial_Palace2();
static void SaveMngSpecial_PalaceBossKey();
static void SaveMngSpecial_PurpleMist();
static void SaveMngSpecial_RopeSkip();
static void SaveMngSpecial_SPRBossKey();
static void SaveMngSpecial_SetDigging();
static void SaveMngSpecial_SolBacktrackCamera();
static void SaveMngSpecial_SpawnHugo();
static void SaveMngSpecial_Stallord();
static void SaveMngSpecial_Stallord2();
static void SaveMngSpecial_Stallord2_init();
static void SaveMngSpecial_StallordCad();
static void SaveMngSpecial_StallordDisplacementClip();
static void SaveMngSpecial_ToTEarlyHP();
static void SaveMngSpecial_ToTEarlyPoe();
static void SaveMngSpecial_WaterfallSidehop();
static void SaveMngSpecial_ZD_Yellows();
static void SaveMngSpecial_ZantDangoro();
static void SaveMngSpecial_ZantFinal();
static void SaveMngSpecial_emptyLake();
static void BeastGanonSpecial_setLayer(){
setup.injectDefault_during();
    setNextStageLayer(1);
}
static void SaveMngSpecial_AGEarlyBk(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 25.0f;
}
static void SaveMngSpecial_Aeralfos(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->mEquipItem = dItemNo_HOOKSHOT_e;
}
static void SaveMngSpecial_AnyPlummOoB(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  
    setNextStageName("F_SP112");                       
    setNextStageRoom(1);
    setNextStagePoint(0);
    setNextStageLayer(4);
    cDmr_SkipInfo = 0xFF;
}
static void SaveMngSpecial_Argorok(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x10000;
}
static void SaveMngSpecial_Argorok2_after(){
SaveMngSpecial_BossFlags();
    
    
    
    
    
    

    

    
    
    
    g_dComIfG_gameInfo.info.mRestart.mRoomAngleY = -32768;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.x = 8.585349082946777;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.y = 0.0;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.z = 2004.4146728515625;
    g_dComIfG_gameInfo.info.mRestart.mStartPoint = 2;
    g_dComIfG_gameInfo.info.mZone[0].mRoomNo = 50;
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[1] = 240;
    g_dComIfG_gameInfo.info.mMemory.mBit.mSwitch[0] = 843264206;
    g_dComIfG_gameInfo.info.mMemory.mBit.mSwitch[1] = 2395341057;
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] = 7; 
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 1191182405;
    g_dComIfG_gameInfo.info.mSavedata.mSave[22].mBit.mSwitch[0] = 843264206;
    g_dComIfG_gameInfo.info.mSavedata.mSave[22].mBit.mSwitch[1] = 2395341057;
    g_dComIfG_gameInfo.info.mRestart.mRoomParam = 0;

    setup.setSavePosition(0.0f, -300.0f, 4000.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_ArgorokCSSkip(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->swordEquip(0);
}
static void SaveMngSpecial_BossFlags(){
setup.injectDefault_during();
    cDmr_SkipInfo = 0xFF;
}
static void SaveMngSpecial_CaveOfOrdeals(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;
}
static void SaveMngSpecial_CenterCamera(){
setup.setLinkInfo();

    cXyz pos = daAlink_getAlinkActorClass()->current.pos;
    u16 angle = daAlink_getAlinkActorClass()->shape_angle.y;
    f32 radians = angle * (2.0f * M_PI / 65536.0f);

    dCam_getBody()->mCenter.x = pos.x - sin(radians) * -373.0f;
    dCam_getBody()->mCenter.y = pos.y + 250.0f;
    dCam_getBody()->mCenter.z = pos.z - cos(radians) * -373.0f;

    dCam_getBody()->mEye.x = pos.x;
    dCam_getBody()->mEye.y = pos.y + 105.0f;
    dCam_getBody()->mEye.z = pos.z;
}
static void SaveMngSpecial_CityPoeCycle(){
setup.injectDefault_during();
    setup.setSaveAngle(0);
    setup.setSavePosition(-13990.0f, 3000.0f, -16200.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_Dangoro(){
g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x200000;
}
static void SaveMngSpecial_Darkhammer(){
dComIfGs_onEventBit(0x0B02);
    dComIfGs_onEventBit(0x0B04);
}
static void SaveMngSpecial_DeathSword(){
setup.injectDefault_during();
    dComIfGs_setLife(2);
}
static void SaveMngSpecial_EarlyEle(){
setup.injectDefault_during();
    dComIfGs_onTmpBit(0x0002);
}
static void SaveMngSpecial_EarlyEleSpawn(){
setup.setSaveAngle(43917);
    setup.setSavePosition(1130, -355.6, -5569);
    setup.setLinkInfo();
}
static void SaveMngSpecial_EarlyPlatform(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->swordEquip(0);
}
static void SaveMngSpecial_EldinCollection(){
SaveMngSpecial_HorseSpawn();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 42.0f;
}
static void SaveMngSpecial_Escort(){
setup.injectDefault_during();
    setNextStageRoom(0xD);
    setNextStagePoint(98);
    setNextStageLayer(2);
}
static void SaveMngSpecial_EscortKeys(){
dComIfGs_setKeyNum(2);
}
static void SaveMngSpecial_FanTower(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;
}
static void SaveMngSpecial_ForestBit(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->mNoResetFlg2 |= 1; 
    daAlink_getAlinkActorClass()->swordEquip(0); 
    setup.setSaveAngle(32768);
    setup.setSavePosition(45.0f, 1911.1345f, 20425.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_Ganondorf(){
g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 1048578;
    g_dComIfG_gameInfo.info.mZone[0].mBit.mRoomSwitch = 1024;
    daAlink_getAlinkActorClass()->swordEquip(0); 
    setup.setSavePosition(0.0f, -2000.0f, 0.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_Goats1(){
setup.injectDefault_during();
    setNextStageLayer(5);
    dComIfGs_onTmpBit(0x1480);
}
static void SaveMngSpecial_GorgeVoid(){
g_dComIfG_gameInfo.info.mRestart.mRoomAngleY = 26033;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.x = -17316.703125;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.y = -6450.0;
    g_dComIfG_gameInfo.info.mRestart.mRoomPos.z = 67532.7578125;
    setup.setLinkInfo();
    
    SaveMngSpecial_CenterCamera();
}
static void SaveMngSpecial_HoldSol(){
g_dComIfG_gameInfo.info.mRestart.mLastMode |= 0x100000;
}
static void SaveMngSpecial_HorseSpawn(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 1;
}
static void SaveMngSpecial_Hugo(){
setup.injectDefault_during();
    dComIfGs_onSwitch(47, 0);   
    dComIfGs_offSwitch(63, 0);
}
static void SaveMngSpecial_Iza1Skip(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  
    setNextStageName("F_SP112");                       
    setNextStageRoom(1);
    setNextStagePoint(0);
    setNextStageLayer(4);
}
static void SaveMngSpecial_KB2Skip(){
setup.injectDefault_during();
    setNextStageLayer(3);
}
static void SaveMngSpecial_KB4(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->swordEquip(0); 
    setup.setSaveAngle(24354);
    setup.setSavePosition(-8566.32617f, 200.0f, -4870.11084f);
    setup.setLinkInfo();

    SaveMngSpecial_CenterCamera();
}
static void SaveMngSpecial_KargOoB(){
setup.mPracticeFileOpts.inject_options_before_load = nullptr;
    setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 0xA;  
    dComIfGs_setTransformStatus(TF_STATUS_HUMAN);
}
static void SaveMngSpecial_LakebedBKSkip(){
setup.injectDefault_during();
    dComIfGs_onSwitch(2, 0);    
    dComIfGs_onSwitch(122, 0);
}
static void SaveMngSpecial_MDHBridge(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 25.0f;
}
static void SaveMngSpecial_Morpheel(){
daAlink_getAlinkActorClass()->mEquipItem = dItemNo_HOOKSHOT_e;                        
    daAlink_getAlinkActorClass()->onNoResetFlg0(daPy_py_c::FLG0_EQUIP_HVY_BOOTS);  
    setup.setSaveAngle(10754);
    setup.setSavePosition(-1193.0f, -23999.0f, -770.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_NoSQAeralfos(){
setup.injectDefault_during();
    dComIfGs_setLife(4);
}
static void SaveMngSpecial_OrdonRock(){
setup.setSaveAngle(32768);
    setup.setSavePosition(400.0f, 307.5f, -11270.2f);
    setup.setLinkInfo();

    cXyz position(400.0f, 307.8f, -11365.f);

    fopAc_ac_c* actorData = find_actor([](fopAc_ac_c& act) {
        return fopAcM_GetName(&act) == fpcNm_Obj_Stone_e && fopAcM_GetParam(&act) == 0x00FF6511;
    });

    if (actorData != NULL) {
        actorData->current.pos = position;
        actorData->shape_angle.y = 5880;
    }
}
static void SaveMngSpecial_Palace1(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mDan.mSwitch[0] = 0;
}
static void SaveMngSpecial_Palace2(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->swordEquip(0);
}
static void SaveMngSpecial_PalaceBossKey(){
setup.injectDefault_during();
    daAlink_getAlinkActorClass()->swordEquip(0);
}
static void SaveMngSpecial_PurpleMist(){
setup.injectDefault_during();
    dComIfGs_setTransformStatus(TF_STATUS_HUMAN);
}
static void SaveMngSpecial_RopeSkip(){
setNextStageLayer(2);
}
static void SaveMngSpecial_SPRBossKey(){
setup.injectDefault_during();
    setNextStageRoom(0xB);  
    setNextStagePoint(0);
}
static void SaveMngSpecial_SetDigging(){
setup.mPracticeFileOpts.inject_options_before_load = nullptr;
    setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastMode = 9;
}
static void SaveMngSpecial_SolBacktrackCamera(){
setup.setLinkInfo();

    dCam_getBody()->mCenter.x = 0.0f;
    dCam_getBody()->mCenter.y = 170.850647f;
    dCam_getBody()->mCenter.z = 533.318665f;

    dCam_getBody()->mEye.x = 0.0f;
    dCam_getBody()->mEye.y = 93.3100357f;
    dCam_getBody()->mEye.z = 299.36438f;
}
static void SaveMngSpecial_SpawnHugo(){
setup.setSaveAngle(40166);
    setup.setSavePosition(2.9385, 396.9580, -18150.087);
    setup.setLinkInfo();

    cXyz position(-289.9785, 401.5400, -18533.078);

    
    fopAc_ac_c* actorData =
        find_actor([](auto& act) { return fopAcM_GetName(&act) == fpcNm_E_RD_e; });

    if (actorData != NULL) {
        actorData->current.pos = position;
        actorData->shape_angle.y = 5880;
    }
}
static void SaveMngSpecial_Stallord(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  
    setNextStagePoint(1);
}
static void SaveMngSpecial_Stallord2(){
daB_DS_c* stallord = (daB_DS_c*)fopAcM_SearchByName(fpcNm_B_DS_e);

    
    fopAcM_create(fpcNm_B_DS_e, fopAcM_GetParam(stallord) | 2, &stallord->current.pos,
                fopAcM_GetRoomNo(stallord), nullptr, nullptr, -1);
    fopAcM_delete(stallord);  

    daObjLv4Wall_c* rwall = (daObjLv4Wall_c*)fopAcM_SearchByName(fpcNm_Obj_Lv4RailWall_e);
    daObjSwSpinner_c* spinnersw = (daObjSwSpinner_c*)fopAcM_SearchByName(fpcNm_Obj_SwSpinner_e);

    spinnersw->mRotSpeedY = 3000;  
    rwall->mRotCounter = 101;  
    rwall->mHeight = 3370.0f;
}
static void SaveMngSpecial_Stallord2_init(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  
    setNextStagePoint(1);
}
static void SaveMngSpecial_StallordCad(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  
    setNextStagePoint(1);                                          
    
    setup.setSaveAngle(59392);
    
    setup.setSavePosition(50.997982f, 1775.0f, -4024.03882f);
    setup.setLinkInfo();

    SaveMngSpecial_CenterCamera();

    
    fopAc_ac_c* actorData1 =
        find_actor([](auto& act) { return fopAcM_GetName(&act) == fpcNm_E_ZS_e && (int) act.current.pos.x == -920; });

    daE_ZS_c* joseph = (daE_ZS_c*) actorData1;
    joseph->mAction = 1; 
    joseph->mMode = 0; 

    
    
    joseph->setBck(9, 2, 3.0f, 1.0f); 
    joseph->field_0x65c = 0; 
    joseph->field_0x673 = 1; 
    joseph->mCyl.OnTgSetBit(); 
    joseph->mCyl.OnCoSetBit(); 
    fopAcM_OnStatus(joseph, 0x200000);
}
static void SaveMngSpecial_StallordDisplacementClip(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mZone[0].mBit.mSwitch[0] |= 0x300000;  
    setNextStagePoint(1);                                          
    setup.setSaveAngle(59392);
    setup.setSavePosition(-327.861115f, 1800.0f, -4990.23926f);
    setup.setLinkInfo();

    
    fopAc_ac_c* actorData1 =
        find_actor([](auto& act) { return fopAcM_GetName(&act) == fpcNm_E_ZS_e && (int) act.current.pos.x == -920; });

    daE_ZS_c* joseph = (daE_ZS_c*) actorData1;
    joseph->mAction = 1; 
    joseph->mMode = 0; 
    
    joseph->setBck(9, 2, 3.0f, 1.0f); 
    joseph->field_0x65c = 0; 
    joseph->field_0x673 = 1; 
    joseph->mCyl.OnTgSetBit(); 
    joseph->mCyl.OnCoSetBit(); 
    fopAcM_OnStatus(joseph, 0x200000); 
    cXyz corner (-301.9, 1800.0, -4966.0);
    joseph->current.pos = corner;
}
static void SaveMngSpecial_ToTEarlyHP(){
setup.injectDefault_during();
    setup.setSaveAngle(0);
    setup.setSavePosition(-6626.f, 5250.0f, -5587.f);
    setup.setLinkInfo();
    dComIfGs_onSwitch(224, 4);
}
static void SaveMngSpecial_ToTEarlyPoe(){
setup.injectDefault_during();
    setup.setSaveAngle(49299);
    setup.setSavePosition(-2462.85f, 2750.0f, -7.10f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_WaterfallSidehop(){
setup.injectDefault_during();
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 10.0f;
}
static void SaveMngSpecial_ZD_Yellows(){
dComIfGs_onItemFirstBit(dItemNo_YELLOW_RUPEE_e); 
    g_dComIfG_gameInfo.info.mRestart.mLastSpeedF = 25.0f;
}
static void SaveMngSpecial_ZantDangoro(){


    
    daB_ZANT_c* actorData = (daB_ZANT_c*)find_actor([](auto& act) { return fopAcM_GetName(&act) == fpcNm_B_ZANT_e; });

    

    if (actorData != nullptr) {
        
        actorData->mAction = 0;      
        
        
        actorData->current.pos.set(-200.0f, -1050.0f, -1250.0f);
    }
    

    daAlink_getAlinkActorClass()->swordEquip(0); 
    setup.setSaveAngle(32768);
    setup.setSavePosition(-200.0f, -800.0f, -850.0f);
    setup.setLinkInfo();
}
static void SaveMngSpecial_ZantFinal(){


    
    daB_ZANT_c* actorData = (daB_ZANT_c*)find_actor([](auto& act) { return fopAcM_GetName(&act) == fpcNm_B_ZANT_e; });

    
    if (actorData != nullptr) {
        
        actorData->mAction = 23;      
        actorData->mFightPhase = 5;   
        actorData->mMode = 0;         
    }

    daAlink_getAlinkActorClass()->swordEquip(0);
}
static void SaveMngSpecial_emptyLake(){
setup.injectDefault_during();
    setNextStageLayer(4);
    cDmr_SkipInfo = 0xFF;
}
static bool supportsSpecial(std::string_view name){
 if(name=="nullptr")return true;
 if(name=="BeastGanonSpecial_setLayer")return true;
 if(name=="SaveMngSpecial_AGEarlyBk")return true;
 if(name=="SaveMngSpecial_Aeralfos")return true;
 if(name=="SaveMngSpecial_AnyPlummOoB")return true;
 if(name=="SaveMngSpecial_Argorok")return true;
 if(name=="SaveMngSpecial_Argorok2_after")return true;
 if(name=="SaveMngSpecial_ArgorokCSSkip")return true;
 if(name=="SaveMngSpecial_BossFlags")return true;
 if(name=="SaveMngSpecial_CaveOfOrdeals")return true;
 if(name=="SaveMngSpecial_CenterCamera")return true;
 if(name=="SaveMngSpecial_CityPoeCycle")return true;
 if(name=="SaveMngSpecial_Dangoro")return true;
 if(name=="SaveMngSpecial_Darkhammer")return true;
 if(name=="SaveMngSpecial_DeathSword")return true;
 if(name=="SaveMngSpecial_EarlyEle")return true;
 if(name=="SaveMngSpecial_EarlyEleSpawn")return true;
 if(name=="SaveMngSpecial_EarlyPlatform")return true;
 if(name=="SaveMngSpecial_EldinCollection")return true;
 if(name=="SaveMngSpecial_Escort")return true;
 if(name=="SaveMngSpecial_EscortKeys")return true;
 if(name=="SaveMngSpecial_FanTower")return true;
 if(name=="SaveMngSpecial_ForestBit")return true;
 if(name=="SaveMngSpecial_Ganondorf")return true;
 if(name=="SaveMngSpecial_Goats1")return true;
 if(name=="SaveMngSpecial_GorgeVoid")return true;
 if(name=="SaveMngSpecial_HoldSol")return true;
 if(name=="SaveMngSpecial_HorseSpawn")return true;
 if(name=="SaveMngSpecial_Hugo")return true;
 if(name=="SaveMngSpecial_Iza1Skip")return true;
 if(name=="SaveMngSpecial_KB2Skip")return true;
 if(name=="SaveMngSpecial_KB4")return true;
 if(name=="SaveMngSpecial_KargOoB")return true;
 if(name=="SaveMngSpecial_LakebedBKSkip")return true;
 if(name=="SaveMngSpecial_MDHBridge")return true;
 if(name=="SaveMngSpecial_Morpheel")return true;
 if(name=="SaveMngSpecial_NoSQAeralfos")return true;
 if(name=="SaveMngSpecial_OrdonRock")return true;
 if(name=="SaveMngSpecial_Palace1")return true;
 if(name=="SaveMngSpecial_Palace2")return true;
 if(name=="SaveMngSpecial_PalaceBossKey")return true;
 if(name=="SaveMngSpecial_PurpleMist")return true;
 if(name=="SaveMngSpecial_RopeSkip")return true;
 if(name=="SaveMngSpecial_SPRBossKey")return true;
 if(name=="SaveMngSpecial_SetDigging")return true;
 if(name=="SaveMngSpecial_SolBacktrackCamera")return true;
 if(name=="SaveMngSpecial_SpawnHugo")return true;
 if(name=="SaveMngSpecial_Stallord")return true;
 if(name=="SaveMngSpecial_Stallord2")return true;
 if(name=="SaveMngSpecial_Stallord2_init")return true;
 if(name=="SaveMngSpecial_StallordCad")return true;
 if(name=="SaveMngSpecial_StallordDisplacementClip")return true;
 if(name=="SaveMngSpecial_ToTEarlyHP")return true;
 if(name=="SaveMngSpecial_ToTEarlyPoe")return true;
 if(name=="SaveMngSpecial_WaterfallSidehop")return true;
 if(name=="SaveMngSpecial_ZD_Yellows")return true;
 if(name=="SaveMngSpecial_ZantDangoro")return true;
 if(name=="SaveMngSpecial_ZantFinal")return true;
 if(name=="SaveMngSpecial_emptyLake")return true;
 return false;
}
static void dispatchSpecial(std::string_view name){
 if(name=="BeastGanonSpecial_setLayer"){BeastGanonSpecial_setLayer();return;}
 if(name=="SaveMngSpecial_AGEarlyBk"){SaveMngSpecial_AGEarlyBk();return;}
 if(name=="SaveMngSpecial_Aeralfos"){SaveMngSpecial_Aeralfos();return;}
 if(name=="SaveMngSpecial_AnyPlummOoB"){SaveMngSpecial_AnyPlummOoB();return;}
 if(name=="SaveMngSpecial_Argorok"){SaveMngSpecial_Argorok();return;}
 if(name=="SaveMngSpecial_Argorok2_after"){SaveMngSpecial_Argorok2_after();return;}
 if(name=="SaveMngSpecial_ArgorokCSSkip"){SaveMngSpecial_ArgorokCSSkip();return;}
 if(name=="SaveMngSpecial_BossFlags"){SaveMngSpecial_BossFlags();return;}
 if(name=="SaveMngSpecial_CaveOfOrdeals"){SaveMngSpecial_CaveOfOrdeals();return;}
 if(name=="SaveMngSpecial_CenterCamera"){SaveMngSpecial_CenterCamera();return;}
 if(name=="SaveMngSpecial_CityPoeCycle"){SaveMngSpecial_CityPoeCycle();return;}
 if(name=="SaveMngSpecial_Dangoro"){SaveMngSpecial_Dangoro();return;}
 if(name=="SaveMngSpecial_Darkhammer"){SaveMngSpecial_Darkhammer();return;}
 if(name=="SaveMngSpecial_DeathSword"){SaveMngSpecial_DeathSword();return;}
 if(name=="SaveMngSpecial_EarlyEle"){SaveMngSpecial_EarlyEle();return;}
 if(name=="SaveMngSpecial_EarlyEleSpawn"){SaveMngSpecial_EarlyEleSpawn();return;}
 if(name=="SaveMngSpecial_EarlyPlatform"){SaveMngSpecial_EarlyPlatform();return;}
 if(name=="SaveMngSpecial_EldinCollection"){SaveMngSpecial_EldinCollection();return;}
 if(name=="SaveMngSpecial_Escort"){SaveMngSpecial_Escort();return;}
 if(name=="SaveMngSpecial_EscortKeys"){SaveMngSpecial_EscortKeys();return;}
 if(name=="SaveMngSpecial_FanTower"){SaveMngSpecial_FanTower();return;}
 if(name=="SaveMngSpecial_ForestBit"){SaveMngSpecial_ForestBit();return;}
 if(name=="SaveMngSpecial_Ganondorf"){SaveMngSpecial_Ganondorf();return;}
 if(name=="SaveMngSpecial_Goats1"){SaveMngSpecial_Goats1();return;}
 if(name=="SaveMngSpecial_GorgeVoid"){SaveMngSpecial_GorgeVoid();return;}
 if(name=="SaveMngSpecial_HoldSol"){SaveMngSpecial_HoldSol();return;}
 if(name=="SaveMngSpecial_HorseSpawn"){SaveMngSpecial_HorseSpawn();return;}
 if(name=="SaveMngSpecial_Hugo"){SaveMngSpecial_Hugo();return;}
 if(name=="SaveMngSpecial_Iza1Skip"){SaveMngSpecial_Iza1Skip();return;}
 if(name=="SaveMngSpecial_KB2Skip"){SaveMngSpecial_KB2Skip();return;}
 if(name=="SaveMngSpecial_KB4"){SaveMngSpecial_KB4();return;}
 if(name=="SaveMngSpecial_KargOoB"){SaveMngSpecial_KargOoB();return;}
 if(name=="SaveMngSpecial_LakebedBKSkip"){SaveMngSpecial_LakebedBKSkip();return;}
 if(name=="SaveMngSpecial_MDHBridge"){SaveMngSpecial_MDHBridge();return;}
 if(name=="SaveMngSpecial_Morpheel"){SaveMngSpecial_Morpheel();return;}
 if(name=="SaveMngSpecial_NoSQAeralfos"){SaveMngSpecial_NoSQAeralfos();return;}
 if(name=="SaveMngSpecial_OrdonRock"){SaveMngSpecial_OrdonRock();return;}
 if(name=="SaveMngSpecial_Palace1"){SaveMngSpecial_Palace1();return;}
 if(name=="SaveMngSpecial_Palace2"){SaveMngSpecial_Palace2();return;}
 if(name=="SaveMngSpecial_PalaceBossKey"){SaveMngSpecial_PalaceBossKey();return;}
 if(name=="SaveMngSpecial_PurpleMist"){SaveMngSpecial_PurpleMist();return;}
 if(name=="SaveMngSpecial_RopeSkip"){SaveMngSpecial_RopeSkip();return;}
 if(name=="SaveMngSpecial_SPRBossKey"){SaveMngSpecial_SPRBossKey();return;}
 if(name=="SaveMngSpecial_SetDigging"){SaveMngSpecial_SetDigging();return;}
 if(name=="SaveMngSpecial_SolBacktrackCamera"){SaveMngSpecial_SolBacktrackCamera();return;}
 if(name=="SaveMngSpecial_SpawnHugo"){SaveMngSpecial_SpawnHugo();return;}
 if(name=="SaveMngSpecial_Stallord"){SaveMngSpecial_Stallord();return;}
 if(name=="SaveMngSpecial_Stallord2"){SaveMngSpecial_Stallord2();return;}
 if(name=="SaveMngSpecial_Stallord2_init"){SaveMngSpecial_Stallord2_init();return;}
 if(name=="SaveMngSpecial_StallordCad"){SaveMngSpecial_StallordCad();return;}
 if(name=="SaveMngSpecial_StallordDisplacementClip"){SaveMngSpecial_StallordDisplacementClip();return;}
 if(name=="SaveMngSpecial_ToTEarlyHP"){SaveMngSpecial_ToTEarlyHP();return;}
 if(name=="SaveMngSpecial_ToTEarlyPoe"){SaveMngSpecial_ToTEarlyPoe();return;}
 if(name=="SaveMngSpecial_WaterfallSidehop"){SaveMngSpecial_WaterfallSidehop();return;}
 if(name=="SaveMngSpecial_ZD_Yellows"){SaveMngSpecial_ZD_Yellows();return;}
 if(name=="SaveMngSpecial_ZantDangoro"){SaveMngSpecial_ZantDangoro();return;}
 if(name=="SaveMngSpecial_ZantFinal"){SaveMngSpecial_ZantFinal();return;}
 if(name=="SaveMngSpecial_emptyLake"){SaveMngSpecial_emptyLake();return;}
}
