/*
 * Read a scroll and let it happen
 *
 * @(#)scrolls.c	6.2 (Berkeley) 5/28/84
 */

#include <curses.h>
#include <ctype.h>
#include "rogue.h"

/*
 * read_scroll:
 *	Read a scroll from the pack and do the appropriate thing
 */
read_scroll()
{
	register THING *obj;
	register int y, x;
	register char ch;
	register THING *op;
	register int index, mcount;
	register bool discardit = FALSE;

	obj = get_item("Read", SCROLL);
	if (obj == NULL)
		return;
	if (obj->o_type != SCROLL)
	{
		msg("Nothing written on it");
		return;
	}
	/*
	 * Calculate the effect it has on the poor guy.
	 */
	if (obj == cur_weapon)
		cur_weapon = NULL;
	del_obj = obj;
	switch (obj->o_which)
	{
		when S_CONFUSE:
			/*
			 * Scroll of monster confusion.  Give him that power.
			 */
			player.t_flags |= CANHUH;
			addmsg("Your hands begin to glow ");
			if (on(player, ISTRIPY))
				msg(rnd_color());
			else
				msg("red");
		when S_ARMOR:
			if (cur_armor != NULL)
			{
				cur_armor->o_ac--;
				cur_armor->o_flags &= ~ISCURSED;
				msg("Your armor glows %s for a moment",
					on(player, ISTRIPY) ? rnd_color() : "silver");
			}
		when S_HOLD:
			/*
			 * Hold monster scroll.  Stop all monsters within two spaces
			 * from chasing after the hero.
			 */

			mcount = 0;
			for (x = hero.x - 2; x <= hero.x + 2; x++)
				if (x >= 0 && x < COLS)
					for (y = hero.y - 2; y <= hero.y + 2; y++)
						if (y >= 0 && y <= LINES - 1)
							if ((op = moat(y, x)) != NULL && on(*op, ISRUN))
							{
								op->t_flags &= ~ISRUN;
								op->t_flags |= ISHELD;
								mcount++;
							}
			if (mcount)
			{
				addmsg("The monster");
				if (mcount > 1)
					addmsg("s around you");
				addmsg(" freeze");
				if (mcount == 1)
					addmsg("s");
				endmsg();
				s_know[S_HOLD] = TRUE;
			}
			else
				msg("You feel a strange sense of loss");
		when S_SLEEP:
			/*
			 * Scroll which makes you fall asleep
			 */
			s_know[S_SLEEP] = TRUE;
			no_command += rnd(SLEEPTIME) + 4;
			player.t_flags &= ~ISRUN;
			msg("You fall asleep");
		when S_CREATE:
			/*
			 * Create a monster
			 * First look in a circle around him, next try his room
			 * otherwise give up
			 */
			{
				register bool appear = 0;
				coord mp;

				/*
				 * Search for an open place
				 */
				for (y = hero.y - 1; y <= hero.y + 1; y++)
					for (x = hero.x - 1; x <= hero.x + 1; x++)
					{
						/*
						 * Don't put a monster in top of the player.
						 */
						if (y == hero.y && x == hero.x)
							continue;
						/*
						 * Or anything else nasty
						 */
						if (step_ok(ch = winat(y, x)))
						{
							if (ch == SCROLL
								&& find_obj(y, x)->o_which == S_SCARE)
									continue;
							if (rnd(++appear) == 0)
							{
								mp.y = y;
								mp.x = x;
							}
						}
					}
				if (appear)
				{
					op = new_item();
					new_monster(op, randmonster(FALSE), &mp);
				}
				else
					msg("You hear a faint cry of anguish in the distance");
			}
		when S_IDENT:
			/*
			 * Identify, let the rogue figure something out
			 */
			s_know[S_IDENT] = TRUE;
			msg("This scroll is an identify scroll");
			whatis(TRUE);
		when S_MAP:
			/*
			 * Scroll of magic mapping.
			 */
			s_know[S_MAP] = TRUE;
			msg("Oh, now this scroll has a map on it");
			/*
			 * Take all the things we want to keep hidden out of the window
			 */
			for (y = 1; y < LINES - 1; y++)
				for (x = 0; x < COLS; x++)
				{
					index = INDEX(y, x);
					switch (ch = s_level[index])
					{
						case '-':
						case '|':
							if (!(s_flags[index] & F_REAL))
							{
								ch = s_level[index] = DOOR;
								s_flags[index] &= ~F_REAL;
							}
						case PASSAGE:
							s_flags[index] |= F_SEEN;
						case DOOR:
						case STAIRS:
							if ((op = moat(y, x)) != NULL)
								if (op->t_oldch == ' ')
									op->t_oldch = ch;
							break;
						default:
							ch = ' ';
					}
					if (ch != ' ')
						mvaddch(y, x, ch);
				}
		when S_GFIND:
			/*
			 * Potion of gold detection
			 */
			ch = FALSE;
			wclear(hw);
			for (op = lvl_obj; op != NULL; op = next(op))
				if (op->o_type == GOLD)
				{
					ch = TRUE;
					mvwaddch(hw, op->o_pos.y, op->o_pos.x, GOLD);
				}
			if (ch)
			{
				s_know[S_GFIND] = TRUE;
				show_win(hw,
					"You begin to feel greedy and you sense gold.--More--");
			}
			else
				msg("You feel a pull downward");
		when S_TELEP:
			/*
			 * Scroll of teleportation:
			 * Make him dissapear and reappear
			 */
			{
				register struct room *cur_room;

				cur_room = proom;
				teleport();
				if (cur_room != proom)
					s_know[S_TELEP] = TRUE;
			}
		when S_ENCH:
			if (cur_weapon == NULL || cur_weapon->o_type != WEAPON)
				msg("You feel a strange sense of loss");
			else
			{
				cur_weapon->o_flags &= ~ISCURSED;
				if (rnd(2) == 0)
					cur_weapon->o_hplus++;
				else
					cur_weapon->o_dplus++;
				msg("Your %s glows %s for a moment",
					w_names[cur_weapon->o_which],
					on(player, ISTRIPY) ? rnd_color() : "blue");
			}
		when S_SCARE:
			/*
			 * Reading it is a mistake and produces laughter at the
			 * poor rogue's boo boo.
			 */
			msg("You hear maniacal laughter in the distance");
		when S_REMOVE:
			if (cur_armor != NULL)
				cur_armor->o_flags &= ~ISCURSED;
			if (cur_weapon != NULL)
				cur_weapon->o_flags &= ~ISCURSED;
			if (cur_ring[LEFT] != NULL)
				cur_ring[LEFT]->o_flags &= ~ISCURSED;
			if (cur_ring[RIGHT] != NULL)
				cur_ring[RIGHT]->o_flags &= ~ISCURSED;
			if (on(player, ISTRIPY))
				msg("You feel in touch with the Universal Onenes");
			else
				msg("You feel as if somebody is watching over you");
		when S_AGGR:
			/*
			 * This scroll aggravates all the monsters on the current
			 * level and sets them running towards the hero
			 */
			aggravate();
			msg("You hear a high pitched humming noise");
		when S_NOP:
			if (on(player, ISTRIPY))
				msg("What an interesting piece of paper.  Like, groovy, man!");
			else
				msg("This scroll seems to be blank");
		when S_GENOCIDE:
			s_know[S_GENOCIDE] = TRUE;
			msg("You have been granted the boon of genocide");
			genocide();
		otherwise:
			msg("What a puzzling scroll!");
			return;
	}
	look(TRUE);	/* put the result of the scroll on the screen */
	status();
	/*
	 * Get rid of the thing
	 */
	inpack--;
	if (obj->o_count > 1)
		obj->o_count--;
	else
	{
		detach(pack, obj);
		discardit = TRUE;
	}

	call_it(s_know[obj->o_which], &s_guess[obj->o_which]);

	if (discardit)
		discard(obj);
	del_obj = NULL;
}
