/*
 * Adapted from plvlex.h in orafce 3.17.
 * See COPYRIGHT.orafce for the upstream copyright and license.
 */

typedef struct
{
	int		typenode;
	char   *str;
	int		keycode;
	int		lloc;
	char   *sep;
	char   *modificator;
	char   *classname;
} orafce_lexnode;

#define PLVLEX_D
