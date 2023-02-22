/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __UDEV_RULES_H__
#define __UDEV_RULES_H__

typedef enum {
	OP_MATCH,        /* == */
	OP_NOMATCH,      /* != */
	_OP_TYPE_IS_MATCH,
	OP_ADD,          /* += */
	OP_REMOVE,       /* -= */
	OP_ASSIGN,       /* = */
	OP_ASSIGN_FINAL, /* := */
	_OP_TYPE_MAX,
} rule_op_t;

typedef enum {
	KEY_ACTION,
	KEY_ATTR,
	KEY_ATTRS,
	KEY_CONST,
	KEY_DEVPATH,
	KEY_DRIVER,
	KEY_DRIVERS,
	KEY_ENV,
	KEY_GOTO,
	KEY_GROUP,
	KEY_IMPORT,
	KEY_KERNEL,
	KEY_KERNELS,
	KEY_LABEL,
	KEY_MODE,
	KEY_NAME,
	KEY_OPTIONS,
	KEY_OWNER,
	KEY_PROGRAM,
	KEY_RESULT,
	KEY_RUN,
	KEY_SECLABEL,
	KEY_SUBSYSTEM,
	KEY_SUBSYSTEMS,
	KEY_SYMLINK,
	KEY_SYSCTL,
	KEY_TAG,
	KEY_TAGS,
	KEY_TEST,
	_KEY_TYPE_MAX,
} rule_key_t;

#include "list.h"
#include "udev-string.h"

struct rule_file {
	struct list_head list;
	char *name;
	int rules_nr;
};

struct rule;

struct rule_pair {
	struct list_head list;
	rule_key_t key;
	rule_op_t op;
	struct string *attr;
	struct string *value;
	struct rule *rule;
};

struct rule {
	struct list_head list;
	struct list_head pairs;
	int global_order;
	int line_nr;
	int has_goto;
	const struct rule_file *file;
};

struct rule_goto_label {
	struct list_head list;
	const struct rule *rule;
	const struct string *name;
};

enum {
	W_ERROR,
	W_LABEL_BEFORE_GOTO,
	W_UNUSED_LABELS,
	W_DUPLICATE_LABELS,
	_W_TYPE_MAX,
};

extern const char *warning_str[_W_TYPE_MAX];

struct rules_state {
	struct list_head *files;
	struct list_head *rules;

	struct list_head *gotos;
	struct list_head *labels;

	int global_rule_nr;
	struct rule_file *cur_file;
	struct rule *cur_rule;

	int warning[_W_TYPE_MAX];
	int show_external;

	int retcode;
};

extern int rules_readfile(char *filename, struct rules_state *state);
extern int rules_readdir(const char *dir, struct rules_state *state);

extern struct rule_goto_label *get_goto(struct rules_state *state);
extern struct rule_goto_label *get_label(struct rules_state *state);
extern void free_goto_label(struct list_head *head);
extern void check_goto_label(struct rules_state *state);

static inline void warning_update_retcode(struct rules_state *state)
{
	if (!state->retcode)
		state->retcode = state->warning[W_ERROR];
}

static inline bool isempty(const char *a) {
        return !a || a[0] == '\0';
}

#endif /* __UDEV_RULES_H__ */
