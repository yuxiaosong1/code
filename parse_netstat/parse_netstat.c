#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

#define MAX_ITEM	256
#define MAX_STAT	3
#define MAX_NAME	64
#define MAX_CHAR_PER_LINE	2500

typedef struct {
	char title[MAX_NAME];
	size_t val;
} StatsItem;

typedef struct {
	char name[MAX_NAME];
	StatsItem item[MAX_ITEM];
	size_t item_cnt;
} Stats;

Stats g_stats[MAX_STAT];

/* parse line data from stdin */
int parse_line(char *line_buf, bool *title, size_t *nr)
{
	bool is_title = *title;
	size_t stat_nr = *nr;
	char *name = strtok(line_buf, " ");
	assert(name != NULL);
	assert(stat_nr < MAX_STAT);

	Stats *stats = g_stats + stat_nr;
	if (is_title) { /* only save the name when is title line */
		strncpy(stats->name, name, MAX_NAME);
		size_t len = strlen(stats->name);
		stats->name[len - 1] = '\0'; /* remove ‘:’ */
	}

	char *data = strtok(NULL, " ");
	size_t i = 0;
	StatsItem *item = stats->item;
	while (data) {
		if (is_title) {
			strncpy(item[i].title, data, MAX_NAME);
		} else {
			item[i].val = strtol(data, NULL, 10);
		}
		data = strtok(NULL, " ");
		i++;
	}

	*title = !is_title;
	if (*title) { /* current stats have done, move to next */
		stats->item_cnt = i;
		(*nr)++;
	}

	return 0;
}

void dump_single_stat(Stats *stats, size_t nr)
{
	size_t newline_cnt = 0;
	printf("%s:%ld\n", stats[nr].name, stats[nr].item_cnt);
	for (size_t i = 0; i < stats[nr].item_cnt; i++) {
		StatsItem *item = stats[nr].item;
		printf("%-25s:%-10ld", item[i].title, item[i].val);
		newline_cnt++;
		if ((newline_cnt % 5) == 0) {
			printf("\n");
		}
	}
	printf("\n");

	return;
}

/* dump all stat infomation */
void dump_all_stats(Stats *stats)
{
	for (size_t nr = 0; nr < MAX_STAT; nr++) {
		printf("\n");
		dump_single_stat(stats, nr);
	}
}

void dump_stats_by_name(Stats *stats, char *name)
{
	size_t nr = 0;
	for (nr = 0; nr < MAX_STAT; nr++) {
		if (strcmp(stats[nr].name, name) == 0)
			break;
	}

	if (nr == MAX_STAT) {
		printf("invalid name:%s\n", name);
		return;
	}

	dump_single_stat(stats, nr);
	return;
}

int main(int argc, char *argv[])
{
	char line_buf[MAX_CHAR_PER_LINE] = { 0 };

	size_t stat_nr = 0;
	bool is_title = true;
	int ret = -1;
	while (fgets(line_buf, sizeof(line_buf), stdin) != NULL) {
		size_t buf_len = strlen(line_buf);
		line_buf[buf_len - 1] = '\0'; /* remove the newline  */

		ret = parse_line(line_buf, &is_title, &stat_nr);
		assert(ret == 0);

		line_buf[0] = '\0';
	}

	if (argc == 2) { /* means we should filter with the name */
		dump_stats_by_name(g_stats, argv[1]);
	} else {
		dump_all_stats(g_stats);
	}

	return 0;
}
