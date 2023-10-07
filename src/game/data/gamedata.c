#include "gamedata.h"

#define _(X) (X)

GameData gameData = {
    .course = {
        {
            .courseTitle = _(" 1 BOB-OMB BATTLEFIELD"),
            .missionDescription = {
                    _("BIG BOB-OMB ON THE SUMMIT")     , _("FOOTRACE WITH KOOPA THE QUICK") , _("SHOOT TO THE ISLAND IN THE SKY"),
                    _("FIND THE 8 RED COINS")          , _("MARIO WINGS TO THE SKY")        , _("BEHIND CHAIN CHOMP'S GATE")
            }
        },
        {
            .courseTitle = " 2 WHOMP'S FORTRESS",
            .missionDescription = {
                    _("CHIP OFF WHOMP'S BLOCK")        , _("TO THE TOP OF THE FORTRESS")    , _("SHOOT INTO THE WILD BLUE"),
                    _("RED COINS ON THE FLOATING ISLE"), _("FALL ONTO THE CAGED ISLAND")    , _("BLAST AWAY THE WALL")
            }
        }
    }
};