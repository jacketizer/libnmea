#include "parser.h"

int
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
nmea_create_parser(nmea_t type)
{
	nmea_sentence_parser_s *parser;

	/* Get file list */
	int n_files;
	char *files[255];

	n_files = _get_so_files("sentences/", files);
	if (1 > n_files) {
		return NULL;
	}

	while (n_files-- > 0) {
		printf("%s\n", files[n_files]);
	}

	/* Allocate parser struct */
	parser = malloc(sizeof(nmea_sentence_parser_s));
	if (NULL == parser) {
		return (nmea_sentence_parser_s *) NULL;
	}

	printf("Loading plugins...\n");
	void *plugin = dlopen("sentences/libgpgll.so", RTLD_NOW);
	if (NULL == plugin) {
		printf("Cannot load: %s\n", dlerror());
		return NULL;
	}
	printf("OK\n");

	init_f init = dlsym(plugin, "init");
	if (NULL == init) {
		printf("Cannot resolve init fn: %s\n", dlerror());
		return NULL;
	}
	init(parser);
	if (NULL == parser) {
		printf("Cannot init parser\n");
		return NULL;
	}

	parser->set_default = dlsym(plugin, "set_default");
	if (NULL == parser->set_default) {
		printf("Cannot resolve default fn: %s\n", dlerror());
		return NULL;
	}

	parser->free_data = dlsym(plugin, "free_data");
	if (NULL == parser->free_data) {
		printf("Cannot resolve free fn: %s\n", dlerror());
		return NULL;
	}

	parser->parse = dlsym(plugin, "parse");
	if (NULL == parser->parse) {
		printf("Cannot resolve parse fn: %s\n", dlerror());
		return NULL;
	}


	/* Set default values */
	parser->set_default(parser->data);

	return parser;
}

