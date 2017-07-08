#include "TimedEvent.h"
#include "Configuration.h"
#include "Game.h"
#include "GameGraphics.h"

extern Configuration* config;
extern Game* game;
extern GameGraphics* gamegraphics;

TimedEvent::TimedEvent(int interval)
	: interval(interval),
	  trigger_time(interval)
{}

/**------------------------------------------------------------
  Checkes whether current time exceeded time to trigger
  execution. If did, sets up next trigger time
  -----------------------------------------------------------*/
void TimedEvent::on_time(int current_time)
{
	if (current_time > trigger_time)
	{
		trigger_time += interval;
		// triggered_time - current_time + interval
		execute();
	}
}

/*-------------------------------------------------------------*/

TimedGameMove::TimedGameMove()
	: TimedEvent(config->TIME_UNIT)
{}

void TimedGameMove::execute()
{
	game->make_move();
	
}

/*-------------------------------------------------------------*/

TimedStarRevision::TimedStarRevision()
	: TimedEvent(config->STAR_REVISE_TIME)
{}

void TimedStarRevision::execute()
{
	if (game->revise_stars())
	{
		gamegraphics->reset_forestar_layer();
	}
}

/*-------------------------------------------------------------*/

TimedBotAction::TimedBotAction()
	: TimedEvent(config->BOT_ACTION)
{}

void TimedBotAction::execute()
{
	game->call_bots_action();
}