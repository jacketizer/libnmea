#include "parser.h"

#define PARSER_PATH "/usr/lib/nmea/"

static inline int
_get_so_files(const char *path, char **files)
{
	int len, i = 0;
	DIR *d;
	struct dirent *dir;

	d = opendir(path);
	if (NULL == d) {
		return -1;
	}

	while (NULL != (dir = readdir(d))) {
		if (dir->d_name == NULL || dir->d_name[0] == '.') {
			continue;
		}

		len = strlen(dir->d_name);
		if (len < 3) {
			continue;
		}

		if (0 != strncmp(dir->d_name + len - 3, ".so", 3)) {
			continue;
		}

		char *name = malloc(255);
		if (NULL == name) {
			return -1;
		}

		strcpy(name, path);
		strcat(name, dir->d_name);
		files[i++] = name;
	}

	return i;
}

nmea_sentence_parser_s *
nmea_init_parser(const char *filename)
{
	nmea_sentence_parser_s *parser;

	/* Allocate parser struct */
	parser = malloc(sizeof (nmea_sentence_parser_s));
	if (NULL == parser) {
		return (nmea_sentence_parser_s *) NULL;
	}

	void *plugin = dlopen(filename, RTLD_NOW);
	if (NULL == plugin) {
		return (nmea_sentence_parser_s *) NULL;
	}

	init_f init = dlsym(plugin, "init");
	if (NULL == init) {
		return (nmea_sentence_parser_s *) NULL;
	}

	parser->allocate_data = dlsym(plugin, "allocate_data");
	if (NULL == parser->allocate_data) {
		return (nmea_sentence_parser_s *) NULL;
	}

	parser->set_default = dlsym(plugin, "set_default");
	if (NULL == parser->set_default) {
		return (nmea_sentence_parser_s *) NULL;
	}

	parser->free_data = dlsym(plugin, "free_data");
	if (NULL == parser->free_data) {
		return (nmea_sentence_parser_s *) NULL;
	}

	parser->parse = dlsym(plugin, "parse");
	if (NULL == parser->parse) {
		return (nmea_sentence_parser_s *) NULL;
	}

	if (-1 == init((nmea_parser_s *) parser)) {
		return (nmea_sentence_parser_s *) NULL;
	}

	return parser;
}

int
nmea_load_parsers()
{
	int n_files, i;
	char *files[255];
	nmea_sentence_parser_s *parser;

	memset(parsers, 0, sizeof parsers);

	/* Get list of so files */
	n_files = _get_so_files(PARSER_PATH, files);
	if (1 > n_files) {
		return -1;
	}

	i = n_files;
	while (i-- > 0) {
		parser = nmea_init_parser(files[i]);
		if (NULL == parser) {
			return -1;
		}

		parsers[(int) parser->parser.type] = parser;
	}

	return n_files;
}

nmea_sentence_parser_s *
nmea_get_parser(nmea_t type)
{
	return parsers[(int) type];
}
