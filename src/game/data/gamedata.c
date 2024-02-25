#include "gamedata.h"

#define _(X) (X)

GameData gameData = {
    .course = {
        {
            .courseTitle = _(" 1 GRASS LAND PLAINS"),
            .missionDescription = {
                    _("REACH TO THE GOAL")     , _("BOX ON THE FLOOR") , _("FAR ABOVE THE CLOUDS"),
                    _("TOP PLUMBER")          , _("BEHIND THE SCENES")        , _("GOTTA GO FAST")
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