#pragma once

enum PMSPolygonType : unsigned char {
    ptNORMAL = 0,
    ptONLY_BULLETS_COLLIDE,
    ptONLY_PLAYERS_COLLIDE,
    ptNO_COLLIDE,
    ptICE,
    ptDEADLY,
    ptBLOODY_DEADLY,
    ptHURTS,
    ptREGENERATES,
    ptLAVA,
    ptALPHABULLETS,
    ptALPHAPLAYERS,
    ptBRAVOBULLETS,
    ptBRAVOPLAYERS,
    ptCHARLIEBULLETS,
    ptCHARLIEPLAYERS,
    ptDELTABULLETS,
    ptDELTAPLAYERS,
    ptBOUNCY,
    ptEXPLOSIVE,
    ptHURTFLAGGERS,
    ptFLAGGERCOLLIDES,
    ptNONFLAGGERCOLLIDES,
    ptFLAGCOLLIDES
};

enum PMSSceneryLevel : unsigned char { lBEHIND_ALL = 0, lBEHIND_MAP, lBEHIND_NONE };

enum PMSSpawnPointType : unsigned int {
    sptGENERAL = 0,
    sptALPHA,
    sptBRAVO,
    sptCHARLIE,
    sptDELTA,
    sptALPHA_FLAG,
    sptBRAVO_FLAG,
    sptGRENADES,
    sptMEDKITS,
    sptCLUSTERS,
    sptVEST,
    sptFLAMER,
    sptBERSERKER,
    sptPREDATOR,
    sptYELLOW_FLAG,
    sptRAMBO_BOW,
    sptSTAT_GUN
};

enum PMSSpecialAction : unsigned char {
    saNONE = 0,
    saSTOP_AND_CAMP,
    saWAIT_1_SECOND,
    saWAIT_5_SECONDS,
    saWAIT_10_SECONDS,
    saWAIT_15_SECONDS,
    saWAIT_20_SECONDS
};

enum PMSStepType : unsigned char { stHARD_GROUND = 0, stSOFT_GROUND, stNONE };

enum PMSWeatherType : unsigned char { wtNONE = 0, wtRAIN, wtSANDSTORM, wtSNOW };
