/*	SCCS Id: @(#)shknam.c	3.0	88/04/13
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* shknam.c -- initialize a shop */

#include "hack.h"
#include "eshk.h"

static const char *shkliquors[] = {
    /* Ukraine */
    "Njezjin", "Tsjernigof", "Gomel", "Ossipewsk", "Gorlowka",
    /* N. Russia */
    "Konosja", "Weliki Oestjoeg", "Syktywkar", "Sablja",
    "Narodnaja", "Kyzyl",
    /* Silezie */
    "Walbrzych", "Swidnica", "Klodzko", "Raciborz", "Gliwice",
    "Brzeg", "Krnov", "Hradec Kralove",
    /* Schweiz */
    "Leuk", "Brig", "Brienz", "Thun", "Sarnen", "Burglen", "Elm",
    "Flims", "Vals", "Schuls", "Zum Loch",
    ""
};

static const char *shkbooks[] = {
    /* Eire */
    "Skibbereen", "Kanturk", "Rath Luirc", "Ennistymon", "Lahinch",
    "Kinnegad", "Lugnaquillia", "Enniscorthy", "Gweebarra",
    "Kittamagh", "Nenagh", "Sneem", "Ballingeary", "Kilgarvan",
    "Cahersiveen", "Glenbeigh", "Kilmihil", "Kiltamagh",
    "Droichead Atha", "Inniscrone", "Clonegal", "Lisnaskea",
    "Culdaff", "Dunfanaghy", "Inishbofin", "Kesh",
    ""
};

static const char *shkarmors[] = {
    /* Turquie */
    "Demirci", "Kalecik", "Boyabai", "Yildizeli", "Gaziantep",
    "Siirt", "Akhalataki", "Tirebolu", "Aksaray", "Ermenak",
    "Iskenderun", "Kadirli", "Siverek", "Pervari", "Malasgirt",
    "Bayburt", "Ayancik", "Zonguldak", "Balya", "Tefenni",
    "Artvin", "Kars", "Makharadze", "Malazgirt", "Midyat",
    "Birecik", "Kirikkale", "Alaca", "Polatli", "Nallihan",
    ""
};

static const char *shkwands[] = {
    /* Wales */
    "Yr Wyddgrug", "Trallwng", "Mallwyd", "Pontarfynach",
    "Rhaeader", "Llandrindod", "Llanfair-ym-muallt",
    "Y-Fenni", "Measteg", "Rhydaman", "Beddgelert",
    "Curig", "Llanrwst", "Llanerchymedd", "Caergybi",
    /* Scotland */
    "Nairn", "Turriff", "Inverurie", "Braemar", "Lochnagar",
    "Kerloch", "Beinn a Ghlo", "Drumnadrochit", "Morven",
    "Uist", "Storr", "Sgurr na Ciche", "Cannich", "Gairloch",
    "Kyleakin", "Dunvegan",
    ""
};

static const char *shkrings[] = {
    /* Hollandse familienamen */
    "Feyfer", "Flugi", "Gheel", "Havic", "Haynin", "Hoboken",
    "Imbyze", "Juyn", "Kinsky", "Massis", "Matray", "Moy",
    "Olycan", "Sadelin", "Svaving", "Tapper", "Terwen", "Wirix",
    "Ypey",
    /* Skandinaviske navne */
    "Rastegaisa", "Varjag Njarga", "Kautekeino", "Abisko",
    "Enontekis", "Rovaniemi", "Avasaksa", "Haparanda",
    "Lulea", "Gellivare", "Oeloe", "Kajaani", "Fauske",
    ""
};

static const char *shkfoods[] = {
    /* Indonesia */
    "Djasinga", "Tjibarusa", "Tjiwidej", "Pengalengan",
    "Bandjar", "Parbalingga", "Bojolali", "Sarangan",
    "Ngebel", "Djombang", "Ardjawinangun", "Berbek",
    "Papar", "Baliga", "Tjisolok", "Siboga", "Banjoewangi",
    "Trenggalek", "Karangkobar", "Njalindoeng", "Pasawahan",
    "Pameunpeuk", "Patjitan", "Kediri", "Pemboeang", "Tringanoe",
    "Makin", "Tipor", "Semai", "Berhala", "Tegal", "Samoe",
    ""
};

static const char *shkweapons[] = {
    /* Perigord */
    "Voulgezac", "Rouffiac", "Lerignac", "Touverac", "Guizengeard",
    "Melac", "Neuvicq", "Vanzac", "Picq", "Urignac", "Corignac",
    "Fleac", "Lonzac", "Vergt", "Queyssac", "Liorac", "Echourgnac",
    "Cazelon", "Eypau", "Carignan", "Monbazillac", "Jonzac",
    "Pons", "Jumilhac", "Fenouilledes", "Laguiolet", "Saujon",
    "Eymoutiers", "Eygurande", "Eauze", "Labouheyre",
    ""
};

static const char *shktools[] = {
    /* Spmi */
    "Ymla", "Eed-morra", "Cubask", "Nieb", "Bnowr Falr", "Telloc Cyaj",
    "Sperc", "Noskcirdneh", "Yawolloh", "Hyeghu", "Niskal", "Trahnil",
    "Htargcm", "Enrobwem", "Kachzi Rellim", "Regien", "Donmyar",
    "Yelpur", "Nosnehpets", "Stewe", "Renrut", "Zlaw", "Nosalnef",
    "Rewuorb", "Rellenk",
    ""
};

static const char *shkgeneral[] = {
    /* Suriname */
    "Hebiwerie", "Possogroenoe", "Asidonhopo", "Manlobbi",
    "Adjama", "Pakka Pakka", "Kabalebo", "Wonotobo",
    "Akalapi", "Sipaliwini",
    /* Greenland */
    "Annootok", "Upernavik", "Angmagssalik",
    /* N. Canada */
    "Aklavik", "Inuvik", "Tuktoyaktuk",
    "Chicoutimi", "Ouiatchouane", "Chibougamau",
    "Matagami", "Kipawa", "Kinojevis",
    "Abitibi", "Maganasipi",
    /* Iceland */
    "Akureyri", "Kopasker", "Budereyri", "Akranes", "Bordeyri",
    "Holmavik",
    ""
};

/*
 * To add new shop types, all that is necessary is to edit the shtypes[] array.
 * See mkroom.h for the structure definition. Typically, you'll have to lower
 * some or all of the probability fields in old entries to free up some
 * percentage for the new type.
 *
 * The placement type field is not yet used but will be in the near future.
 *
 * The iprobs array in each entry defines the probabilities for various kinds
 * of artifacts to be present in the given shop type. You can associate with
 * each percentage either a generic artifact type (represented by one of the
 * *_SYM macros) or a specific artifact (represented by an onames.h define).
 * In the latter case, prepend it with a unary minus so the code can know
 * (by testing the sign) whether to use mkobj() or mksobj().
 */
const struct shclass shtypes[] = {
	{"general store", RANDOM_SYM,
#ifdef SPELLS
	    44,
#else
	    47,
#endif
	    D_SHOP, {{100, RANDOM_SYM}, {0, 0}, {0, 0}}, shkgeneral},
	{"used armor dealership", ARMOR_SYM, 14,
	    D_SHOP, {{90, ARMOR_SYM}, {10, WEAPON_SYM}, {0, 0}}, shkarmors},
	{"second hand bookstore", SCROLL_SYM, 10, D_SHOP,
#ifdef SPELLS
	    {{90, SCROLL_SYM}, {10, SPBOOK_SYM}, {0, 0}},
#else
	    {{100, SCROLL_SYM}, {0, 0}, {0, 0}},
#endif
	    shkbooks},
	{"liquor emporium", POTION_SYM, 10, D_SHOP,
	    {{100, POTION_SYM}, {0, 0}, {0, 0}}, shkliquors},
	{"antique weapons outlet", WEAPON_SYM, 5, D_SHOP,
	    {{90, WEAPON_SYM}, {10, ARMOR_SYM}, {0, 0}}, shkweapons},
	{"delicatessen", FOOD_SYM, 5, D_SHOP,
	    {{95, FOOD_SYM}, {5, POTION_SYM}, {0, 0}}, shkfoods},
	{"jewelers", RING_SYM, 3, D_SHOP,
	    {{85, RING_SYM}, {10, GEM_SYM}, {5, AMULET_SYM}, {0, 0}}, shkrings},
	{"quality apparel and accessories", WAND_SYM, 3, D_SHOP,
	    {{90, WAND_SYM}, {5, -LEATHER_GLOVES}, {5, -ELVEN_CLOAK}, {0, 0}},
	     shkwands},
	{"hardware store", TOOL_SYM, 3, D_SHOP,
	    {{100, TOOL_SYM}, {0, 0}, {0, 0}}, shktools},
	/* Actually shktools is ignored; the code specifically chooses a
	 * random implementor name (the only shop type with random shopkeepers)
	 */
#ifdef SPELLS
	{"rare books", SPBOOK_SYM, 3, D_SHOP,
	    {{90, SPBOOK_SYM}, {10, SCROLL_SYM}, {0, 0}}, shkbooks},
#endif
	{NULL, 0, 0, 0, {{0, 0}, {0, 0}, {0, 0}}, (const char **)0}
};

static void
mkshobj_at(shp, sx, sy)
/* make an object of the appropriate type for a shop square */
struct shclass *shp;
int sx, sy;
{
	register struct monst *mtmp;
	int atype;

	if (rn2(100) < dlevel && levl[sx][sy].mmask == 0 &&
				(mtmp=makemon(mkclass(S_MIMIC),sx,sy))) {
		mtmp->mimic = 1;
		/* note: makemon will set the mimic symbol to a shop item */
		if (rn2(10) >= dlevel) mtmp->mappearance = S_MIMIC_DEF;
	} else if ((atype = get_shop_item(shp - shtypes)) < 0)
		(void) mksobj_at(-atype, sx, sy);
	else (void) mkobj_at(atype, sx, sy);
}

/* extract a shopkeeper name for the given shop type */
static void
findname(nampt, nlp)
char *nampt;
const char *nlp[];
{
    register int i;

    for(i = 0; i < dlevel; i++)
	if (strlen(nlp[i]) == 0) {
	    /* Not enough names, try general name */
	    if (nlp != shkgeneral)
		findname(nampt, shkgeneral);
	    else
		Strcpy(nampt, "Dirk");
	    return;
	}
    (void) strncpy(nampt, nlp[i-1], PL_NSIZ);
    nampt[PL_NSIZ-1] = 0;
}

static int
shkinit(shp, sroom)	/* create a new shopkeeper in the given room */
struct shclass	*shp;
struct mkroom	*sroom;
{
	register int sh, sx, sy;
	struct monst *shk;

	/* place the shopkeeper in the given room */
	sh = sroom->fdoor;
	sx = doors[sh].x;
	sy = doors[sh].y;

	/* check that the shopkeeper placement is sane */
	if(sx == sroom->lx-1) sx++; else
	    if(sx == sroom->hx+1) sx--; else
		if(sy == sroom->ly-1) sy++; else
		    if(sy == sroom->hy+1) sy--; else {
#ifdef DEBUG
# ifdef WIZARD
		    /* Said to happen sometimes, but I have never seen it. */
		    /* Supposedly fixed by fdoor change in mklev.c */
			if(wizard) {
			    register int j = sroom->doorct;

			    pline("Where is shopdoor?");
			    pline("Room at (%d,%d),(%d,%d).",
				  sroom->lx, sroom->ly, sroom->hx, sroom->hy);
			    pline("doormax=%d doorct=%d fdoor=%d",
			    doorindex, sroom->doorct, sh);
			    while(j--) {
				pline("door [%d,%d]", doors[sh].x, doors[sh].y);
				sh++;
			    }
			    more();
			}
# endif
#endif
			return(-1);
		    }

	/* now initialize the shopkeeper monster structure */
	if(!(shk = makemon(&mons[PM_SHOPKEEPER], sx, sy))) return(-1);
	shk->isshk = shk->mpeaceful = 1;
	shk->msleep = 0;
	shk->mtrapseen = ~0;	/* we know all the traps already */
	ESHK(shk)->shoproom = sroom - rooms;
	ESHK(shk)->shoplevel = dlevel;
	ESHK(shk)->shd = doors[sh];
	ESHK(shk)->shk.x = sx;
	ESHK(shk)->shk.y = sy;
	/* makemon() has already zeroed out all the extra space
	ESHK(shk)->robbed = 0;
	ESHK(shk)->credit = 0;
	ESHK(shk)->debit = 0;
	ESHK(shk)->visitct = 0;
	ESHK(shk)->following = 0;
	ESHK(shk)->billct = 0;
	*/
	shk->mgold = 1000 + 30*rnd(100);	/* initial capital */
	if (shp->shknms == shktools) {
		static int who;
		who = rn2(sizeof(shktools)/sizeof(char *));
		if (who==21) ESHK(shk)->ismale = FALSE;
		else ESHK(shk)->ismale = TRUE;
		(void) strncpy(ESHK(shk)->shknam, shp->shknms[who], PL_NSIZ);
		ESHK(shk)->shknam[PL_NSIZ-1] = 0;
	} else {
		ESHK(shk)->ismale = dlevel%2;
		findname(ESHK(shk)->shknam, shp->shknms);
	}

	return(sh);
}

/* stock a newly-created room with artifacts */
#ifdef __STDC__
void
stock_room(const struct shclass *shp,struct mkroom *sroom)
#else
void
stock_room(shp, sroom)
const struct shclass	*shp;
register struct mkroom *sroom;
#endif
{
    /*
     * Someday soon we'll dispatch on the dist field of shclass to do
     * different placements in this routine. Currently it only supports
     * shop-style placement (all squares except a row nearest the first
     * door get artifacts).
     */
    register int sx, sy, sh;

    /* first, try to place a shopkeeper in the room */
    if ((sh = shkinit(shp, sroom)) < 0)
	return;

    /* make sure no doorways without doors in shops */
    for(sx = sroom->lx - 1; sx <= sroom->hx + 1; sx++)
	for(sy = sroom->ly - 1; sy <= sroom->hy + 1; sy++) {
	    if(IS_DOOR(levl[sx][sy].typ))
		if (levl[sx][sy].doormask == D_NODOOR)
		    levl[sx][sy].doormask = D_ISOPEN;
    }

    for(sx = sroom->lx; sx <= sroom->hx; sx++)
	for(sy = sroom->ly; sy <= sroom->hy; sy++) {
	    if((sx == sroom->lx && doors[sh].x == sx-1) ||
	       (sx == sroom->hx && doors[sh].x == sx+1) ||
	       (sy == sroom->ly && doors[sh].y == sy-1) ||
	       (sy == sroom->hy && doors[sh].y == sy+1)) continue;
	    mkshobj_at(shp, sx, sy);
	}

    /*
     * Special monster placements (if any) should go here: that way,
     * monsters will sit on top of artifacts and not the other way around.
     */
}

int
saleable(nshop, obj)			/* does "shop" stock this item type */
register int nshop;
register struct	obj *obj;
{
	int i;

	if(shtypes[nshop].symb == RANDOM_SYM) return(1);
	else {
	    for(i = 0; shtypes[nshop].iprobs[i].iprob; i++)
		if(shtypes[nshop].iprobs[i].itype < 0) {
		   if(shtypes[nshop].iprobs[i].itype == - obj->otyp) return(1);
		}
		else if(shtypes[nshop].iprobs[i].itype == obj->olet) return(1);
	}
	return(0);
}

/* positive value: letter; negative value: specific item type */
int
get_shop_item(type)
int type;
{
	const struct shclass *shp = shtypes+type;
	register int i,j;

	/* select an appropriate artifact type at random */
	for(j = rnd(100), i = 0; j -= shp->iprobs[i].iprob; i++)
		if (j < 0) break;

	return shp->iprobs[i].itype;
}
