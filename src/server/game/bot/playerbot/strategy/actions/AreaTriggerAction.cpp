﻿#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "AreaTriggerAction.h"
#include "../../PlayerbotAIConfig.h"
#include "Opcodes.h"

#pragma execution_character_set("utf-8")

using namespace BotAI;

bool ReachAreaTriggerAction::Execute(Event event)
{
    uint32 triggerId;
    WorldPacket p(event.getPacket());
    p.rpos(0);
    p >> triggerId;

	AreaTrigger const* atEntry = sObjectMgr->GetAreaTrigger(triggerId);
    if(!atEntry)
        return false;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
    {
        WorldPacket p1(CMSG_AREATRIGGER);
        p1 << triggerId;
        p1.rpos(0);
        bot->GetSession()->HandleAreaTriggerOpcode(p1);

        return true;
    }

    if (bot->GetMapId() != atEntry->map || bot->GetDistance(atEntry->x, atEntry->y, atEntry->z) > sPlayerbotAIConfig.sightDistance)
    {
        ai->TellMaster("挂机中有事在群里找我");
        return true;
    }

    MotionMaster &mm = *bot->GetMotionMaster();
    mm.Clear();
	mm.MovePoint(atEntry->map, atEntry->x, atEntry->y, atEntry->z);
    float distance = bot->GetDistance(atEntry->x, atEntry->y, atEntry->z);
    float delay = 1000.0f * distance / bot->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;
    bot->Say("等我哈", LANG_UNIVERSAL);
    ai->SetNextCheckDelay(delay);
    context->GetValue<LastMovement&>("last movement")->Get().lastAreaTrigger = triggerId;

    return true;
}



bool AreaTriggerAction::Execute(Event event)
{
    LastMovement& movement = context->GetValue<LastMovement&>("last movement")->Get();

    uint32 triggerId = movement.lastAreaTrigger;
    movement.lastAreaTrigger = 0;

	AreaTrigger const* atEntry = sObjectMgr->GetAreaTrigger(triggerId);
    if(!atEntry)
        return false;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
        return true;

    WorldPacket p(CMSG_AREATRIGGER);
    p << triggerId;
    p.rpos(0);
    bot->GetSession()->HandleAreaTriggerOpcode(p);

    ai->TellMaster("挂机。。。。。。。。。。");
    return true;
}
