#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include "parser.h"

int n_parsers;
nmea_parser_module_s **parsers;

/**
 * Where to find the parser modules.
 * Can be overridden by env variable NMEA_PARSER_PATH
 */
#ifndef PARSER_PATH
#define PARSER_PATH "/usr/lib/nmea/"
#endif

#define FILENAME_MAX 255

static int
_get_so_files(const char *path, char **files)
{
	int j, i = 0;
	size_t len;
	char *name;
	DIR *d = NULL;
	struct dirent *dir;

	d = opendir(path);
	if (NULL == d) {
		return -1;
	}

	while (NULL != (dir = readdir(d))) {
		if ('.' == dir->d_name[0]) {
			continue;
		}

		len = strlen(dir->d_name);

#ifdef __APPLE__
		if (len < 6) {
			continue;
		}

		if (0 != strncmp(dir->d_name + len - 6, ".dylib", 6)) {
			continue;
		}
#else
		if (len < 3) {
			continue;
		}

		if (0 != strncmp(dir->d_name + len - 3, ".so", 3)) {
			continue;
		}
#endif

		name = malloc(FILENAME_MAX);
		if (NULL == name) {
			goto fail;
		}

		strcpy(name, path);
		strcat(name, dir->d_name);
		files[i++] = name;
	}

	if (NULL != d) {
		closedir(d);
	}

	return i;

fail:
	for (j = 0; j < i; i++) {
		free(files[j]);
	}
	if (NULL != d) {
		closedir(d);
	}

	return -1;
}

nmea_parser_module_s *
nmea_init_parser(const char *filename)
{
	nmea_parser_module_s *parser;
	void *plugin;
	init_f init;

	/* Allocate parser struct */
	parser = malloc(sizeof (nmea_parser_module_s));
	if (NULL == parser) {
		return (nmea_parser_module_s *) NULL;
	}

	plugin = dlopen(filename, RTLD_NOW);
	if (NULL == plugin) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->handle = plugin;

	init = dlsym(plugin, "init");
	if (NULL == init) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->allocate_data = dlsym(plugin, "allocate_data");
	if (NULL == parser->allocate_data) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->set_default = dlsym(plugin, "set_default");
	if (NULL == parser->set_default) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->free_data = dlsym(plugin, "free_data");
	if (NULL == parser->free_data) {
		return (nmea_parser_module_s *) NULL;
	}

	parser->parse = dlsym(plugin, "parse");
	if (NULL == parser->parse) {
		return (nmea_parser_module_s *) NULL;
	}

	if (-1 == init((nmea_parser_s *) parser)) {
		return (nmea_parser_module_s *) NULL;
	}

	return parser;
}

int
nmea_load_parsers()
{
	int i;
	char *files[255];
	char *parser_path;
	nmea_parser_module_s *parser;

	/* Get list of so files */
	parser_path = getenv("NMEA_PARSER_PATH");

	if (NULL == parser_path) {
		/* Use default path */
		parser_path = PARSER_PATH;
	}

	n_parsers = _get_so_files(parser_path, files);
	if (1 > n_parsers) {
		return -1;
	}

	/* Allocate parsers array */
	parsers = malloc((sizeof (nmea_parser_module_s *)) * n_parsers);
	if (NULL == parsers) {
		return -1;
	}
	memset(parsers, 0, (sizeof (nmea_parser_module_s *)) * n_parsers);

	i = n_parsers;
	while (i-- > 0) {
		parser = nmea_init_parser(files[i]);
		free(files[i]);

		if (NULL == parser) {
			return -1;
		}

		parsers[i] = parser;
	}

	return n_parsers;
}

void
nmea_unload_parsers()
{
	int i;

	for (i = 0; i < n_parsers; i++) {
		dlclose(parsers[i]->handle);
		free(parsers[i]);
	}

	free(parsers);
}

nmea_parser_module_s *
nmea_get_parser_by_type(nmea_t type)
{
	int i;

	for (i = 0; i < n_parsers; i++) {
		if (type == parsers[i]->parser.type) {
			return parsers[i];
		}
	}

	return (nmea_parser_module_s *) NULL;
}

nmea_parser_module_s *
nmea_get_parser_by_sentence(const char *sentence)
{
	int i;
	nmea_parser_module_s *parser;

	if (0 == strlen(sentence)) {
		return (nmea_parser_module_s *) NULL;
	}

	for (i = 0; i < n_parsers; i++) {
		parser = parsers[i];
		if (0 == strncmp(sentence + 1, parser->parser.type_word, NMEA_PREFIX_LENGTH)) {
			return parser;
		}
	}

	return (nmea_parser_module_s *) NULL;
}
